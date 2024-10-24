#include "game.h"

/**
 * Whenever the player has no more pokeballs left, this variable is used to count 2*POKEBALL_TICKS ticks.
 * Once the counting is over, the player's game is over.
 * 
 */
int counter_last_ball=0;


void init_game(Game* game, int level_nr) 
{
    
    game->state=LAUNCHED;
    //Define and intialize the Level
    LevelInfo levelinf = generate_level_info(level_nr);
    init_level(&game->level,levelinf);              
    gui_set_level_info(&game->level.level_info);

    //Player starts on the coordinates (1,1)
    game->player.x=TILE_SIZE;        
    game->player.y=TILE_SIZE;           
    game->player.orientation=SOUTH; 
    game->player.is_catched=0;
    game->player.pokeball_power=1%(POKEBALL_MAX_POWER+1);
    game->player.remaining_pokeballs =2*game->level.level_info.nr_of_pikachus+2+STRONG_PIKACHU_LIVES*2*(game->level.level_info.level_nr%2==0); 
    game->pikachus_left=game->level.level_info.nr_of_pikachus;                 
    game->pikachus=malloc(game->level.level_info.nr_of_pikachus*sizeof(Pikachu));
    game->input.moves[0]=0;game->input.moves[1]=0;game->input.moves[2]=0;game->input.moves[3]=0;game->input.drop_pokeball=0;
    int xrand,yrand;

    //Each pikachu start on the map with random coordinates
    for(int i=0;i<game->level.level_info.nr_of_pikachus;i++)
    {
        xrand = rand()%game->level.level_info.width;
        yrand = rand()%game->level.level_info.height;
        while((game->level.entities[yrand][xrand].type!=EMPTY_SPACE) || (xrand<=2 && yrand<=2) || (xrand==0) || (yrand==0) || (xrand==game->level.level_info.width-1) || (yrand==game->level.level_info.height-1))
        {
            xrand = rand()%game->level.level_info.width;
            yrand = rand()%game->level.level_info.height;
        }
        game->pikachus[i].frozen=0;
        game->pikachus[i].x=TILE_SIZE*xrand;
        game->pikachus[i].y=TILE_SIZE*yrand;
        game->pikachus[i].orientation=SOUTH;
        game->pikachus[i].is_catched=0;
        game->pikachus[i].is_strong=0;

        //Number of attempts on strong_pikachu before it is caught
        game->pikachus[i].remaining_attempts=2;         
        game->pikachus[i].num_attempts=2;

    }
    game->pikachus[game->level.level_info.nr_of_pikachus-1].is_strong=game->level.level_info.spawn_strong_pikachu;
    counter_last_ball=0;
    render_game(game);
    return;
}


void destroy_game(Game* game) 
{
    if ((game->pikachus[game->level.level_info.nr_of_pikachus-1].is_strong) && (game->pikachus[game->level.level_info.nr_of_pikachus-1].remaining_attempts<game->pikachus[game->level.level_info.nr_of_pikachus-1].num_attempts))
    {
        free(game->pikachus[game->level.level_info.nr_of_pikachus-1].attempts);
        
    }
    
    free(game->pikachus);
}



Event ev;
int do_game_loop(Game* game, int start_score, HighScoreTable* highscores) 
{
    int stop = 0;
    game->score=0;
    while(!stop) {
        gui_game_event(&ev); 
        
        update_game(game);

        if (game->player.is_catched==1)
        {
            counter_last_ball=0;
            gui_set_game_over(highscores);
            game->state=GAME_OVER;
            game->player.is_catched=1;
            stop=1;
        }
        

        else if(game->pikachus_left<=0)
        {
            gui_set_finished_level((game->score-gui_get_timer_score())*(game->score-gui_get_timer_score()>=0));
            game->state=FINISHED;
            stop=1;
        }


        else if (game->state==GAME_ENDED)
        {
            stop=1; 
        }
        
        render_game(game);
        
        check_game_input(game);
        
    }    
    destroy_level(&game->level);
    return start_score+(game->score-gui_get_timer_score())*(game->score-gui_get_timer_score()>=0);
}

