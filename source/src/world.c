#include "rogue.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int irand(int a, int b){ return a + rand() % (b-a+1); }
static int signi(int v){ return (v>0)-(v<0); }
static bool inside(int x,int y){ return x>0 && x<MAP_W-1 && y>0 && y<MAP_H-1; }
static bool walkable(char t){ return t==T_FLOOR||t==T_CORRIDOR||t==T_DOOR||t==T_STAIRS||t==T_TRAP||t==T_GOLD||t==T_FOOD||t==T_POTION||t==T_WEAPON||t==T_SCROLL; }
static Monster *monster_at(Game *g,int x,int y){ for(int i=0;i<g->monster_count;i++) if(g->monsters[i].alive&&g->monsters[i].x==x&&g->monsters[i].y==y) return &g->monsters[i]; return NULL; }
static bool occupied(Game *g,int x,int y){ return monster_at(g,x,y)!=NULL || (g->player.x==x&&g->player.y==y); }

void add_message(Game *g,const char *fmt,...){
    if(g->message_count<MAX_MESSAGES) g->message_count++;
    for(int i=g->message_count-1;i>0;i--) strcpy(g->messages[i],g->messages[i-1]);
    va_list ap; va_start(ap,fmt); vsnprintf(g->messages[0],sizeof(g->messages[0]),fmt,ap); va_end(ap);
}

const char *weapon_name(Weapon w){ static const char *n[]={"Mace","Dagger","Sword","Bow","Magic Wand"}; return (w>=0&&w<W_COUNT)?n[w]:"Unknown"; }
int weapon_damage(Weapon w){ static const int d[]={5,8,11,9,14}; return (w>=0&&w<W_COUNT)?d[w]:4; }

static void clear_map(Game *g){ memset(g->tiles,T_VOID,sizeof(g->tiles)); memset(g->seen,0,sizeof(g->seen)); memset(g->visible,0,sizeof(g->visible)); g->room_count=0; g->monster_count=0; }

static bool room_fits(Game *g,int x,int y,int w,int h){
    if(x<2||y<2||x+w>=MAP_W-2||y+h>=MAP_H-2) return false;
    for(int yy=y-1;yy<=y+h;yy++) for(int xx=x-1;xx<=x+w;xx++) if(g->tiles[yy][xx]!=T_VOID) return false;
    return true;
}

static void carve_room(Game *g,Room r){
    for(int y=r.y;y<r.y+r.h;y++) for(int x=r.x;x<r.x+r.w;x++){
        if(y==r.y||y==r.y+r.h-1) g->tiles[y][x]=T_WALL_H;
        else if(x==r.x||x==r.x+r.w-1) g->tiles[y][x]=T_WALL_V;
        else g->tiles[y][x]=T_FLOOR;
    }
    if(r.w>=9&&r.h>=7&&rand()%3==0) g->tiles[r.y+r.h/2][r.x+r.w/2]=T_PILLAR;
    if(rand()%4==0 && r.w>8){ int wx=r.x+irand(2,r.w-3); g->tiles[r.y][wx]=T_WINDOW; }
}

static void corridor(Game *g,Point a,Point b){
    int x=a.x,y=a.y; bool horiz=rand()%2;
    while(x!=b.x||y!=b.y){
        if((horiz&&x!=b.x)||y==b.y) x+=signi(b.x-x); else y+=signi(b.y-y);
        if(!inside(x,y)) break;
        char *t=&g->tiles[y][x];
        if(*t==T_VOID) *t=T_CORRIDOR;
        else if(*t==T_WALL_H||*t==T_WALL_V||*t==T_WINDOW) *t=T_DOOR;
        if(rand()%8==0) horiz=!horiz;
    }
}

static Point center(Room *r){ Point p={r->x+r->w/2,r->y+r->h/2}; return p; }
static Point random_floor(Game *g){
    for(int tries=0;tries<2000;tries++){ int x=irand(2,MAP_W-3),y=irand(2,MAP_H-3); if(g->tiles[y][x]==T_FLOOR&&!occupied(g,x,y)) return (Point){x,y}; }
    return center(&g->rooms[0]);
}

static void place_feature(Game *g,char tile,int count){ for(int i=0;i<count;i++){ Point p=random_floor(g); g->tiles[p.y][p.x]=tile; } }

