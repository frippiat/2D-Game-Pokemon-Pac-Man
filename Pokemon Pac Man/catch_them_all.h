#ifndef H_CATCH_THEM_ALL
#define H_CATCH_THEM_ALL

#define MAX_LEVEL_WIDTH 90
#define MAX_LEVEL_HEIGHT 60

#define TILE_SIZE 64

#define PLAYER_MOVEMENT_INCREMENT 4
#define PIKACHU_MOVEMENT_INCREMENT 2
#define STRONG_PIKACHU_MOVEMENT_INCREMENT 4

#define POKEBALL_TICKS 60
#define CATCH_ATTEMPT_TICKS 30
#define POKEBALL_MAX_POWER 5

#define FREEZE_DURATION 100

#define PIKACHU_SCORE 60
#define STRONG_PIKACHU_SCORE 150

#define STRONG_PIKACHU_LIVES 3

typedef enum {POKEBALL, CATCH_ATTEMPT, BONUS, OBSTACLE, EMPTY_SPACE} ENTITY_TYPE;
typedef enum {EXTRA_POWER, EXTRA_POKEBALL, FREEZE_PIKACHUS} BONUS_TYPE;
typedef enum {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3} ORIENTATION;




typedef struct {                
    int width;                 
    int height;                
    int level_nr;              
    double fill_ratio;       
    int nr_of_pikachus;         
    int spawn_strong_pikachu;   
    double bonus_spawn_ratio;   
                    
} LevelInfo;


typedef struct {
    int x;                   
    int y;      

    ORIENTATION orientation; 
                              
    int pokeball_power;     
    int remaining_pokeballs;
    int is_catched;       
} Player;

typedef struct {
    ENTITY_TYPE type;
    int x;
    int y;
    int spread[4];
    int power;
    int ticks_left;
} CatchAttempt;

typedef struct {
    int x;
    int y;
    ORIENTATION orientation;
    int is_strong;
    int remaining_attempts;
    int is_catched;
    int frozen;
    CatchAttempt** attempts;
    int num_attempts;
} Pikachu;

typedef struct {
    ENTITY_TYPE type;
    int x;
    int y;
    int ticks_left;
} Pokeball;

typedef struct {
    ENTITY_TYPE type;
    int x;
    int y;
    BONUS_TYPE bonus_type;
} Bonus;

typedef struct {
    ENTITY_TYPE type;
    int x;
    int y;
    int is_catchable;
} Obstacle;

typedef struct {
    ENTITY_TYPE type;
    int x;
    int y;
} EmptySpace;

typedef union {
    ENTITY_TYPE type;
    Pokeball pokeball;
    CatchAttempt catch_attempt;
    Bonus bonus;
    Obstacle obstacle;
    EmptySpace empty_space;
} Entity;


#endif