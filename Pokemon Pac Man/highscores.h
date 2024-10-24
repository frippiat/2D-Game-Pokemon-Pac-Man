#ifndef H_HIGHSCORES
#define H_HIGHSCORES

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HIGHSCORE_FILE "highscores.dat"
#define MAX_HIGHSCORE_ENTRIES 5

typedef struct {
    char *player_name;
    int score;
    char* date;
} HighScore;

typedef struct {
    HighScore* entries;
    int size;
    int changed;
} HighScoreTable;

void load_highscores(HighScoreTable* highscores);

void check_highscore_entry(HighScoreTable* highscores, int score);

/**
 * @brief Deep copy of a Highscore-struct
 * 
 * @param highscore1 
 * @param highscore2 
 */
void highscore_copy(HighScore* source ,HighScore* destination);

void save_highscores(HighScoreTable* highscores);

/**
 * @brief return the text in the file (@param filename) as a string
 * 
 * @param filename 
 * @return char* 
 */
char* read_from_file(const char* filename);

/**
 * @brief Empty a file and write the given text
 * 
 * @param filename 
 * @param text 
 */
void write_to_file(const char* filename, const char* text);

/**
 * @brief Append the given string (@param text) to the text in the file
 * 
 * @param filename 
 * @param text 
 */
void append_to_file_str(const char* filename, const char* text);

/**
 * @brief Append the given integer (@param text) to the text in the file
 * 
 * @param filename 
 * @param text 
 */
void append_to_file_int(const char* filename, const int text);


#endif