static void add_monster_kind(Game *g,MonsterKind k){
    if(g->monster_count>=MAX_MONSTERS) return;
    Point p=random_floor(g);
    Monster *m=&g->monsters[g->monster_count++];
    int base=g->floor + g->difficulty;
    *m=(Monster){.x=p.x,.y=p.y,.alive=1,.awake=0,.vision=7}; m->kind=k;
    switch(k){
        case M_DEMON: m->max_hp=13+base*2; m->damage=5+base; break;
        case M_FIRE: m->max_hp=9+base; m->damage=4+base; m->vision=9; break;
        case M_GIANT: m->max_hp=20+base*3; m->damage=7+base; m->vision=5; break;
        case M_SNAKE: m->max_hp=7+base; m->damage=3+base; m->vision=10; break;
        default: m->max_hp=12+base*2; m->damage=4+base; break;
    }
    m->hp=m->max_hp;
}

void generate_floor(Game *g){
    clear_map(g);
    int desired=6+MIN(g->floor,2); int attempts=0;
    while(g->room_count<desired && attempts++<500){
        int w=irand(8,16),h=irand(5,9),x=irand(2,MAP_W-w-3),y=irand(2,MAP_H-h-3);
        if(!room_fits(g,x,y,w,h)) continue;
        Room r={x,y,w,h,rand()%4}; g->rooms[g->room_count++]=r; carve_room(g,r);
    }
    if(g->room_count<4){ /* deterministic fallback */
        clear_map(g); int coords[6][4]={{3,2,18,8},{28,2,18,8},{53,2,18,8},{3,14,18,8},{28,14,18,8},{53,14,18,8}};
        for(int i=0;i<6;i++){ Room r={coords[i][0],coords[i][1],coords[i][2],coords[i][3],i%4}; g->rooms[g->room_count++]=r; carve_room(g,r); }
    }
    for(int i=1;i<g->room_count;i++) corridor(g,center(&g->rooms[i-1]),center(&g->rooms[i]));
    for(int i=0;i<g->room_count/3;i++){ int a=rand()%g->room_count,b=rand()%g->room_count; if(a!=b) corridor(g,center(&g->rooms[a]),center(&g->rooms[b])); }

    Point start=center(&g->rooms[0]); g->player.x=start.x; g->player.y=start.y;
    Point stair=center(&g->rooms[g->room_count-1]); g->tiles[stair.y][stair.x]=T_STAIRS;
    place_feature(g,T_GOLD,5+g->floor*2); place_feature(g,T_FOOD,3); place_feature(g,T_POTION,2); place_feature(g,T_WEAPON,1+g->floor/2); place_feature(g,T_SCROLL,2); place_feature(g,T_TRAP,2+g->difficulty);
    int mc=5+g->floor*2+g->difficulty*2;
    MonsterKind kinds[]={M_SNAKE,M_UNDEAD,M_FIRE,M_DEMON,M_GIANT};
    for(int i=0;i<mc;i++) add_monster_kind(g,kinds[rand()%(2+MIN(g->floor,3))]);
    add_message(g,"Floor %d: the dungeon shifts around you...",g->floor);
    update_visibility(g);
}

void game_new(Game *g,const char *username,const Settings *settings){
    memset(g,0,sizeof(*g)); g->magic=SAVE_MAGIC; g->version=SAVE_VERSION; g->seed=(unsigned)time(NULL)^(unsigned)(uintptr_t)g; srand(g->seed);
    g->floor=1; g->difficulty=settings?settings->difficulty:1; g->player_color=settings?settings->player_color:2; g->started_at=time(NULL);
    snprintf(g->username,sizeof(g->username),"%s",username&&*username?username:"guest");
    g->player=(Player){.hp=35,.max_hp=35,.hunger=100,.food=1,.potions=1,.arrows=8,.weapon=W_MACE,.owned_weapons=1u<<W_MACE};
    generate_floor(g); add_message(g,"Welcome, %s. Find the stairs and survive.",g->username);
}

static bool los(Game *g,int x0,int y0,int x1,int y1){
    int dx=abs(x1-x0),sx=x0<x1?1:-1,dy=-abs(y1-y0),sy=y0<y1?1:-1,err=dx+dy;
    for(;;){ if(x0==x1&&y0==y1) return true; if(!(x0==g->player.x&&y0==g->player.y)) { char t=g->tiles[y0][x0]; if(t==T_WALL_H||t==T_WALL_V||t==T_PILLAR) return false; }
        int e2=2*err; if(e2>=dy){err+=dy;x0+=sx;} if(e2<=dx){err+=dx;y0+=sy;} if(!inside(x0,y0)&&!(x0==x1&&y0==y1)) return false; }
}

