#include "rogue.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define USERS_FILE "data/users.db"
#define SETTINGS_FILE "data/settings.cfg"

void ensure_data_dir(void){ mkdir("data",0755); mkdir("data/saves",0755); }
uint64_t hash_password(const char *s){ uint64_t h=1469598103934665603ULL; for(;*s;s++){h^=(unsigned char)*s;h*=1099511628211ULL;} return h; }
bool validate_password(const char*s){ bool up=false,lo=false,d=false; if(!s||strlen(s)<7)return false; for(;*s;s++){up|=isupper((unsigned char)*s);lo|=islower((unsigned char)*s);d|=isdigit((unsigned char)*s);} return up&&lo&&d; }
bool validate_email(const char*s){ if(!s)return false; const char *a=strchr(s,'@'),*dot=strrchr(s,'.'); return a&&dot&&a>s&&dot>a+1&&dot[1]&&strchr(a+1,'@')==NULL; }

static bool parse_user(const char *line,User*u){
    char hash[32]; User t={0}; if(sscanf(line,"%23[^|]|%63[^|]|%31[^|]|%ld|%ld|%d|%d|%ld",t.username,t.email,hash,&t.total_score,&t.total_gold,&t.games_finished,&t.wins,&t.first_played)!=8)return false;
    t.password_hash=strtoull(hash,NULL,16); *u=t; return true;
}

int users_read(User *users,int cap){ ensure_data_dir(); FILE*f=fopen(USERS_FILE,"r"); if(!f)return 0; char line[512];int n=0;while(n<cap&&fgets(line,sizeof line,f))if(parse_user(line,&users[n]))n++;fclose(f);return n; }
bool user_load(const char *username,User*out){ User us[MAX_USERS];int n=users_read(us,MAX_USERS);for(int i=0;i<n;i++)if(strcmp(us[i].username,username)==0){if(out)*out=us[i];return true;}return false; }
static bool write_users(User *us,int n){ FILE*f=fopen(USERS_FILE,"w");if(!f)return false;for(int i=0;i<n;i++)fprintf(f,"%s|%s|%016llx|%ld|%ld|%d|%d|%ld\n",us[i].username,us[i].email,(unsigned long long)us[i].password_hash,us[i].total_score,us[i].total_gold,us[i].games_finished,us[i].wins,us[i].first_played);fclose(f);return true; }
bool user_create(const char *u,const char *p,const char *e,User *out){
    if(!u || strlen(u)<3 || strlen(u)>=NAME_LEN || !validate_password(p) || !validate_email(e) || user_load(u,NULL)) return false;
    for(size_t i=0; u[i]; i++) if(!isalnum((unsigned char)u[i]) && u[i]!='_' && u[i]!='-') return false;
    User us[MAX_USERS];
    int n=users_read(us,MAX_USERS);
    if(n>=MAX_USERS) return false;
    User x={0};
    snprintf(x.username,sizeof x.username,"%s",u);
    snprintf(x.email,sizeof x.email,"%s",e);
    x.password_hash=hash_password(p);
    x.first_played=time(NULL);
    us[n++]=x;
    if(!write_users(us,n)) return false;
    if(out) *out=x;
    return true;
}
bool user_auth(const char*u,const char*p,User*out){User x;if(!user_load(u,&x)||x.password_hash!=hash_password(p))return false;if(out)*out=x;return true;}
void user_record_game(const Game*g){ if(strcmp(g->username,"guest")==0)return;User us[MAX_USERS];int n=users_read(us,MAX_USERS);for(int i=0;i<n;i++)if(strcmp(us[i].username,g->username)==0){us[i].total_score+=g->player.score;us[i].total_gold+=g->player.gold;us[i].games_finished++;if(g->won)us[i].wins++;write_users(us,n);return;} }
static void save_path(const char*u,char*p,size_t n){char safe[NAME_LEN];size_t j=0;for(size_t i=0;u[i]&&j<sizeof safe-1;i++)if(isalnum((unsigned char)u[i])||u[i]=='_'||u[i]=='-')safe[j++]=u[i];safe[j]=0;snprintf(p,n,"data/saves/%s.sav",*safe?safe:"guest");}
bool save_game(const Game*g){ensure_data_dir();char p[128];save_path(g->username,p,sizeof p);FILE*f=fopen(p,"wb");if(!f)return false;bool ok=fwrite(g,sizeof *g,1,f)==1;fclose(f);return ok;}
bool load_game(const char*u,Game*g){char p[128];save_path(u,p,sizeof p);FILE*f=fopen(p,"rb");if(!f)return false;bool ok=fread(g,sizeof *g,1,f)==1;fclose(f);if(!ok||g->magic!=SAVE_MAGIC||g->version!=SAVE_VERSION)return false;srand(g->seed+g->turn);update_visibility(g);return true;}
void delete_save(const char*u){char p[128];save_path(u,p,sizeof p);remove(p);}
Settings settings_load(void){Settings s={1,2};FILE*f=fopen(SETTINGS_FILE,"r");if(f){fscanf(f,"%d %d",&s.difficulty,&s.player_color);fclose(f);}if(s.difficulty<0||s.difficulty>2)s.difficulty=1;if(s.player_color<1||s.player_color>6)s.player_color=2;return s;}
void settings_save(const Settings*s){ensure_data_dir();FILE*f=fopen(SETTINGS_FILE,"w");if(f){fprintf(f,"%d %d\n",s->difficulty,s->player_color);fclose(f);}}
