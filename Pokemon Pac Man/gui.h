#ifndef H_GUI
#define H_GUI

#include <stdio.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "catch_them_all.h"
#include "highscores.h"
#include "util.h"

#define TICK_PER_S 40

#define FONT_H 42
#define FONT_2H (2*FONT_H)
#define FONT_3H (3*FONT_H)

#define TILE_W 64
#define TILE_H 64

#define SCALE 1
#define TILE_DIM 64

#define PLAYER_NORTH_X 0
#define PLAYER_SOUTH_X 1
#define PLAYER_EAST_X 2
#define PLAYER_WEST_X 3

#define POKEBALL_X 4
#define POKEBALL_Y 0

#define CATCH_CORE_X 4
#define CATCH_CORE_Y 1

#define PLAYER_CATCHED_X 4
#define PLAYER_CATCHED_Y 2

#define OBSTACLE_X 5
#define OBSTACLE_UNCATCHABLE_Y 0
#define OBSTACLE_CATCHABLE_Y 1

#define EMPTY_X 5
#define EMPTY_Y 2

#define PIKACHU_NORTH_X 6
#define PIKACHU_SOUTH_X 7
#define PIKACHU_EAST_X 8
#define PIKACHU_WEST_X 9

#define BONUS_X 10
#define BONUS_POKEBALL 0
#define BONUS_POKEBALL_POWER 1
#define BONUS_POKEBALL_FREEZE 2

#define CATCH_X 11
#define CATCH_CENTER_Y 0
#define CATCH_VERTICAL_Y 1
#define CATCH_HORIZONTAL_Y 2

#define GAME_OFFSET_X 10
#define GAME_OFFSET_Y FONT_3H
#define GAME_EXTRA_W 10
#define GAME_EXTRA_H 10

#define CATCH_ATTEMPT 1
#define CATCH_ATTEMPT_POKEBALL 2
#define CATCH_ATTEMPT_OBSTACLE 4
#define CATCH_ATTEMPT_PLAYER 8
#define CATCH_ATTEMPT_PIKACHU 16

#define CHECK_INIT { if(!ready) { fprintf(stderr, "Please call the initialize method first!\n"); exit(-1); } }

typedef enum {GUI_KEY_UP = 1, GUI_KEY_DOWN = 2, GUI_KEY_RIGHT = 4, GUI_KEY_LEFT = 8, GUI_KEY_SPACE = 16, GUI_KEY_ESC = 32} GUI_KEY;

void gui_initialize();
void gui_clean();
void gui_set_level_info(LevelInfo* level_info);
void gui_game_event(Event* ev);
void gui_add_bonus(Bonus* bonus);
void gui_add_pikachu(Pikachu* pikachu);
void gui_add_catch_attempt_tile(int x, int y);
void gui_add_player(Player* player);
void gui_add_obstacle(Obstacle* obstacle);
void gui_add_pokeball(Pokeball* pokeball);
void gui_draw_buffer();
void gui_start_timer();
int gui_get_timer_score();
void gui_set_game_over(HighScoreTable* highscores);
void gui_set_finished_level(int total_score);
void gui_set_pikachus_left(int pikachus);
void gui_set_pokeballs_left(int pokeballs);
void gui_set_level_score(int level_score);
void gui_all_keys_up();


#endif