void update_visibility(Game *g){
    memset(g->visible,0,sizeof(g->visible)); int r=8;
    for(int y=0;y<MAP_H;y++) for(int x=0;x<MAP_W;x++){
        int dx=x-g->player.x,dy=y-g->player.y; if(dx*dx+dy*dy<=r*r && los(g,g->player.x,g->player.y,x,y)){ g->visible[y][x]=1; g->seen[y][x]=1; }
    }
}

static void damage_player(Game *g,int dmg,const char *source){
    if(g->player.shield_turns>0) dmg=MAX(1,dmg/2);
    g->player.hp-=dmg;
    add_message(g,"%s hits you for %d.",source,dmg);
    if(g->player.hp<=0){ g->player.hp=0; g->game_over=true; add_message(g,"You fall in the darkness..."); }
}

static void attack_monster(Game *g,Monster *m,int bonus){
    int dmg=weapon_damage(g->player.weapon)+bonus+irand(0,3); if(g->player.power_turns>0) dmg+=5; m->hp-=dmg; m->awake=1; add_message(g,"You strike %c for %d damage.",(char)m->kind,dmg);
    if(m->hp<=0){ m->alive=0; g->player.kills++; g->player.xp+=10+g->floor*3; g->player.score+=25+g->floor*10; add_message(g,"The %c is defeated.",(char)m->kind); }
}

static void pickup(Game *g,int x,int y){ char t=g->tiles[y][x];
    switch(t){
        case T_GOLD:{ int n=irand(5,20)*g->floor; g->player.gold+=n; g->player.score+=n; add_message(g,"You collect %d gold.",n); break; }
        case T_FOOD:g->player.food++;add_message(g,"You pack a ration.");break;
        case T_POTION:g->player.potions++;add_message(g,"You found a healing potion.");break;
        case T_SCROLL:g->player.scrolls++;add_message(g,"You found a rune scroll.");break;
        case T_WEAPON:{ Weapon w=(Weapon)irand(1,W_COUNT-1); g->player.owned_weapons|=1u<<w; if(w==W_BOW) g->player.arrows+=irand(4,10); add_message(g,"Found %s%s.",weapon_name(w),w==W_BOW?" and arrows":""); break; }
        case T_TRAP:{ int d=irand(3,8)+g->difficulty; add_message(g,"A hidden trap snaps!"); damage_player(g,d,"Trap"); break; }
        default:return;
    }
    g->tiles[y][x]=T_FLOOR;
}

static bool try_move(Game *g,int dx,int dy){
    int nx=g->player.x+dx,ny=g->player.y+dy; if(!inside(nx,ny)) return false;
    Monster *m=monster_at(g,nx,ny); if(m){ attack_monster(g,m,0); return true; }
    if(!walkable(g->tiles[ny][nx])){ add_message(g,"A solid barrier blocks the way."); return false; }
    g->player.x=nx;g->player.y=ny; pickup(g,nx,ny); return true;
}

static Point dir_from_key(int ch){
    if(ch==KEY_UP||ch=='k'||ch=='K') return (Point){0,-1};
    if(ch==KEY_DOWN||ch=='j'||ch=='J') return (Point){0,1};
    if(ch==KEY_LEFT||ch=='h'||ch=='H') return (Point){-1,0};
    if(ch==KEY_RIGHT||ch=='l'||ch=='L') return (Point){1,0};
    if(ch=='y'||ch=='Y') return (Point){-1,-1};
    if(ch=='u'||ch=='U') return (Point){1,-1};
    if(ch=='b'||ch=='B') return (Point){-1,1};
    if(ch=='n'||ch=='N') return (Point){1,1};
    return (Point){0,0};
}

static void eat(Game *g){ if(g->player.food<=0){add_message(g,"Your pack has no food.");return;} g->player.food--; g->player.hunger=MIN(100,g->player.hunger+45); g->player.hp=MIN(g->player.max_hp,g->player.hp+4); add_message(g,"You eat a ration and recover a little."); }
static void potion(Game *g){ if(g->player.potions<=0){add_message(g,"No potions left.");return;} g->player.potions--; int heal=12+g->floor*2; g->player.hp=MIN(g->player.max_hp,g->player.hp+heal); add_message(g,"Warmth returns to your body (+%d HP).",heal); }
static void scroll(Game *g){ if(g->player.scrolls<=0){add_message(g,"No rune scrolls left.");return;} g->player.scrolls--; int e=rand()%3; if(e==0){g->player.shield_turns=20;add_message(g,"Arcane armor surrounds you.");} else if(e==1){g->player.power_turns=18;add_message(g,"Your weapon burns with power.");} else {g->player.haste_turns=15;add_message(g,"Time seems to slow around you.");} }
static void cycle_weapon(Game *g){ for(int i=1;i<=W_COUNT;i++){ Weapon w=(Weapon)((g->player.weapon+i)%W_COUNT); if(g->player.owned_weapons&(1u<<w)){g->player.weapon=w;add_message(g,"Equipped %s.",weapon_name(w));return;} } }

