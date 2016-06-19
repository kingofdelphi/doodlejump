#include <stdio.h>
#include "string.h"
#include "sdldecl.h"

#define HIGHSCORE_BIG_FONT 30
#define HIGHSCORE_SMALL_FONT 18
#define MAXHIGHSCORE 8
#define MAXDISPLAYNAME 20
#define LEFTRIGHTGAP 30

#define SCOREGAP 20
typedef struct {
    char name[21];
    int score;
} SCOREDATA;


int retrievelist(SCOREDATA *,FILE *);
