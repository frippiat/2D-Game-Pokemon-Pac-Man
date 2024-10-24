#ifndef H_GAME
#define H_GAME

#include "catch_them_all.h"
#include "level.h"
#include "highscores.h"

typedef enum {LAUNCHED=0, FINISHED=1, GAME_OVER=2, GAME_ENDED=3} STATE;

typedef struct {
    int has_moved;
    int moves[4];
    int drop_pokeball;
} Input;

typedef struct {
    Level level;
    Player player;
    Pikachu* pikachus;
    int pikachus_left;
    STATE state;
    Input input;
    int score;
} Game;

void init_game(Game* game, int level_nr);
void destroy_game(Game* game);
int do_game_loop(Game* game, int start_score, HighScoreTable* highscores);
void initialise_catch_attempt(Game* game, int x_n, int y_n);
void process_pokeballs(Game* game);
void process_bonus_items(Game* game);

/**
 * @brief calculates if there is a potential collision on the given coordinates
 * 
 * @param game 
 * @param x_n 
 * @param y_n 
 * @return 1 if there is a potential collision, else return 0 
 */

int collision_detection(Game* game, int x_n, int y_n);
void do_player_movement(Game* game);
void do_pikachu_ai(Game* game);
void pikachu_movement(Game* game);
void update_game(Game* game);
void render_game(Game* game);
void check_game_input(Game* game);
void wait_for_space();


#endif