#include "gui.h"

int ready = 0;

ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_BITMAP* tile = NULL;
ALLEGRO_FONT* font = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_EVENT_QUEUE* queue =  NULL;

int fps = 0, fps_accum = 0;
double fps_time = 0.0;

int keys = 0;

int is_game_over = 0, is_finished = 0;

int pokeballs_left = 0, pikachus_left = 0;

int level_score = 0, total_score = 0;

int level_time = 0, tick_time = 0, walk_time = 0, step_time = 0;

int screen_w = 0, screen_h = 0;

LevelInfo* cur_level = NULL;

HighScoreTable* highscores=NULL;

int** catch_attempts = NULL;

void gui_initialize() {
    int i;

    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_ttf_addon();
    al_install_keyboard();

    font = al_load_ttf_font("fonts/digital.ttf", 50, 0);

    if(font == NULL) {
        fprintf(stderr, "Failed to load font!\n");
        exit(-1);
    }
 
   tile = al_load_bitmap("images/icons.png");
    if(tile == NULL) {
        fprintf(stderr, "Failed to load icons!\n");
        exit(-1);
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    display = al_create_display(640, 480);
    if(display == NULL) {
        fprintf(stderr, "Failed to create display!\n");
        exit(-1);        
    }
    al_set_window_title(display, "Project PGM");

    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
 
    timer = al_create_timer(1.0 / TICK_PER_S);
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    catch_attempts = (int**) calloc(MAX_LEVEL_WIDTH, sizeof(int*));
    for(i = 0; i < MAX_LEVEL_WIDTH; i++) {
        catch_attempts[i] = (int*)calloc(MAX_LEVEL_HEIGHT, sizeof(int));
    }

    ready = 1;
}

void gui_clean() {
    int i;

    CHECK_INIT;

    ready = 0;
    
    al_stop_timer(timer);
    al_unregister_event_source(queue, al_get_keyboard_event_source());
    al_unregister_event_source(queue, al_get_display_event_source(display));
    al_unregister_event_source(queue, al_get_timer_event_source(timer));
    al_destroy_event_queue(queue);
    queue = NULL;
    al_destroy_timer(timer);
    timer = NULL;
    
    al_destroy_display(display);
    display = NULL;

    al_destroy_bitmap(tile);
    tile = NULL;

    al_destroy_font(font);
    font = NULL;

    al_uninstall_keyboard();
    al_shutdown_ttf_addon();
    al_shutdown_primitives_addon();
    al_shutdown_image_addon();
    al_uninstall_system();

    for(i = 0; i < MAX_LEVEL_WIDTH; i++) {
        free(catch_attempts[i]);
    }
    free(catch_attempts);
}

int orientation_offset(ORIENTATION orientation) {
    switch (orientation) {
        case NORTH: return 0;
        case EAST: return 2;
        case SOUTH: return 1;
        case WEST: return 3;
    }
    fprintf(stderr, "Bad orientation %d!\n", orientation);
    exit(-1);
}

void draw_base_level() {
    /*zwarte donkere rechthoek tekenen met de aangegeven size*/
    al_draw_filled_rectangle(
        0, 0,
        screen_w + GAME_OFFSET_X + GAME_EXTRA_W, screen_h + GAME_OFFSET_Y + GAME_EXTRA_H,
        al_map_rgb(0, 0, 0));
        
    
    int x, y;
    for(x = 0; x < cur_level->width; x++) {
        for(y = 0; y < cur_level->height; y++) {
            int sx, sy;

            if((x == 0) || (x == cur_level->width-1) || (y == 0) || (y == cur_level->height-1)) {
                sx = OBSTACLE_X * TILE_W;
                sy = OBSTACLE_UNCATCHABLE_Y * TILE_H;
            } else {
                sx = EMPTY_X * TILE_W;
                sy = EMPTY_Y * TILE_H;
            }

            al_draw_tinted_scaled_rotated_bitmap_region(
                tile,
                sx, sy, TILE_W, TILE_H,
                al_map_rgba_f(1, 1, 1, 1),
                0, 0,
                (x * TILE_DIM) + GAME_OFFSET_X, (y * TILE_DIM) + GAME_OFFSET_Y,
                SCALE, SCALE,
                0, 0);                

            if(catch_attempts[x][y] & CATCH_ATTEMPT) {
                int neighbors_x = 0;
                int neighbors_y = 0;
                if(x > 0 && catch_attempts[x-1][y] & CATCH_ATTEMPT) { neighbors_x++; }
                if(x < cur_level->width-1 && catch_attempts[x+1][y] & CATCH_ATTEMPT) { neighbors_x++; }
                if(y > 0 && catch_attempts[x][y-1] & CATCH_ATTEMPT) { neighbors_y++; }
                if(y < cur_level->height-1 && catch_attempts[x][y+1] & CATCH_ATTEMPT) { neighbors_y++; }

                sx = CATCH_CORE_X * TILE_W;
                sy = CATCH_CORE_Y * TILE_H;
                if(catch_attempts[x][y] & CATCH_ATTEMPT_POKEBALL) {
                    sx = CATCH_X * TILE_W;
                    sy = CATCH_CENTER_Y * TILE_H;
                } else {
                    if(neighbors_x >= 1 && neighbors_y >= 1) {
                        sx = CATCH_X * TILE_W;
                        sy = CATCH_CENTER_Y * TILE_H;
                    } else if(neighbors_x >= 1) {
                        sx = CATCH_X * TILE_W;
                        sy = CATCH_HORIZONTAL_Y * TILE_H;
                    } else if(neighbors_y >= 1) {
                        sx = CATCH_X * TILE_W;
                        sy = CATCH_VERTICAL_Y * TILE_H;
                    }

                    if(catch_attempts[x][y] & CATCH_ATTEMPT_OBSTACLE) {
                        sx = CATCH_CORE_X * TILE_W;
                        sy = CATCH_CORE_Y * TILE_H;
                    }
                }

                al_draw_tinted_scaled_rotated_bitmap_region(
                    tile,
                    sx, sy, TILE_W, TILE_H,
                    al_map_rgba_f(1, 1, 1, 1),
                    0, 0,
                    (x * TILE_DIM) + GAME_OFFSET_X, (y * TILE_DIM) + GAME_OFFSET_Y,
                    SCALE, SCALE,
                    0, 0);    

                if(catch_attempts[x][y] & CATCH_ATTEMPT_PLAYER || catch_attempts[x][y] & CATCH_ATTEMPT_PIKACHU) {
                    sx = CATCH_CORE_X * TILE_W;
                    sy = CATCH_CORE_Y * TILE_H;

                    al_draw_tinted_scaled_rotated_bitmap_region(
                        tile,
                        sx, sy, TILE_W, TILE_H,
                        al_map_rgba_f(1, 1, 1, 1),
                        0, 0,
                        (x * TILE_DIM) + GAME_OFFSET_X, (y * TILE_DIM) + GAME_OFFSET_Y,
                        SCALE, SCALE,
                        0, 0);    
                }
            }
        }
    }
        
    for(x = 0; x < cur_level->width; x++) {
        for(y = 0; y < cur_level->height; y++) {
            if(catch_attempts[x][y] & CATCH_ATTEMPT) {
                catch_attempts[x][y] &= ~CATCH_ATTEMPT;
            }
            else {
                catch_attempts[x][y] = 0;
            }
        }
    }
    
    
}

void gui_set_level_info(LevelInfo* level_info) {
    CHECK_INIT;

    if(level_info->width <= MAX_LEVEL_WIDTH && level_info->height <= MAX_LEVEL_HEIGHT) {
        screen_w = level_info->width * TILE_DIM;
        screen_h = level_info->height * TILE_DIM;

        al_resize_display(display, screen_w + GAME_OFFSET_X + GAME_EXTRA_W, screen_h + GAME_OFFSET_Y + GAME_EXTRA_H);

        cur_level = level_info;

        is_finished = 0;
        is_game_over = 0;

        draw_base_level();
    } else {
        fprintf(stderr, "Wrong level size %dx%d!\n", level_info->width, level_info->height);
        exit(-1);
    }
}

void gui_game_event(Event* ev) {
    int has_event = 0;

    while (!has_event) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
 
        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE: {
                keys |= GUI_KEY_ESC;
                ev->keyDownEvent.type = EVENT_KEY;
                ev->keyDownEvent.key = keys;
                has_event = 1;
                break;
            }
            /*  Support following key combinations:
                    LEFT, UP, RIGHT, DOWN
                    A, W, D, S (qwerty)
                    Q, Z, D, S (azerty)
                    4, 8, 6, 5
            */
            case ALLEGRO_EVENT_KEY_UP: {
                 switch(event.keyboard.keycode) {
                    case ALLEGRO_KEY_PAD_8:
                    case ALLEGRO_KEY_W:
                    case ALLEGRO_KEY_Z:
                    case ALLEGRO_KEY_UP:
                       keys &= ~GUI_KEY_UP;
                       break;
 
                    case ALLEGRO_KEY_PAD_5:
                    case ALLEGRO_KEY_S:
                    case ALLEGRO_KEY_DOWN:
                       keys &= ~GUI_KEY_DOWN;
                       break;
 
                    case ALLEGRO_KEY_PAD_4:
                    case ALLEGRO_KEY_A:
                    case ALLEGRO_KEY_Q:
                    case ALLEGRO_KEY_LEFT: 
                       keys &= ~GUI_KEY_LEFT;
                       break;
 
                    case ALLEGRO_KEY_PAD_6:
                    case ALLEGRO_KEY_D:
                    case ALLEGRO_KEY_RIGHT:
                       keys &= ~GUI_KEY_RIGHT;
                       break;
 
                    case ALLEGRO_KEY_ENTER:
                    case ALLEGRO_KEY_SPACE:
                        keys &= ~GUI_KEY_SPACE;
                       break;
                 }
                 ev->keyDownEvent.type = EVENT_KEY;
                 ev->keyDownEvent.key = keys;
                 has_event = 1;
                 break;
            }
            case ALLEGRO_EVENT_KEY_DOWN: {
                 switch(event.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                        keys |= GUI_KEY_ESC;
                        break;

                    case ALLEGRO_KEY_PAD_8:
                    case ALLEGRO_KEY_W:
                    case ALLEGRO_KEY_Z:
                    case ALLEGRO_KEY_UP:
                       keys |= GUI_KEY_UP;
                       break;
 
                    case ALLEGRO_KEY_PAD_5:
                    case ALLEGRO_KEY_S:
                    case ALLEGRO_KEY_DOWN:
                       keys |= GUI_KEY_DOWN;
                       break;
 
                    case ALLEGRO_KEY_PAD_4:
                    case ALLEGRO_KEY_A:
                    case ALLEGRO_KEY_Q:
                    case ALLEGRO_KEY_LEFT: 
                       keys |= GUI_KEY_LEFT;
                       break;
 
                    case ALLEGRO_KEY_PAD_6:
                    case ALLEGRO_KEY_D:
                    case ALLEGRO_KEY_RIGHT:
                       keys |= GUI_KEY_RIGHT;
                       break;
 
                    case ALLEGRO_KEY_ENTER:
                    case ALLEGRO_KEY_SPACE:
                        keys |= GUI_KEY_SPACE;
                       break;
                 }
                 ev->keyDownEvent.type = EVENT_KEY;
                 ev->keyDownEvent.key = keys;
                 has_event = 1;
                 break;
            }
            case ALLEGRO_EVENT_TIMER: {
                if(++tick_time >= TICK_PER_S) {
                    level_time++;
                    tick_time = 0;
                }

                if(++step_time >= 5) {
                    step_time = 0;

                    if(++walk_time >= 3) {
                        walk_time = 1;
                    }
                }
                ev->timerEvent.type = EVENT_TIMER;
                has_event = 1;
                break;
            }
            case ALLEGRO_EVENT_DISPLAY_RESIZE: {
                al_acknowledge_resize(display);
                has_event = 0;
                break;
            }
            default: {
                has_event = 0;
                break;
            }
        }
    }
}

