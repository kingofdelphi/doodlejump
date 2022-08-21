#include "sdldecl.h"
#include "highscore.h" //only retrievelist function needed but whats other alternative
#include "audioengine.h"
#define POWERUP_RELEASE_VELOCITY 10
#define SCORELEVELY 30
#define SCORELEVELX (SCREEN_WIDTH - 100)
#define MAXHEIGHT 21
#define MAXHELIHEIGHT 50
#define MAXROCKETHEIGHT 80
#define MAXSINGLESPRINGHEIGHT 30
#define MAXSPRINGSHOEHEIGHT 35
#define SPRINGSHOEHITS 5
#define CAMERALEVEL 271
#define LEVELY 430
#define BRICKH 16
#define BRICKGAP 37 //i have made this equal to doodle height normal
#define BRICKW 57
#define MAXSPEED 18
#define SLIDESPEED 4
#define BRICKROWS 10
#define SCOREBACKGROUNDCOLOR 230,230,230
#define SCORE_FONT_SIZE 18
#define MAXFALLING 5
#define PLATFORMFALLSPEED 8
typedef struct {
    int falling;
    SDL_Rect rect;
} PLATFORMFALL;

typedef struct {
int visible;
enum platform_type {standable, nonstandable} platformstyle;
enum platform_color {green=0,blue=1,brown=2} platformcolor;
SDL_Rect platformrect;
enum power_up_type {none,rocket,copter,singlespring,singlespringpressed,springshoeunpressed} powerup;
SDL_Rect poweruprect;
enum sliding_type {nil,horizontal,vertical} sliding;
enum slide_direction {left,right,top,down} direction;

} PLATFORM;

int checkcollision(SDL_Rect *,SDL_Rect *);
void displayplatforms(SDL_Surface*, PLATFORM *,SDL_Surface **,SDL_Surface **,int*,int);
void placeplatformrandom(PLATFORM *,int*,int*,int*);
void updateslidingmovement(PLATFORM *);
void initallplatforms(PLATFORM *,SDL_Rect* ,int *,int*,int*);
void displayscore(SDL_Surface*,int,TTF_Font *,SDL_Rect*);
void cleanupresources(SDL_Surface **,int,SDL_Surface**,int,SDL_Surface **,int,TTF_Font*);
int writehighscore(int ,char*,int);
void displayfallingplatforms(SDL_Surface*,PLATFORMFALL *,SDL_Surface*,int);