void initialise_catch_attempt(Game* game, int x_n, int y_n) 
{
    if(game->level.entities[x_n][y_n].obstacle.is_catchable==true && rand()%100<=game->level.level_info.bonus_spawn_ratio*100) {
        game->level.entities[x_n][y_n].bonus.type=BONUS;
        game->level.entities[x_n][y_n].bonus.x=y_n*TILE_SIZE;
        game->level.entities[x_n][y_n].bonus.y=x_n*TILE_SIZE;
        game->level.entities[x_n][y_n].bonus.bonus_type=rand()%3;
    } else {
        game->level.entities[x_n][y_n].catch_attempt.type=CATCH_ATTEMPT;
        game->level.entities[x_n][y_n].catch_attempt.x=y_n*TILE_SIZE;
        game->level.entities[x_n][y_n].catch_attempt.y=x_n*TILE_SIZE;
        game->level.entities[x_n][y_n].catch_attempt.power=game->player.pokeball_power;
        game->level.entities[x_n][y_n].catch_attempt.ticks_left=CATCH_ATTEMPT_TICKS;
        game->level.entities[x_n][y_n].catch_attempt.spread[0]=0;
        game->level.entities[x_n][y_n].catch_attempt.spread[1]=0;
        game->level.entities[x_n][y_n].catch_attempt.spread[2]=0;
        game->level.entities[x_n][y_n].catch_attempt.spread[3]=0;
    }
}

//Ensure that the player only drops 1 pokeball at the time
int space_pauze = 0;    




//Counts how much the pokeball power increases, depending on how many power bonusses the player has picked up
int extra_power=0;


