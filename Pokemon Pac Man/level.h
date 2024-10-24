#ifndef H_LEVEL
#define H_LEVEL

#include "catch_them_all.h"
#include "gui.h"

//DATATYPES


typedef struct {
    LevelInfo level_info;  //geeft alle info over level, zie catch_them_all.h
    Entity** entities;     //2D array die alle info over de elementen in het spel bijhoudt, zie catch_them_all.h
/*BELANGRIJK: BIJKOMENDE INFO OVER ENTITY: er zal hier meer info over komen in catch_them_all.h,
    maar in het kort is dit een 2D array die bijhoudt voor elke tile wat erop is
    Vergeet niet dat een tile al een initiële breedte heeft, dus om info te hebben over een object 
    op positie (t_x,t_y): level.entities[t_x/TILE_SIZE][t_y/TILE_SIZE] */
} Level;


//FUNCTIES

/*AL GEIMPLEMENTEERD, geeft struct Level_info van het level */
LevelInfo generate_level_info(int level_nr);

/*Initialiseert een Level dat de LevelInfo level_info bevat. Volgens mij is het de bedoeling
dat dat entities array wordt gemaakt op basis van de meegegeven leven_info*/
void init_level(Level* level, LevelInfo level_info);

/*Tekent de informatie in de struct level (dus eigenlijk de entities array) met behulp van de GUI*/
void render_level(Level* level);

/*Level vernietigen en correct opruimen: hier wordt waarschijnlijk geheugenvrijgegeven*/
void destroy_level(Level* level);


#endif