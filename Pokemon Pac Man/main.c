#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "game.h"
#include "gui.h"
#include "highscores.h"

int main(int argc, char* argv[]) 
{
    int total_score = 0;    
    Game game = {};
    HighScoreTable highscores;
    highscores.size=0;
    load_highscores(&highscores);

    /* Specifiy level number */
    int level = argc >= 2 ? atoi(argv[1]) : 1;

    /* Or use random seed */
    int seed = argc == 3 ? atoi(argv[2]) : (int)time(NULL);
    
    srand(seed);

    /* Initialize the gui */
    gui_initialize();

    while(game.state != GAME_ENDED) {
        
        /* Initialize the next game instance */
        init_game(&game, level);

        /* Start the actual game loop */
        total_score += do_game_loop(&game, total_score, &highscores);

        /* Clean up */
        destroy_game(&game);

        /* Press space to continue to next game*/
        if(game.state == GAME_OVER || game.state == FINISHED) {
            if(game.state==GAME_OVER)
            {
                level=0;
                check_highscore_entry(&highscores,total_score);
                total_score=0;
            }
            gui_all_keys_up();
            wait_for_space();
        }
        level+=1;
        
    }
    
    save_highscores(&highscores);
    
    gui_clean();
}