void process_pokeballs(Game* game) 
{
    counter_last_ball+=1*(counter_last_ball!=0);
    if(game->input.drop_pokeball && game->player.remaining_pokeballs > 0 && space_pauze >= 10) {
        int i = game->player.y/TILE_SIZE;
        int j = game->player.x/TILE_SIZE;
        game->level.entities[i][j].pokeball.type=POKEBALL;
        game->level.entities[i][j].pokeball.ticks_left=POKEBALL_TICKS;
        game->level.entities[i][j].pokeball.x=j*TILE_SIZE;
        game->level.entities[i][j].pokeball.y=i*TILE_SIZE;
        game->player.remaining_pokeballs--;
        counter_last_ball=(game->player.remaining_pokeballs==0);
        space_pauze = 0;
    }
    space_pauze++;
    for(int i=0;i<game->level.level_info.height;i++) {
        for(int j=0;j<game->level.level_info.width-1;j++) {
            if(game->level.entities[i][j].type==POKEBALL) 
            {                 
                game->level.entities[i][j].pokeball.ticks_left--;

                /** 
                 * Once the pokeball's number of remaining ticks reaches zero, the CATCH_ATTEMPTs are initialized 
                 * and spread as them as far as game->player.power allows to go. 
                 */
                if(game->level.entities[i][j].pokeball.ticks_left<=0) {         
                    initialise_catch_attempt(game, i, j);
                    int spr0=1;
                    while ((game->level.entities[i+spr0][j].obstacle.is_catchable==true || game->level.entities[i+spr0][j].type==EMPTY_SPACE || game->level.entities[i+spr0][j].type==CATCH_ATTEMPT)&& spr0<=game->level.entities[i][j].catch_attempt.power) {
                        initialise_catch_attempt(game, i+spr0, j);
                        game->level.entities[i+spr0][j].catch_attempt.spread[SOUTH]=spr0;
                        spr0++;
                    }
                    spr0=1;
                    while ((game->level.entities[i-spr0][j].obstacle.is_catchable==true || game->level.entities[i-spr0][j].type==EMPTY_SPACE || game->level.entities[i-spr0][j].type==CATCH_ATTEMPT)&& spr0<=game->level.entities[i][j].catch_attempt.power) {
                        initialise_catch_attempt(game, i-spr0, j);
                        game->level.entities[i-spr0][j].catch_attempt.spread[NORTH]=spr0;
                        spr0++;
                    }
                    spr0=1;
                    while ((game->level.entities[i][j+spr0].obstacle.is_catchable==true || game->level.entities[i][j+spr0].type==EMPTY_SPACE || game->level.entities[i][j+spr0].type==CATCH_ATTEMPT)&& spr0<=game->level.entities[i][j].catch_attempt.power) {
                        initialise_catch_attempt(game, i, j+spr0);
                        game->level.entities[i][j+spr0].catch_attempt.spread[EAST]=spr0;
                        spr0++;
                    }
                    spr0=1;
                    while ((game->level.entities[i][j-spr0].obstacle.is_catchable==true || game->level.entities[i][j-spr0].type==EMPTY_SPACE || game->level.entities[i][j-spr0].type==CATCH_ATTEMPT)&& spr0<=game->level.entities[i][j].catch_attempt.power) {
                        initialise_catch_attempt(game, i, j-spr0);
                        game->level.entities[i][j-spr0].catch_attempt.spread[WEST]=spr0;
                        spr0++;
                    }
                    spr0=1;
                }

            }
            else if(game->level.entities[i][j].type==CATCH_ATTEMPT) {         
                game->level.entities[i][j].catch_attempt.ticks_left--;
                
                //Iteration through the array of Pikachus to verify if they are on a CATCH_ATTEMPT
                for(int p=0;p<game->level.level_info.nr_of_pikachus; p++) {     //
                    if(game->pikachus[p].x/TILE_SIZE==j && game->pikachus[p].y/TILE_SIZE==i && !game->pikachus[p].is_catched) {
                        if(!game->pikachus[p].is_strong || game->pikachus[p].remaining_attempts<=0) {

                            game->player.pokeball_power-=extra_power;
                            extra_power=0;
                            game->pikachus[p].is_catched = 1;
                            game->pikachus_left--;
                            game->score += PIKACHU_SCORE*(!game->pikachus[p].is_strong)+STRONG_PIKACHU_SCORE*(game->pikachus[p].is_strong);
                            if(rand()%100<=game->level.level_info.bonus_spawn_ratio*100) {
                                game->level.entities[i][j].bonus.type=BONUS;
                                game->level.entities[i][j].bonus.x=j*TILE_SIZE;
                                game->level.entities[i][j].bonus.y=i*TILE_SIZE;
                                game->level.entities[i][j].bonus.bonus_type=rand()%3;                                
                            }
                        }
                        else if (game->pikachus[p].is_strong)
                        {
                            int change=0;
                            int centerAttemptX=(j-game->level.entities[i][j].catch_attempt.spread[EAST]+game->level.entities[i][j].catch_attempt.spread[WEST]);
                            int centerAttemptY=(i-game->level.entities[i][j].catch_attempt.spread[SOUTH]+game->level.entities[i][j].catch_attempt.spread[NORTH]);
                            for (int att = 0; att < game->pikachus[p].num_attempts-game->pikachus[p].remaining_attempts; att++)
                            {
                                if (game->pikachus[p].attempts[att]!=NULL)
                                {
                                    change+=(((game->pikachus[p].attempts[att]->x==centerAttemptX*TILE_SIZE) && (game->pikachus[p].attempts[att]->y==centerAttemptY*TILE_SIZE)) && game->level.entities[centerAttemptY][centerAttemptX].type==CATCH_ATTEMPT);
                                }
                                
                            }
                            if (change==0)
                            {
                                game->pikachus[p].remaining_attempts--;
                                if (game->pikachus[p].remaining_attempts==game->pikachus[p].num_attempts-1)
                                {
                                    game->pikachus[p].attempts=malloc(sizeof(CatchAttempt*));

                                }
                                else
                                {
                                    game->pikachus[p].attempts=realloc(game->pikachus[p].attempts,sizeof(CatchAttempt*)*(game->pikachus[p].num_attempts-game->pikachus[p].remaining_attempts));  
                                }
                                game->pikachus[p].attempts[game->pikachus[p].num_attempts-game->pikachus[p].remaining_attempts-1]=&game->level.entities[centerAttemptY][centerAttemptX].catch_attempt;
                            }
                            

                        }
                        
                    }
                }

                //If the player moves into his own CATCH_ATTEMPS, he is caught
                if((game->player.x/TILE_SIZE==j) && (game->player.y/TILE_SIZE==i) && (!game->player.is_catched)) {
                    game->player.is_catched = 1;
                }

                //Once there a no more ticks left, the tile on the position of the CATCH_ATTEMPT becomes an EMPTY_SPACE 
                if(game->level.entities[i][j].catch_attempt.ticks_left<=0)      
                {
                    game->level.entities[i][j].empty_space.type=EMPTY_SPACE;
                    game->level.entities[i][j].empty_space.x=j;
                    game->level.entities[i][j].empty_space.y=i;

                    //When a catch attempt becomes a EMPTY_TILE, the pointer 
                    if (game->pikachus[game->level.level_info.nr_of_pikachus-1].is_strong)
                    {
                        for (int att = 0; att < game->pikachus[game->level.level_info.nr_of_pikachus-1].num_attempts-game->pikachus[game->level.level_info.nr_of_pikachus-1].remaining_attempts; att++)
                        {
                            if (game->pikachus[game->level.level_info.nr_of_pikachus-1].attempts[att]!=NULL)
                            {
                                if (game->pikachus[game->level.level_info.nr_of_pikachus-1].attempts[att]->type==EMPTY_SPACE)
                                {
                                    game->pikachus[game->level.level_info.nr_of_pikachus-1].attempts[att]=NULL;
                                }
                            }
                                
                        }
                    }
                    
                }
            }
        }
    }

}