void gui_add_bonus(Bonus* bonus) 
{
    CHECK_INIT;

    if(bonus->x < MAX_LEVEL_WIDTH * TILE_DIM && bonus->y < MAX_LEVEL_HEIGHT * TILE_DIM) {
        int sx = BONUS_X * TILE_W;
        int sy = 0;

        if(bonus->bonus_type == EXTRA_POWER) {
            sy = BONUS_POKEBALL_POWER * TILE_H;
        } else if(bonus->bonus_type == EXTRA_POKEBALL) {
            sy = BONUS_POKEBALL * TILE_H;
        } else if(bonus->bonus_type == FREEZE_PIKACHUS) {
            sy = BONUS_POKEBALL_FREEZE * TILE_H;
        }

        al_draw_tinted_scaled_rotated_bitmap_region(
            tile,
            sx, sy, TILE_W, TILE_H,
            al_map_rgba_f(1, 1, 1, 1),
            0, 0,
            bonus->x + GAME_OFFSET_X, bonus->y + GAME_OFFSET_Y,
            SCALE, SCALE,
            0, 0);
    }
}

void gui_add_pikachu(Pikachu* pikachu) {
    CHECK_INIT;

    if(pikachu->x < MAX_LEVEL_WIDTH * TILE_DIM && pikachu->y < MAX_LEVEL_HEIGHT * TILE_DIM) {
        int sx = (PIKACHU_NORTH_X + orientation_offset(pikachu->orientation)) * TILE_W;
        int sy = 0;

        if(!pikachu->frozen) { 
            if(pikachu->orientation == NORTH || pikachu->orientation == SOUTH) {
                sy = walk_time * TILE_H;
            } else {
                if(walk_time <= 1) {
                    sy = walk_time * TILE_H;
                }
            }
        }

        ALLEGRO_COLOR color = al_map_rgba_f(1, 1, 1, 1);
        if(pikachu->is_strong) {
            color = al_map_rgba_f(1, 0, 0, 1);
        }

        al_draw_tinted_scaled_rotated_bitmap_region(
            tile,
            sx, sy, TILE_W, TILE_H,
            color,
            0, 0,
            pikachu->x + GAME_OFFSET_X, pikachu->y + GAME_OFFSET_Y,
            SCALE, SCALE,
            0, 0);

        if(pikachu->is_catched) {
            catch_attempts[pikachu->x / TILE_DIM][pikachu->y / TILE_DIM] |= CATCH_ATTEMPT_PIKACHU;
        }
    }
}