static bool ranged_attack(Game *g,Point d){
    if(d.x==0&&d.y==0) return false;
    int range=0,dmg=0;
    if(g->player.weapon==W_BOW){ if(g->player.arrows<=0){add_message(g,"Your quiver is empty.");return false;} g->player.arrows--;range=9;dmg=weapon_damage(W_BOW); }
    else if(g->player.weapon==W_WAND){range=7;dmg=weapon_damage(W_WAND);} else {add_message(g,"Equip Bow or Magic Wand to fire.");return false;}
    int x=g->player.x,y=g->player.y; for(int i=0;i<range;i++){ x+=d.x;y+=d.y; if(!inside(x,y)||!walkable(g->tiles[y][x])) break; Monster*m=monster_at(g,x,y); if(m){attack_monster(g,m,dmg/2);return true;} }
    add_message(g,"Your shot disappears into the dark."); return true;
}

void game_command(Game *g,int ch){
    bool turn=false; Point d=dir_from_key(ch);
    if(d.x||d.y) turn=try_move(g,d.x,d.y);
    else if(ch=='e'||ch=='E'){eat(g);turn=true;} else if(ch=='p'||ch=='P'){potion(g);turn=true;} else if(ch=='z'||ch=='Z'){scroll(g);turn=true;} else if(ch=='w'||ch=='W'){cycle_weapon(g);}
    else if(ch=='>'){ if(g->tiles[g->player.y][g->player.x]==T_STAIRS){ if(g->floor>=FINAL_FLOOR){g->won=true;g->game_over=true;g->player.score+=1000;add_message(g,"You escape with the Crown of Rogue! Victory!");} else {g->floor++;g->player.score+=150;generate_floor(g);} turn=true; } else add_message(g,"There are no stairs here."); }
    else if(ch=='f'||ch=='F'){ add_message(g,"Fire: choose a direction."); update_visibility(g); }
    if(turn&&!g->game_over){
        g->turn++; if(g->player.haste_turns>0)g->player.haste_turns--; if(g->player.shield_turns>0)g->player.shield_turns--; if(g->player.power_turns>0)g->player.power_turns--;
        if(g->turn%6==0){g->player.hunger--; if(g->player.hunger<=0){g->player.hunger=0;damage_player(g,1,"Starvation");}}
        if(g->player.haste_turns==0 || g->turn%2==0) game_tick_monsters(g);
        update_visibility(g);
    }
}

static int dist2(int x1,int y1,int x2,int y2){int a=x1-x2,b=y1-y2;return a*a+b*b;}
void game_tick_monsters(Game *g){
    for(int i=0;i<g->monster_count&&!g->game_over;i++){ Monster*m=&g->monsters[i]; if(!m->alive)continue; int d2=dist2(m->x,m->y,g->player.x,g->player.y); if(d2<=m->vision*m->vision&&los(g,m->x,m->y,g->player.x,g->player.y))m->awake=1; if(!m->awake)continue;
        int dx=signi(g->player.x-m->x),dy=signi(g->player.y-m->y); if(abs(g->player.x-m->x)<=1&&abs(g->player.y-m->y)<=1){damage_player(g,m->damage+irand(0,2),(m->kind==M_GIANT?"Giant":m->kind==M_DEMON?"Demon":m->kind==M_FIRE?"Fire beast":m->kind==M_SNAKE?"Snake":"Undead"));continue;}
        int nx=m->x+dx,ny=m->y+dy; if(rand()%3==0){ if(rand()%2)dx=0;else dy=0; nx=m->x+dx;ny=m->y+dy; }
        if(inside(nx,ny)&&walkable(g->tiles[ny][nx])&&!occupied(g,nx,ny)){m->x=nx;m->y=ny;}
    }
}

/* called by UI after 'f' because directional input belongs to the UI event stream */
void world_fire_direction(Game *g,int ch){ Point d=dir_from_key(ch); if(ranged_attack(g,d)){g->turn++;game_tick_monsters(g);update_visibility(g);} }