int bonus_freeze_duration=0;


void process_bonus_items(Game* game) 
{

    //Process a bonus if the player moves into this bonus
    if(game->level.entities[game->player.y/TILE_SIZE][game->player.x/TILE_SIZE].type==BONUS) {
        int i = game->player.y/TILE_SIZE;
        int j=game->player.x/TILE_SIZE;
        if(game->level.entities[i][j].bonus.bonus_type==EXTRA_POWER && game->player.pokeball_power<=POKEBALL_MAX_POWER-1) {
            game->player.pokeball_power++;
            extra_power+=1;
        } 
        else if(game->level.entities[i][j].bonus.bonus_type==EXTRA_POKEBALL) {
            game->player.remaining_pokeballs++;
        } 
        else if(game->level.entities[i][j].bonus.bonus_type==FREEZE_PIKACHUS) {
            bonus_freeze_duration=FREEZE_DURATION;
            for(int p=0;p<game->level.level_info.nr_of_pikachus;p++) {
                game->pikachus[p].frozen=1;
            }
        }

        //Once the player has taken the bonus, the tile (on the position of the BONUS) becomes an EMPTY_SPACE
        game->level.entities[i][j].empty_space.type=EMPTY_SPACE;            
        game->level.entities[i][j].empty_space.x=j;
        game->level.entities[i][j].empty_space.y=i;                

    }
    if(bonus_freeze_duration>0) {
        bonus_freeze_duration--;
    }    
}


//Description in game.h
int collision_detection(Game* game, int x_n, int y_n) 
{    
    int collision = 0;

    //Collision with pikachus
    for(int i=0;i<game->level.level_info.nr_of_pikachus;i++) {      
        if(game->pikachus[i].x==x_n && game->pikachus[i].y==y_n && !game->pikachus[i].is_catched) {
            collision = 1;
        }
    }

    //Collision with obstacles
    if(game->level.entities[y_n/TILE_SIZE][x_n/TILE_SIZE].type==OBSTACLE) {  
        collision = 1;

    } 

    //Collision with the right wall of the game
    else if(x_n >= game->level.level_info.width*TILE_SIZE-TILE_SIZE) 
    {
        collision = 1;
    }
    return collision;
}



