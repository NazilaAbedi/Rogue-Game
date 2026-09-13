#ifndef ROGUE_DELUXE_H
#define ROGUE_DELUXE_H

#if defined(__has_include)
#  if __has_include(<ncurses.h>)
#    include <ncurses.h>
#  elif __has_include(<curses.h>)
#    include <curses.h>
#  else
#    error "ncurses/curses development headers are required"
#  endif
#else
#  include <ncurses.h>
#endif
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define MAP_W 76
#define MAP_H 24
#define MAX_ROOMS 9
#define MAX_MONSTERS 28
#define MAX_MESSAGES 6
#define MAX_USERS 128
#define NAME_LEN 24
#define EMAIL_LEN 64
#define SAVE_MAGIC 0x524F4755u
#define SAVE_VERSION 3u
#define FINAL_FLOOR 4
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

typedef struct { int x, y; } Point;
typedef struct { int x, y, w, h, theme; } Room;

typedef enum {
    T_VOID=' ', T_FLOOR='.', T_CORRIDOR='#', T_DOOR='+', T_WALL_H='-', T_WALL_V='|',
    T_STAIRS='>', T_TRAP='^', T_GOLD='$', T_FOOD='%', T_POTION='!', T_WEAPON=')',
    T_SCROLL='?', T_PILLAR='O', T_WINDOW='='
} Tile;

typedef enum { W_MACE, W_DAGGER, W_SWORD, W_BOW, W_WAND, W_COUNT } Weapon;
typedef enum { M_DEMON='D', M_FIRE='F', M_GIANT='G', M_SNAKE='S', M_UNDEAD='U' } MonsterKind;

typedef struct {
    int x, y, hp, max_hp, damage, vision, awake, alive;
    MonsterKind kind;
} Monster;

typedef struct {
    int x, y;
    int hp, max_hp;
    int hunger, gold, score, xp, kills;
    int food, potions, scrolls, arrows;
    Weapon weapon;
    unsigned owned_weapons;
    int shield_turns, haste_turns, power_turns;
} Player;

typedef struct {
    uint32_t magic, version;
    unsigned seed;
    int floor, turn, difficulty;
    int player_color;
    bool game_over, won;
    char username[NAME_LEN];
    Player player;
    char tiles[MAP_H][MAP_W];
    unsigned char seen[MAP_H][MAP_W];
    unsigned char visible[MAP_H][MAP_W];
    Room rooms[MAX_ROOMS];
    int room_count;
    Monster monsters[MAX_MONSTERS];
    int monster_count;
    char messages[MAX_MESSAGES][96];
    int message_count;
    time_t started_at;
} Game;

typedef struct {
    char username[NAME_LEN];
    char email[EMAIL_LEN];
    uint64_t password_hash;
    long total_score, total_gold;
    int games_finished, wins;
    long first_played;
} User;

typedef struct { int difficulty; int player_color; } Settings;

/* world.c */
void game_new(Game *g, const char *username, const Settings *settings);
void generate_floor(Game *g);
void update_visibility(Game *g);
void game_command(Game *g, int ch);
void game_tick_monsters(Game *g);
void world_fire_direction(Game *g, int ch);
void add_message(Game *g, const char *fmt, ...);
const char *weapon_name(Weapon w);
int weapon_damage(Weapon w);

/* persistence.c */
void ensure_data_dir(void);
uint64_t hash_password(const char *s);
bool validate_password(const char *s);
bool validate_email(const char *s);
bool user_load(const char *username, User *out);
bool user_create(const char *username, const char *password, const char *email, User *out);
bool user_auth(const char *username, const char *password, User *out);
void user_record_game(const Game *g);
int users_read(User *users, int cap);
bool save_game(const Game *g);
bool load_game(const char *username, Game *g);
void delete_save(const char *username);
Settings settings_load(void);
void settings_save(const Settings *s);

/* ui.c */
void ui_init(void);
void ui_shutdown(void);
void ui_title(void);
int ui_menu(const char *title, const char *subtitle, const char *items[], int count, int selected);
void ui_message_box(const char *title, const char *text);
bool ui_input(const char *title, const char *label, char *buf, int maxlen, bool secret);
void ui_draw_game(const Game *g);
void ui_help(void);
void ui_scoreboard(const char *current_user);
void ui_settings(Settings *settings);
void ui_inventory(const Game *g);
int ui_pause_menu(void);

#endif
