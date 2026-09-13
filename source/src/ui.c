#include "rogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void center_text(int y,const char*s,int attr){int x=(COLS-(int)strlen(s))/2;if(x<0)x=0;attron(attr);mvaddnstr(y,x,s,COLS-1);attroff(attr);}
void ui_init(void){ initscr(); cbreak(); noecho(); keypad(stdscr,TRUE); curs_set(0); start_color(); use_default_colors();
    init_pair(1,COLOR_WHITE,-1);init_pair(2,COLOR_CYAN,-1);init_pair(3,COLOR_YELLOW,-1);init_pair(4,COLOR_RED,-1);init_pair(5,COLOR_GREEN,-1);init_pair(6,COLOR_MAGENTA,-1);init_pair(7,COLOR_BLUE,-1);init_pair(8,COLOR_BLACK,COLOR_WHITE);
}
void ui_shutdown(void){endwin();}
void ui_title(void){ clear(); const char *logo[]={"============================================","              R O G U E   D E L U X E","============================================","         procedural terminal adventure","              C11 + ncurses edition"};
    int y=MAX(1,LINES/2-8);for(int i=0;i<5;i++)center_text(y+i,logo[i],COLOR_PAIR(i%2?3:2)|A_BOLD);center_text(y+7,"A polished ncurses roguelike",COLOR_PAIR(1)|A_DIM);center_text(y+10,"Arrow keys / HJKL navigate  -  Enter selects",COLOR_PAIR(5));refresh();
}
int ui_menu(const char*title,const char*subtitle,const char*items[],int count,int selected){
    int ch; if(selected<0)selected=0; for(;;){clear(); int w=52,h=count+8,y=MAX(1,(LINES-h)/2),x=MAX(0,(COLS-w)/2); box(stdscr,0,0); center_text(y,title,COLOR_PAIR(2)|A_BOLD); if(subtitle)center_text(y+2,subtitle,COLOR_PAIR(1)|A_DIM);
        for(int i=0;i<count;i++){int yy=y+4+i; if(i==selected)attron(COLOR_PAIR(8)|A_BOLD); mvprintw(yy,x+5,"%c %-38s",i==selected?'>':' ',items[i]); if(i==selected)attroff(COLOR_PAIR(8)|A_BOLD);} center_text(y+h-2,"Esc goes back",COLOR_PAIR(1)|A_DIM);refresh();ch=getch();
        if(ch==KEY_UP||ch=='k')selected=(selected+count-1)%count;else if(ch==KEY_DOWN||ch=='j')selected=(selected+1)%count;else if(ch=='\n'||ch==KEY_ENTER)return selected;else if(ch==27)return -1; }
}
void ui_message_box(const char*title,const char*text){clear();box(stdscr,0,0);center_text(MAX(2,LINES/2-3),title,COLOR_PAIR(3)|A_BOLD);center_text(MAX(4,LINES/2),text,COLOR_PAIR(1));center_text(MAX(6,LINES/2+3),"Press any key",COLOR_PAIR(2)|A_DIM);refresh();getch();}
bool ui_input(const char*title,const char*label,char*buf,int maxlen,bool secret){clear();box(stdscr,0,0);center_text(MAX(2,LINES/2-4),title,COLOR_PAIR(2)|A_BOLD);int y=LINES/2,x=MAX(2,(COLS-54)/2);mvprintw(y,x,"%s",label);move(y+2,x);curs_set(1);echo(); if(secret) noecho(); int rc=getnstr(buf,maxlen-1);noecho();curs_set(0);return rc!=ERR;}
static int tile_color(char t,int theme){ if(t==T_GOLD)return 3;if(t==T_FOOD||t==T_POTION)return 5;if(t==T_WEAPON||t==T_SCROLL)return 6;if(t==T_TRAP)return 4;if(t==T_STAIRS)return 2;if(t==T_DOOR)return 3;if(t==T_CORRIDOR)return 7;if(t==T_FLOOR)return 1+(theme%3);return 1; }
static int room_theme_at(const Game*g,int x,int y){for(int i=0;i<g->room_count;i++){Room r=g->rooms[i];if(x>=r.x&&x<r.x+r.w&&y>=r.y&&y<r.y+r.h)return r.theme;}return 0;}
void ui_draw_game(const Game*g){
    clear(); if(LINES<31||COLS<98){center_text(LINES/2-1,"Terminal too small for Rogue Deluxe.",COLOR_PAIR(4)|A_BOLD);center_text(LINES/2+1,"Resize to at least 98 x 31.",COLOR_PAIR(3));refresh();return;}
    int ox=1,oy=2; mvprintw(0,2,"ROGUE DELUXE  |  %s  |  Floor %d/%d  Turn %d",g->username,g->floor,FINAL_FLOOR,g->turn); mvhline(1,0,ACS_HLINE,COLS);
    for(int y=0;y<MAP_H;y++)for(int x=0;x<MAP_W;x++){if(!g->seen[y][x])continue;char t=g->tiles[y][x];int attr=COLOR_PAIR(tile_color(t,room_theme_at(g,x,y)));if(!g->visible[y][x])attr|=A_DIM;attron(attr);mvaddch(oy+y,ox+x,t);attroff(attr);}
    for(int i=0;i<g->monster_count;i++){const Monster*m=&g->monsters[i];if(m->alive&&g->visible[m->y][m->x]){int c=m->kind==M_FIRE?4:m->kind==M_GIANT?3:m->kind==M_DEMON?6:5;attron(COLOR_PAIR(c)|A_BOLD);mvaddch(oy+m->y,ox+m->x,(char)m->kind);attroff(COLOR_PAIR(c)|A_BOLD);}}
    attron(COLOR_PAIR(g->player_color)|A_BOLD);mvaddch(oy+g->player.y,ox+g->player.x,'@');attroff(COLOR_PAIR(g->player_color)|A_BOLD);
    int sx=MAP_W+3; mvprintw(2,sx,"+-- ADVENTURER --+");mvprintw(4,sx,"HP      %3d / %-3d",g->player.hp,g->player.max_hp);mvprintw(5,sx,"Hunger  %3d / 100",g->player.hunger);mvprintw(6,sx,"Gold    %-7d",g->player.gold);mvprintw(7,sx,"Score   %-7d",g->player.score);mvprintw(8,sx,"XP      %-7d",g->player.xp);mvprintw(9,sx,"Kills   %-7d",g->player.kills);
    mvprintw(11,sx,"Weapon: %-11s",weapon_name(g->player.weapon));mvprintw(12,sx,"Arrows: %-3d",g->player.arrows);mvprintw(14,sx,"Food:%-2d Potions:%-2d",g->player.food,g->player.potions);mvprintw(15,sx,"Runes:%-2d",g->player.scrolls);
    if(g->player.shield_turns) mvprintw(17,sx,"[Shield %d]",g->player.shield_turns);
    if(g->player.power_turns) mvprintw(18,sx,"[Power  %d]",g->player.power_turns);
    if(g->player.haste_turns) mvprintw(19,sx,"[Haste  %d]",g->player.haste_turns);
    mvhline(oy+MAP_H,0,ACS_HLINE,COLS);int my=oy+MAP_H+1;for(int i=0;i<MIN(g->message_count,3);i++){int c=i==0?3:1;attron(COLOR_PAIR(c)|(i?A_DIM:A_NORMAL));mvaddnstr(my+i,2,g->messages[i],COLS-4);attroff(COLOR_PAIR(c)|(i?A_DIM:A_NORMAL));}
    mvprintw(LINES-1,1,"Move: arrows/HJKL  E:eat P:potion Z:rune W:weapon F:fire >:stairs I:inventory ?:help Esc:pause");refresh();
}
void ui_help(void){clear();box(stdscr,0,0);center_text(2,"CONTROLS & LEGEND",COLOR_PAIR(2)|A_BOLD);const char*lines[]={"Movement: Arrow keys or H J K L; diagonals Y U B N","Combat: bump into enemies. F + direction fires Bow/Wand.","E eat ration   P drink potion   Z read rune   W cycle weapon","> descend stairs   I inventory   S quick-save   Esc pause","","Map: @ you   . floor   # corridor   + door   > stairs   ^ trap","     $ gold  % food  ! potion  ) weapon  ? rune","Enemies: S snake  U undead  F fire beast  D demon  G giant","","Goal: reach floor 4 and escape alive. Exploration, gold and kills add score."};int y=5;for(int i=0;i<10;i++)mvaddstr(y+i,MAX(2,(COLS-72)/2),lines[i]);center_text(y+12,"Press any key",COLOR_PAIR(3));refresh();getch();}
void ui_inventory(const Game*g){clear();box(stdscr,0,0);center_text(2,"INVENTORY",COLOR_PAIR(3)|A_BOLD);int x=MAX(4,(COLS-55)/2),y=5;mvprintw(y++,x,"Equipped: %s (base damage %d)",weapon_name(g->player.weapon),weapon_damage(g->player.weapon));mvprintw(y++,x,"Weapons owned:");for(int w=0;w<W_COUNT;w++)if(g->player.owned_weapons&(1u<<w))mvprintw(y++,x,"  %c %s%s",(Weapon)w==g->player.weapon?'*':'-',weapon_name((Weapon)w),w==W_BOW?" (uses arrows)":"");y++;mvprintw(y++,x,"Food rations : %d",g->player.food);mvprintw(y++,x,"Potions      : %d",g->player.potions);mvprintw(y++,x,"Rune scrolls : %d",g->player.scrolls);mvprintw(y++,x,"Arrows       : %d",g->player.arrows);center_text(y+3,"Press any key",COLOR_PAIR(2)|A_DIM);refresh();getch();}
int ui_pause_menu(void){const char*it[]={"Resume","Save game","Help","Quit to main menu"};return ui_menu("PAUSED","The dungeon waits.",it,4,0);}
static int cmp_user(const void*a,const void*b){const User*x=a,*y=b;if(y->total_score>x->total_score)return 1;if(y->total_score<x->total_score)return -1;return 0;}
void ui_scoreboard(const char*cur){User u[MAX_USERS];int n=users_read(u,MAX_USERS);qsort(u,n,sizeof(User),cmp_user);clear();box(stdscr,0,0);center_text(2,"HALL OF LEGENDS",COLOR_PAIR(3)|A_BOLD);int x=MAX(2,(COLS-82)/2);mvprintw(4,x,"#   Username                 Score       Gold   Games  Wins");mvhline(5,x,ACS_HLINE,70);for(int i=0;i<n&&i<18;i++){int attr=i==0?COLOR_PAIR(3)|A_BOLD:i==1?COLOR_PAIR(2)|A_BOLD:i==2?COLOR_PAIR(6)|A_BOLD:COLOR_PAIR(1);if(cur&&strcmp(cur,u[i].username)==0)attr|=A_REVERSE;attron(attr);mvprintw(6+i,x,"%-3d %-22s %10ld %10ld %6d %5d",i+1,u[i].username,u[i].total_score,u[i].total_gold,u[i].games_finished,u[i].wins);attroff(attr);}if(n==0)center_text(8,"No legends yet. Be the first.",COLOR_PAIR(1)|A_DIM);center_text(LINES-2,"Press any key",COLOR_PAIR(2));refresh();getch();}
void ui_settings(Settings*s){const char*diff[]={"Easy","Normal","Hard"};for(;;){char a[64],b[64];snprintf(a,sizeof a,"Difficulty: %s",diff[s->difficulty]);snprintf(b,sizeof b,"Hero color: %d",s->player_color);const char*it[]={a,b,"Save & back"};int c=ui_menu("SETTINGS","Customize your next run",it,3,0);if(c<0||c==2){settings_save(s);return;}if(c==0)s->difficulty=(s->difficulty+1)%3;else if(c==1)s->player_color=s->player_color%6+1;}}