void do_player_movement(Game* game) 
{
    //Move NORTH (up) is there is no potential collision in that direction
    if(game->input.moves[0])
    {   
        game->player.orientation=NORTH;   
        if((collision_detection(game, game->player.x, game->player.y-PLAYER_MOVEMENT_INCREMENT)==0) && (game->player.x%TILE_SIZE==0))
        {
            game->player.y -= PLAYER_MOVEMENT_INCREMENT;
        }
    }
    
    //Move SOUTH (down) is there is no potential collision in that direction
    if(game->input.moves[1]) 
    {   
        game->player.orientation=SOUTH;
        if((collision_detection(game, game->player.x,game->player.y+TILE_SIZE)==0) && (game->player.x%TILE_SIZE==0))
        {
            game->player.y += PLAYER_MOVEMENT_INCREMENT;
        }
    }

    //Move EAST (right) is there is no potential collision in that direction
    if(game->input.moves[2]) {      
        game->player.orientation=EAST;
        if((collision_detection(game,game->player.x+TILE_SIZE, game->player.y)==0) && (game->player.y%TILE_SIZE==0)) 
        {
            game->player.x += PLAYER_MOVEMENT_INCREMENT;
        }
    }
    
    //Move WEST (left) is there is no potential collision in that direction
    if(game->input.moves[3]) 
    {      
        game->player.orientation=WEST;
        if((collision_detection(game,game->player.x-PLAYER_MOVEMENT_INCREMENT, game->player.y)==0) && (game->player.y%TILE_SIZE==0)) 
        {
            game->player.x -= PLAYER_MOVEMENT_INCREMENT;
        }
    }

    /**
     * Iteration through the array of pikachus to verify if the player is on the same position as one of them
     * If this is the case, the player is catched
     */
    for(int i=0;i<game->level.level_info.nr_of_pikachus;i++) { 
        if((abs(game->pikachus[i].x - game->player.x)<=(TILE_SIZE/2)) && (abs(game->pikachus[i].y - game->player.y)<=(TILE_SIZE/2)) && (!game->pikachus[i].is_catched)) {
            game->player.is_catched=1;
        }
    }
    
    if (counter_last_ball>2*POKEBALL_TICKS)
    {
        game->player.is_catched=1;
    }
    
}



void do_pikachu_ai(Game* game) 
{

    int randvalue;
    for(int j=0;j<game->level.level_info.nr_of_pikachus;j++)
    {
        //If a pikachu is frozen it cannot move
        if(game->pikachus[j].frozen==1 && bonus_freeze_duration>0) {        
            continue;

        } 
        else if(game->pikachus[j].frozen==1 && bonus_freeze_duration<=0) {
            game->pikachus[j].frozen=0;
        }
        
        //The pikachus can only change their orientation when they are on a new tile
        if(game->pikachus[j].orientation==EAST || game->pikachus[j].orientation==WEST)
        {
            if((game->pikachus[j].x)%TILE_SIZE==0)
            {
                randvalue=rand()%4;
                game->pikachus[j].orientation=randvalue; 
                int cond1=((game->pikachus[j].orientation==SOUTH) && (collision_detection(game,game->pikachus[j].x,game->pikachus[j].y+TILE_SIZE)));
                int cond2 = ((game->pikachus[j].orientation==NORTH) && (collision_detection(game,game->pikachus[j].x,game->pikachus[j].y-TILE_SIZE)));
                int cond3 = ((game->pikachus[j].orientation==WEST) && (collision_detection(game,game->pikachus[j].x-TILE_SIZE,game->pikachus[j].y)));
                int cond4 = ((game->pikachus[j].orientation==EAST) && (collision_detection(game,game->pikachus[j].x+TILE_SIZE,game->pikachus[j].y)));
                if(cond1 || cond2 || cond3 || cond4)
                {
                    continue;
                }

            }
        }

        //The pikachus can only change their orientation when they are on a new tile
        else if (game->pikachus[j].orientation==SOUTH || game->pikachus[j].orientation==NORTH)
        {   
            if((game->pikachus[j].y)%TILE_SIZE==0)
            {
                randvalue=rand()%4;
                game->pikachus[j].orientation=randvalue; //NORTH OR SOUTH
                int cond1=((game->pikachus[j].orientation==SOUTH) && (collision_detection(game,game->pikachus[j].x,game->pikachus[j].y+TILE_SIZE)));
                int cond2 = ((game->pikachus[j].orientation==NORTH) && (collision_detection(game,game->pikachus[j].x,game->pikachus[j].y-TILE_SIZE)));
                int cond3 = ((game->pikachus[j].orientation==WEST) && (collision_detection(game,game->pikachus[j].x-TILE_SIZE,game->pikachus[j].y)));
                int cond4 = ((game->pikachus[j].orientation==EAST) && (collision_detection(game,game->pikachus[j].x+TILE_SIZE,game->pikachus[j].y)));
                if(cond1 || cond2 || cond3 || cond4)
                {
                    continue;
                }
                
            }
        }
        
        //Make each pikachu advance with PIKACHU_MOVEMENT_INCREMENT steps in the direction given by its orientation
        if (game->pikachus[j].orientation==SOUTH || game->pikachus[j].orientation==NORTH)
        {
            game->pikachus[j].y+=PIKACHU_MOVEMENT_INCREMENT*(game->pikachus[j].orientation-1)*(!game->pikachus[j].is_strong)+STRONG_PIKACHU_MOVEMENT_INCREMENT*(game->pikachus[j].orientation-1)*(game->pikachus[j].is_strong);
        }
        else if (game->pikachus[j].orientation==EAST || game->pikachus[j].orientation==WEST)
        {
            game->pikachus[j].x+=PIKACHU_MOVEMENT_INCREMENT*(-1)*(game->pikachus[j].orientation-2)*(!game->pikachus[j].is_strong)+STRONG_PIKACHU_MOVEMENT_INCREMENT*(-1)*(game->pikachus[j].orientation-2)*(game->pikachus[j].is_strong);
        }
    }
    return;
}


