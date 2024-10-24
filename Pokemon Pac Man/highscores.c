#include "highscores.h"


void load_highscores(HighScoreTable* highscores) {
    char* content = read_from_file(HIGHSCORE_FILE);
    const char* del=",";
    char* deel;
    deel = strtok(content,del);
    highscores->size=atoi(deel);
    if (highscores->size==0)
    {
        return;
    }
    highscores->entries=malloc((highscores->size)*sizeof(HighScore));
    int i=0;
    
    while (deel!=0)
    {

        deel = strtok(0,del);
        highscores->entries[i].player_name=malloc((strlen(deel)+1)*sizeof(char));
        strcpy(highscores->entries[i].player_name,deel);
        deel = strtok(0,del);
        highscores->entries[i].score=atoi(deel);
        deel = strtok(0,del);
        highscores->entries[i].date=malloc((strlen(deel)+1)*sizeof(char));
        strcpy(highscores->entries[i].date,deel);
        i+=1;
        if (i==highscores->size)
        {
            break;
        }
    }
    return;
    
}


char* read_from_file(const char* filename){
	FILE* ifp = fopen(filename, "r");
	fseek(ifp, 0L, SEEK_END);
	long size_tell = ftell(ifp);		
	rewind(ifp);				
	char* output = malloc((size_tell+1)*sizeof(char));
	long size_read = fread(output, sizeof(char), size_tell, ifp);
	fclose(ifp);
	if (size_tell != size_read) {
		output = realloc(output,sizeof(char)*(size_read + 1));
		output[size_read] = '\0';
	}
    output[size_read] = '\0';
	return output;
}	

void check_highscore_entry(HighScoreTable* highscores, int score) {

    //If the HighScoreTable is empty
     if ( highscores->size==0)
    {
        highscores->entries=calloc(sizeof(HighScore),MAX_HIGHSCORE_ENTRIES);
        highscores->entries[0].score=score;
        highscores->entries[0].player_name=calloc(sizeof(char),20);
        printf("Gelieve uw naam hier in te vullen, maximaal twintig karakters");
        int succes = scanf("%s",highscores->entries[0].player_name); 
        time_t tijd;
        time(&tijd);
        highscores->entries[0].date=malloc(sizeof(char)*strlen(ctime(&tijd))+1);
        strcpy(highscores->entries[0].date,ctime(&tijd));
        highscores->entries[0].date[strlen(highscores->entries[0].date)-1]='\0';
        highscores->size=1;
        highscores->changed=1;
    }

    //If highscore->size=5 and the current score is inferior to the other score, do not change the highscores-table
    else if(highscores->size==MAX_HIGHSCORE_ENTRIES && score<=highscores->entries[highscores->size-1].score)
    {
        highscores->changed=0;
    }

    /**
     * When highscores->size=5 and the current score is better than one or multiple highscores in the highscores-table,
     * or when highscore->size<5, update the highscores-table
     */
    else
    {
        int newind=highscores->size;

       //Define the ranking (of the current score)
        for(int i=0; i<highscores->size;i++)
        {
            if (score>highscores->entries[i].score)
            {
                newind=i;
                break;
            }
        }

        //If highscores->size<MAX_HIGHSCORE_ENTRIES, allocate additional memory for the new highscore
        if(highscores->size<MAX_HIGHSCORE_ENTRIES)
        {
            highscores->entries = realloc(highscores->entries,sizeof(HighScore)*((highscores->size)+1));
            highscores->entries[highscores->size].player_name=calloc(sizeof(char),20);
            time_t tijd;
            time(&tijd);
            highscores->entries[highscores->size].date=malloc(sizeof(char)*(strlen(ctime(&tijd))+1));
            highscores->size+=1;
        }
        for(int i=highscores->size-1;i>newind;i=i-1)
        {
            highscore_copy(&highscores->entries[i-1],&highscores->entries[i]);
        }
        //index van nieuwe score is bepaald, verschuif de scores kleiner dan de nieuwe score één plek naar achter
        highscores->entries[newind].score=score;
        highscores->entries[newind].player_name = realloc(highscores->entries[newind].player_name,20*sizeof(char));
        printf("Gelieve uw naam hier in te vullen, maximaal tien karakters");
        int succes = scanf("%s",highscores->entries[newind].player_name); 
        highscores->entries[newind].player_name =realloc(highscores->entries[newind].player_name,(strlen(highscores->entries[newind].player_name)+1)*sizeof(char));
        time_t tijd;
        time(&tijd);
        highscores->entries[newind].date =realloc(highscores->entries[newind].date,sizeof(char)*(strlen(ctime(&tijd))+1));
        strcpy(highscores->entries[newind].date,ctime(&tijd));
        highscores->entries[newind].date[strlen(highscores->entries[newind].date)-1]='\0';
        highscores->changed=1;
    }
    
   return;
    
    
}


//Description in highscores.h
void highscore_copy(HighScore* highscore1,HighScore* highscore2)
{
    highscore2->player_name = realloc(highscore2->player_name,sizeof(char)*(strlen(highscore1->player_name)+1));
    highscore2->date = realloc(highscore2->date,sizeof(char)*(strlen(highscore1->date)+1));
    highscore2->score=highscore1->score;
    strcpy(highscore2->player_name,highscore1->player_name);
    strcpy(highscore2->date,highscore1->date);
    return;
}

//Description in highscores.h
void write_to_file(const char* filename, const char* text) {
	FILE* ifp = fopen(filename, "w");
	fprintf(ifp, "%s", text);
	fclose(ifp);
}

//Description in highscores.h
void append_to_file_str(const char* filename, const char* text)
{
    FILE* ifp = fopen(filename, "a");
	fprintf(ifp, "%s", text);
	fclose(ifp);

}

//Description in highscores.h
void append_to_file_int(const char* filename, const int text)
{
    FILE* ifp = fopen(filename, "a");
	fprintf(ifp, "%i", text);
	fclose(ifp);

}	


void save_highscores(HighScoreTable* highscores) {
    write_to_file(HIGHSCORE_FILE,"");
    append_to_file_int(HIGHSCORE_FILE,highscores->size);
    append_to_file_str(HIGHSCORE_FILE,",");
    for(int i=0;i<highscores->size;i++)
    {
        append_to_file_str(HIGHSCORE_FILE,highscores->entries[i].player_name);
        append_to_file_str(HIGHSCORE_FILE,",");
        append_to_file_int(HIGHSCORE_FILE,highscores->entries[i].score);
        append_to_file_str(HIGHSCORE_FILE,",");
        append_to_file_str(HIGHSCORE_FILE,highscores->entries[i].date);
        append_to_file_str(HIGHSCORE_FILE,",");
    }
    for (int i = 0; i < highscores->size; i++)
    {
        free(highscores->entries[i].date); 
        free(highscores->entries[i].player_name);      
    }
    free(highscores->entries);
}

