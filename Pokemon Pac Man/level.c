#include "level.h"

LevelInfo generate_level_info(int level_num) 
{
    LevelInfo level_info = {.width=25,.height=13,.level_nr=level_num,.fill_ratio=0.125,.nr_of_pikachus=level_num%level_info.width,.spawn_strong_pikachu=(level_num+1)%2,.bonus_spawn_ratio=0.5};

    return level_info;
}

void init_level(Level* level, LevelInfo level_info) 
{
    level->level_info=level_info;

    level->entities=(Entity**) malloc(level->level_info.height*sizeof(Entity*));
    gui_start_timer();

    for(int i=0;i<level->level_info.height;i++)
    {
        level->entities[i] = (Entity*)malloc(level->level_info.width*sizeof(Entity));
    }

    //Assign an entity to every tile on the map
    for(int i=0;i<level->level_info.height;i++)
    {
        for(int j=0;j<level->level_info.width-1;j++)
        {
            if((i==1 && j==1)||(i==1 && j==2)||(i==2 && j==1))
            {
                level->entities[i][j].empty_space.type=EMPTY_SPACE;
                level->entities[i][j].empty_space.x=j;
                level->entities[i][j].empty_space.y=i;

                continue;
            }
            else if(i==0 || i==level->level_info.height-1 || j==0 ||j==level->level_info.width-1||(j%2==0 && i%2==0))
            {
                level->entities[i][j].obstacle.type=OBSTACLE;
                level->entities[i][j].obstacle.is_catchable=false;
                level->entities[i][j].obstacle.x=j*TILE_SIZE;
                level->entities[i][j].obstacle.y=i*TILE_SIZE;
            }
            else if (rand()%((int)(1/level->level_info.fill_ratio))==1)
            {
                level->entities[i][j].obstacle.type=OBSTACLE;
                level->entities[i][j].obstacle.is_catchable=true;
                level->entities[i][j].obstacle.x=j*TILE_SIZE;
                level->entities[i][j].obstacle.y=i*TILE_SIZE;
            }
            else
            {
                level->entities[i][j].empty_space.type=EMPTY_SPACE;
                level->entities[i][j].empty_space.x=j*TILE_SIZE;
                level->entities[i][j].empty_space.y=i*TILE_SIZE;
            } 

        }
    }

}

void render_level(Level* level) 
{ 
    for(int i=0;i<level->level_info.height;i++) {
        for(int j=0;j<level->level_info.width-1;j++) {   
            if (level->entities[i][j].type==OBSTACLE) {
                gui_add_obstacle(&level->entities[i][j].obstacle);
            }
        }
    }
}

void destroy_level(Level* level) 
{
    for(int i=0;i<level->level_info.height;i++) 
    {
        free(level->entities[i]);
    }
    free(level->entities);
    return;
}