void gui_add_catch_attempt_tile(int x, int y) {    
    int tx, ty;

    CHECK_INIT;
    
    if(x < MAX_LEVEL_WIDTH * TILE_DIM && y < MAX_LEVEL_HEIGHT * TILE_DIM) {
        if(x % TILE_DIM != 0 || y % TILE_DIM != 0) {
            fprintf(stderr, "Coordinate must be at tile edge: x=%d y=%d!\n", x, y);
            exit(-1);
        }
        tx = x / TILE_DIM;
        ty = y / TILE_DIM;

        catch_attempts[tx][ty] |= CATCH_ATTEMPT;        
    }
}

void gui_add_player(Player* player) {
    CHECK_INIT;

    if(player->x < MAX_LEVEL_WIDTH * TILE_DIM && player->y < MAX_LEVEL_HEIGHT * TILE_DIM) {
        int sx = (PLAYER_NORTH_X + orientation_offset(player->orientation)) * TILE_W;
        int sy = 0;
        if(keys != 0) {
            sy = walk_time * TILE_H;
        }

        if(is_game_over || player->is_catched) {
            sx = PLAYER_CATCHED_X * TILE_W;
            sy = PLAYER_CATCHED_Y * TILE_H;
        }

        al_draw_tinted_scaled_rotated_bitmap_region(
            tile,
            sx, sy, TILE_W, TILE_H,
            al_map_rgba_f(1, 1, 1, 1),
            0, 0,
            player->x + GAME_OFFSET_X, player->y + GAME_OFFSET_Y,
            SCALE, SCALE,
            0, 0);
        
        if(player->is_catched) {
            catch_attempts[player->x / TILE_DIM][player->y / TILE_DIM] |= CATCH_ATTEMPT_PLAYER;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////

void gui_add_obstacle(Obstacle* obstacle)
{
    CHECK_INIT;

    if(obstacle->x < MAX_LEVEL_WIDTH * TILE_DIM && obstacle->y < MAX_LEVEL_HEIGHT * TILE_DIM) {
        int sx = OBSTACLE_X * TILE_W;
        int sy = 0;

        if(obstacle->is_catchable == 0) {
            sy = OBSTACLE_UNCATCHABLE_Y * TILE_H;
        } 
        else if(obstacle->is_catchable == 1) 
        {
            sy = OBSTACLE_CATCHABLE_Y*TILE_H;
        }

        al_draw_tinted_scaled_rotated_bitmap_region(
            tile,
            sx, sy, TILE_W, TILE_H,
            al_map_rgba_f(1, 1, 1, 1),
            0, 0,
            obstacle->x + GAME_OFFSET_X, obstacle->y + GAME_OFFSET_Y,
            SCALE, SCALE,
            0, 0);
    }
}

void gui_add_pokeball(Pokeball* pokeball)
{
    CHECK_INIT;

    if(pokeball->x < MAX_LEVEL_WIDTH * TILE_DIM && pokeball->y < MAX_LEVEL_HEIGHT * TILE_DIM) {
        int sx = POKEBALL_X * TILE_W;
        int sy = POKEBALL_Y*TILE_H;

        al_draw_tinted_scaled_rotated_bitmap_region(
            tile,
            sx, sy, TILE_W, TILE_H,
            al_map_rgba_f(1, 1, 1, 1),
            0, 0,
            pokeball->x + GAME_OFFSET_X, pokeball->y + GAME_OFFSET_Y,
            SCALE, SCALE,
            0, 0);
    }
    
}

void draw_game_over_overlay() 
{
    total_score+=level_score;
    al_draw_filled_rounded_rectangle(
        GAME_OFFSET_X+TILE_SIZE/2, GAME_OFFSET_Y+TILE_SIZE/2,
        screen_w+GAME_OFFSET_X-TILE_SIZE/2,screen_h+GAME_OFFSET_Y-TILE_SIZE/2,
        90.5,90.5,
        al_map_rgba(0, 0, 0,200));
    al_draw_textf(font,al_map_rgba(255,255,255,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+2*TILE_SIZE,ALLEGRO_ALIGN_CENTRE,"GAME OVER!");
    al_draw_textf(font,al_map_rgba(255,255,255,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+4*TILE_SIZE,ALLEGRO_ALIGN_CENTRE,"PRESS SPACE TO RESTART");
    

    for (int i = 0; i < highscores->size; i++)
    {
        al_draw_textf(font,al_map_rgba(255,255,255,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+TILE_SIZE*(i+6),ALLEGRO_ALIGN_CENTRE,"%i. %s  %i  %s", i+1,highscores->entries[i].player_name, highscores->entries[i].score, highscores->entries[i].date );
    }
    
    return;

}

void draw_finished_overlay(int level_nr, int level_s, int total_s) 
{
    if (level_nr==1)
    {
        total_score=0;
    }
    
    total_score+=(level_s-gui_get_timer_score())*(level_s-gui_get_timer_score()>=0);
    al_draw_filled_rounded_rectangle(
        GAME_OFFSET_X+TILE_SIZE/2, GAME_OFFSET_Y+TILE_SIZE/2,
        screen_w+GAME_OFFSET_X-TILE_SIZE/2,screen_h+GAME_OFFSET_Y-TILE_SIZE/2,
        90.5,90.5,
        al_map_rgba(0, 0, 0,200));
    al_draw_textf(font,al_map_rgba(0,255,0,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+screen_h/5,ALLEGRO_ALIGN_CENTRE,"LEVEL %i COMPLETED" , level_nr);
    al_draw_textf(font,al_map_rgba(0,255,0,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+2*screen_h/5,ALLEGRO_ALIGN_CENTRE,"LEVEL SCORE: %i" , (level_s-gui_get_timer_score())*(level_s-gui_get_timer_score()>=0));
    al_draw_textf(font,al_map_rgba(0,255,0,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+3*screen_h/5,ALLEGRO_ALIGN_CENTRE,"TOTAL SCORE: %i", total_score);
    al_draw_textf(font,al_map_rgba(0,255,0,255),(2*GAME_OFFSET_X+screen_w)/2,GAME_OFFSET_Y+4*screen_h/5,ALLEGRO_ALIGN_CENTRE,"PRESS SPACE TO GO TO THE NEXT LEVEL");

}

void gui_draw_buffer() {
    double t = al_get_time();
    CHECK_INIT;
    al_draw_textf(font,al_map_rgba(255,255,255,255),GAME_OFFSET_X,0,ALLEGRO_ALIGN_LEFT,"*POKEBALLS: %i" , pokeballs_left);
    al_draw_textf(font,al_map_rgba(255,255,255,255),GAME_OFFSET_X,FONT_H,ALLEGRO_ALIGN_LEFT,"*PIKACHUS: %i" , pikachus_left);
    al_draw_textf(font,al_map_rgba(255,255,255,255),GAME_OFFSET_X,FONT_2H,ALLEGRO_ALIGN_LEFT,"*POINTS: %i", level_score);
    al_draw_textf(font,al_map_rgba(255,255,255,255),screen_w+GAME_OFFSET_X,0,ALLEGRO_ALIGN_RIGHT,"LEVEL: %i" , cur_level->level_nr);
    al_draw_textf(font,al_map_rgba(255,255,255,255),screen_w+GAME_OFFSET_X,FONT_H,ALLEGRO_ALIGN_RIGHT,"ELAPSED SEC: %i" , gui_get_timer_score());
    al_draw_textf(font,al_map_rgba(255,255,255,255),screen_w+GAME_OFFSET_X,FONT_2H,ALLEGRO_ALIGN_RIGHT,"FPS: %i" , TICK_PER_S);
    if(is_game_over) {
        draw_game_over_overlay();
    }
    if(is_finished) {
        draw_finished_overlay(cur_level->level_nr, level_score, total_score);
    }
    
    al_flip_display();
    fps_accum++;
    if(t - fps_time >= 1) {
        fps = fps_accum;
        fps_accum = 0;
        fps_time = t;
    }

    draw_base_level();
    
}


////////////////////////////////////////////////////////////////////////////////////

void gui_start_timer() {
    CHECK_INIT;

    level_time = 0;
    tick_time = 0;
}

int gui_get_timer_score() {
    CHECK_INIT;

    return level_time;
}

void gui_set_game_over(HighScoreTable* highscores_) {
    CHECK_INIT;

    is_game_over = 1;
    highscores = highscores_;
}

void gui_set_finished_level(int total_score_) {
    CHECK_INIT;

    is_finished = 1;
    total_score = total_score_;
}



void gui_set_pikachus_left(int pikachus_left_) {
    CHECK_INIT;

    pikachus_left = pikachus_left_;
}

void gui_set_pokeballs_left(int pokeballs_left_) {
    CHECK_INIT;

    pokeballs_left = pokeballs_left_;
}

void gui_set_level_score(int level_score_) {
    CHECK_INIT;

    level_score = level_score_;
}

void gui_all_keys_up()
{
    al_flush_event_queue(queue);
    keys=0;
}

