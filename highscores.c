#include "highscore.h"

int retrievelist(SCOREDATA *scorelist,FILE *fp) {
    int i,c=0;
    while(1) {
        fread(&scorelist[c],sizeof(*scorelist),1,fp);
        if (!feof(fp)) {
            for (i=0;i<c;i++)//sort the scoredata as it is read
            {
                if (scorelist[c].score > scorelist[i].score) {
                    char tmpname[255];
                    strcpy(tmpname,scorelist[i].name);
                    int tmpno= scorelist[i].score;
                    scorelist[i].score = scorelist[c].score;
                    strcpy(scorelist[i].name,scorelist[c].name);
                    scorelist[c].score = tmpno;
                    strcpy(scorelist[c].name,tmpname);

                }
            }

            c++;



        } else break;
    }
    return c;//return how many highscore data are there
}


int highscoreloop(SDL_Surface *screen, SDL_Window * window) {
    SDL_Event event;
    FILE *fp=fopen("highscores.dat","a+b");
    rewind(fp);
    TTF_Font *fontbig = TTF_OpenFont("Font/calibri.ttf",HIGHSCORE_BIG_FONT);
    TTF_SetFontStyle(fontbig,TTF_STYLE_BOLD);
    TTF_Font *fontsmall = TTF_OpenFont("Font/calibri.ttf",HIGHSCORE_SMALL_FONT);
    SDL_Surface *caption =TTF_RenderText_Blended(fontbig,"HIGHSCORES",(SDL_Color){0,0,0});

    SCOREDATA scorelist[MAXHIGHSCORE]; // show MAXHIGHSCORE no of highscores
    int count=retrievelist(scorelist,fp);

    while(1) {
        if (SDL_PollEvent(&event)) {
            if (event.type ==SDL_KEYDOWN) {
                if (event.key.keysym.sym ==SDLK_ESCAPE ||\
                        event.key.keysym.sym ==SDLK_RETURN) {
                    fclose(fp);
                    TTF_CloseFont(fontbig);
                    TTF_CloseFont(fontsmall);
                    SDL_FreeSurface(caption);
                    return 0;
                }
            } else if (event.type ==SDL_QUIT) {
                fclose(fp);
                TTF_CloseFont(fontbig);
                TTF_CloseFont(fontsmall);
                SDL_FreeSurface(caption);
                return SDL_QUIT;

            }
        }
        //rendering
        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,255,255,255));//clear screen
        int hght=0;

        //height calculation block
        {
            SDL_Surface *sampletxt = TTF_RenderText_Blended(fontsmall,"  ",(SDL_Color){0,0,0});//for calculating height only
            SDL_FreeSurface(sampletxt);
            hght = MAXHIGHSCORE * (SCOREGAP + sampletxt->h);
        } //

        hght = (SCREEN_HEIGHT-caption->clip_rect.h  - hght) /2;
        SDL_BlitSurface(caption,0,screen,&(SDL_Rect){(SCREEN_WIDTH+SCORE_WIDTH-caption->clip_rect.w)/2,hght});
        hght +=caption->clip_rect.h + SCOREGAP;
        int i;
        for (i=0;i<count;i++) {
            char tmpstr[11];

            SDL_Surface *txt =TTF_RenderText_Blended(fontsmall,scorelist[i].name,(SDL_Color){0,0,0});
            SDL_BlitSurface(txt,0,screen,&(SDL_Rect){LEFTRIGHTGAP,hght});
            SDL_FreeSurface(txt);
            sprintf(tmpstr,"%d",scorelist[i].score);
            txt =TTF_RenderText_Blended(fontsmall,tmpstr,(SDL_Color){0,0,0});
            SDL_BlitSurface(txt,0,screen,&(SDL_Rect){SCREEN_WIDTH+SCORE_WIDTH-LEFTRIGHTGAP-txt->clip_rect.w,hght});
            hght +=txt->clip_rect.h + SCOREGAP;

            SDL_FreeSurface(txt);

        }
		SDL_UpdateWindowSurface(window);

    }
}

