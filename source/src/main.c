#include "rogue.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool register_user(User *u){char n[NAME_LEN]={0},p[64]={0},e[EMAIL_LEN]={0};ui_input("CREATE ACCOUNT","Username (3+ chars):",n,sizeof n,false);if(user_load(n,NULL)){ui_message_box("Account","That username already exists.");return false;}ui_input("CREATE ACCOUNT","Password (7+, upper/lower/digit):",p,sizeof p,true);if(!validate_password(p)){ui_message_box("Account","Password must contain upper, lower and digit.");return false;}ui_input("CREATE ACCOUNT","Email:",e,sizeof e,false);if(!validate_email(e)){ui_message_box("Account","Please enter a valid email address.");return false;}if(!user_create(n,p,e,u)){ui_message_box("Account","Could not create account.");return false;}ui_message_box("Success","Account created. Welcome to the dungeon.");return true;}
static bool login_user(User *u){char n[NAME_LEN]={0},p[64]={0};ui_input("LOGIN","Username:",n,sizeof n,false);ui_input("LOGIN","Password:",p,sizeof p,true);if(!user_auth(n,p,u)){ui_message_box("Login failed","Wrong username or password.");return false;}return true;}
static void finish_run(Game *g){
    user_record_game(g);
    delete_save(g->username);
    char msg[160];
    if(g->won) snprintf(msg,sizeof msg,"Victory! Score %d, gold %d, kills %d.",g->player.score,g->player.gold,g->player.kills);
    else snprintf(msg,sizeof msg,"You died on floor %d. Score %d, gold %d.",g->floor,g->player.score,g->player.gold);
    ui_message_box(g->won?"THE CROWN IS YOURS":"RUN ENDED",msg);
}
static void run_game(Game*g){for(;;){ui_draw_game(g);if(g->game_over){finish_run(g);return;}int ch=getch();if(ch=='?'){ui_help();continue;}if(ch=='i'||ch=='I'){ui_inventory(g);continue;}if(ch=='s'||ch=='S'){save_game(g);add_message(g,"Game saved.");continue;}if(ch==27){int c=ui_pause_menu();if(c==0||c<0)continue;if(c==1){save_game(g);add_message(g,"Game saved.");continue;}if(c==2){ui_help();continue;}if(c==3){save_game(g);return;}}if(ch=='f'||ch=='F'){add_message(g,"Choose firing direction...");ui_draw_game(g);int d=getch();world_fire_direction(g,d);continue;}game_command(g,ch);}}
static void pregame(const char*username,Settings*s){for(;;){const char*it[]={"New adventure","Continue saved game","Scoreboard","Settings","Back"};int c=ui_menu("ADVENTURE HUB",username,it,5,0);if(c<0||c==4)return;if(c==0){Game g;game_new(&g,username,s);run_game(&g);}else if(c==1){Game g;if(load_game(username,&g))run_game(&g);else ui_message_box("Continue","No compatible save was found.");}else if(c==2)ui_scoreboard(username);else if(c==3)ui_settings(s);}}
int main(void){setlocale(LC_ALL,"");ensure_data_dir();ui_init();Settings settings=settings_load();User current={0};bool logged=false;for(;;){ui_title();napms(250);const char*it[]={"Login","Create account","Play as guest","Scoreboard","Settings","Quit"};int c=ui_menu("ROGUE DELUXE","Sharif FOP project - rebuilt edition",it,6,0);if(c<0||c==5)break;if(c==0){logged=login_user(&current);if(logged)pregame(current.username,&settings);}else if(c==1){logged=register_user(&current);if(logged)pregame(current.username,&settings);}else if(c==2)pregame("guest",&settings);else if(c==3)ui_scoreboard(logged?current.username:NULL);else if(c==4)ui_settings(&settings);}ui_shutdown();return 0;}
