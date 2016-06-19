#include <stdio.h>
#include <stdlib.h>
#include "sdldecl.h"
#include <SDL_mixer.h>
#include "main.h"


int main(int argc,char **argv)
{
   SDL_Surface *screen=SDL_SetVideoMode(SCREEN_WIDTH + SCORE_WIDTH,SCREEN_HEIGHT,32,SDL_SWSURFACE | SDL_DOUBLEBUF);
    SDL_Init(SDL_INIT_EVERYTHING);//everything including the audio
 SDL_WM_SetCaption("Doodle Jump",0);
 SDL_Surface *icon=IMG_Load("Images/doodleicon.bmp");

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,1024);
    //Mix_Volume(-1,0);//mute channels, -1 for all channels
    TTF_Init();
    SDL_EnableUNICODE(SDL_ENABLE);
    int menupressed=-1;//no menu pressed
    int selection=0;//initially 1st menu item selected

    SDL_WM_SetIcon(icon,0);
    Mix_Chunk *snd[] = {
                            Mix_LoadWAV("Audio/menu.wav"),
                            Mix_LoadWAV("Audio/jump.wav"),
                            Mix_LoadWAV("Audio/singlespring.wav"),
                            Mix_LoadWAV("Audio/jetpack.wav"),
                            Mix_LoadWAV("Audio/coptersound.wav"),
                            Mix_LoadWAV("Audio/springshoes.wav"),
                            Mix_LoadWAV("Audio/fallingplatform.wav")
                            };

    while(1)
    {
    if (menupressed ==-1) {
              if (menuloop(screen,&selection,&menupressed,snd) ==SDL_QUIT) break;//outer menu
    } else
    if (menupressed ==0)//play game pressed
    {
        if (gameloop(screen,snd) ==SDL_QUIT) break;
         else menupressed=-1;
    } else  if (menupressed ==1)//highscores pressed
    {
        if (highscoreloop(screen) ==SDL_QUIT) break;
        else menupressed=-1;
    } else  if (menupressed ==2)//about pressed
    {
        if (aboutloop(screen) ==SDL_QUIT) break;
        else menupressed=-1;
    } else  if (menupressed ==3)//exit pressed which is the last menu item
    {
         break;
    }
    }
    TTF_Quit();
    SDL_FreeSurface(icon);
    SDL_FreeSurface(screen);
    for (int i=0;i<6;i++) Mix_FreeChunk(snd[i]);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}