void update_game(Game* game) 
{
    /* Do player movement */
    do_player_movement(game);

    /* Pikachu's actions (move, collision with player) */
    do_pikachu_ai(game);

    /* Process bonus items */
    process_bonus_items(game);

    /* Process pokeballs */
    process_pokeballs(game);
    
}


void render_game(Game* game) 
{
    render_level(&game->level);
    gui_set_pikachus_left(game->pikachus_left);                
    gui_set_pokeballs_left(game->player.remaining_pokeballs);  
    gui_add_player(&game->player);

    //Only draw the pikachus that have not been caught yet
    for(int i=0;i<game->level.level_info.nr_of_pikachus; i++) {
        if(!game->pikachus[i].is_catched) {
            gui_add_pikachu(&game->pikachus[i]);
        }
    }

    //Render the game map 
    for(int i=0;i<game->level.level_info.height;i++) {
        for(int j=0;j<game->level.level_info.width-1;j++) {
            if(game->level.entities[i][j].type==POKEBALL) 
            {                 
                gui_add_pokeball(&game->level.entities[i][j].pokeball);
            } 
            else if(game->level.entities[i][j].type==CATCH_ATTEMPT) 
            {     
                gui_add_catch_attempt_tile(j*TILE_SIZE, i*TILE_SIZE);
            } 
            else if(game->level.entities[i][j].type==BONUS) 
            {            
                gui_add_bonus(&game->level.entities[i][j].bonus);
            }
        }
    }
    gui_set_level_score(game->score);
    gui_draw_buffer();
}


void check_game_input(Game* game) 
{
    int keyinput = ev.keyDownEvent.key;
    game->input.moves[0]=((GUI_KEY_UP&keyinput)!=0 && game->state==LAUNCHED);
    game->input.moves[1]=((GUI_KEY_DOWN&keyinput)!=0 && game->state==LAUNCHED);
    game->input.moves[2]=((GUI_KEY_RIGHT&keyinput)!=0 && game->state==LAUNCHED);
    game->input.moves[3]=((GUI_KEY_LEFT&keyinput)!=0 && game->state==LAUNCHED);
    game->input.drop_pokeball=((GUI_KEY_SPACE&keyinput)!=0 && game->state==LAUNCHED);

    //The game ends if the player presses the ESC-key
    if ((keyinput&GUI_KEY_ESC)!=0)
    {
        game->state=GAME_ENDED;
    }

    //The player cannot drop a pokeball within the first second of the game
    if (gui_get_timer_score()==0)
    {
        game->input.drop_pokeball=0;
    }
    
    return;
}


void wait_for_space() 
{
    while((GUI_KEY_SPACE&ev.keyDownEvent.key)==0)
    {
        gui_game_event(&ev);
    }

    return;
}

