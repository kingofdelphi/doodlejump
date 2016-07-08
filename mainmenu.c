#include "sdldecl.h"
#include "SDL_mixer.h"
#include "mainmenu.h"


void clearscreen(SDL_Surface *surface) {
    SDL_FillRect(surface,&surface->clip_rect,SDL_MapRGB(surface->format,BACK_COLOR));
}

void updatebuttons(SDL_Surface *surface,MENUITEM *menu,int cnt,TTF_Font *font) {
    int height=surface->clip_rect.h - DOWNGAP-cnt * (BUTTON_H + GAP);
    int x = surface->clip_rect.w -DOWNGAP- BUTTON_W-GAP;

    int i;
    for ( i=0;i<cnt;i++,height +=BUTTON_H+GAP) {
        SDL_FillRect(surface,&(SDL_Rect){x-1,height-1,BUTTON_W+2,BUTTON_H+2},SDL_MapRGB(surface->format,0,162,232));

        if (menu[i].selstate==0) {
            TTF_SetFontStyle(font,TTF_STYLE_NORMAL);
            SDL_FillRect(surface,&(SDL_Rect){x,height,BUTTON_W,BUTTON_H},SDL_MapRGB(surface->format,BUTTON_COLOR));

        } else {
            TTF_SetFontStyle(font,TTF_STYLE_BOLD);
            SDL_FillRect(surface,&(SDL_Rect){x,height,BUTTON_W,BUTTON_H},SDL_MapRGB(surface->format,BUTTON_SEL_COLOR));
        }
        SDL_Surface *txt=TTF_RenderText_Blended(font,menu[i].caption,(SDL_Color){0,0,0});

        SDL_BlitSurface(txt,0,surface,&(SDL_Rect){x+10,height+(BUTTON_H - txt->clip_rect.h) /2});
        SDL_FreeSurface(txt);
    }

}

int menuloop(SDL_Surface *screen,int *selection,int *menupressed,Mix_Chunk **snd) {
    SDL_Event event;
    MENUITEM menulst[] = {
        {"play",0},
        {"highscores",0},
        {"about",0},
        {"exit",0}
    };
    menulst[*selection].selstate = 1;
    TTF_Font *font= TTF_OpenFont("Font/calibri.ttf",BUTTON_FONT_SIZE);
    SDL_Surface *UFO = IMG_Load("Images/Ufo.gif");
    SDL_Surface *platformgreen = IMG_Load("Images/platform green.gif");
    SDL_Surface *doodle = IMG_Load("Images/Doodle Origright.png");
    SDL_Rect UFORect = {SCREEN_WIDTH+SCORE_WIDTH-UFO->clip_rect.w-10,20};
    SDL_Rect platformrect = {30,SCREEN_HEIGHT-80};
    SDL_Rect doodlerect ={30,SCREEN_HEIGHT-80-doodle->clip_rect.h};
    double gravity = 1.8;
    double v=MENUDOODLEHEIGHT;
    int h=0;
    while (*menupressed ==-1) {
        int tick=SDL_GetTicks();
        if (SDL_PollEvent(&event)) {
            if (event.type ==SDL_KEYDOWN) {
                if (event.key.keysym.sym ==SDLK_ESCAPE) {
                    TTF_CloseFont(font);
                    SDL_FreeSurface(UFO);
                    SDL_FreeSurface(platformgreen);
                    SDL_FreeSurface(doodle);
                    return SDL_QUIT;
                } else if (event.key.keysym.sym ==SDLK_UP) {
                    if (*selection !=0) //not greater than total menu item count
                    {
                        menulst[*selection].selstate=0;
                        (*selection)--;
                        menulst[*selection].selstate=1;//one for selection
                    } else {
                        menulst[0].selstate=0;
                        *selection =MENUITEMCOUNT-1;
                        menulst[*selection].selstate=1;//one for selection
                    }
                    playaudio(MENUSOUND,snd);
                } else     if (event.key.keysym.sym ==SDLK_DOWN) {
                    if (*selection !=MENUITEMCOUNT-1) {
                        menulst[*selection].selstate=0;
                        (*selection)++;
                        menulst[*selection].selstate=1;// one for selection
                    } else {
                        menulst[*selection].selstate=0;
                        (*selection) =0;
                        menulst[*selection].selstate=1;// one for selection
                    }
                    playaudio(MENUSOUND,snd);
                } else if (event.key.keysym.sym ==SDLK_RETURN)//need to cleanup
                {
                    *menupressed = *selection;
                    TTF_CloseFont(font);
                    SDL_FreeSurface(UFO);
                    SDL_FreeSurface(platformgreen);
                    SDL_FreeSurface(doodle);
                    return 0;
                }
            }  else if (event.type ==SDL_QUIT)//need to cleanup
            {
                TTF_CloseFont(font);
                SDL_FreeSurface(UFO);
                SDL_FreeSurface(platformgreen);
                SDL_FreeSurface(doodle);
                return SDL_QUIT;
            }
        }
        //handle gravitational movement of doodle

        v = v - gravity;//v = u - g * t, for each frame time = 1 sec assumed, each second velocity decreases by gravity
        h = (int)(v - gravity/2);//h = u * t - 1/2 * g * t^2, for each frame time = 1 sec assumed
        doodlerect.y -=h;
        if (doodlerect.y +doodlerect.h > platformrect.y) {
            doodlerect.y =  platformrect.y-doodlerect.h;
            v = MENUDOODLEHEIGHT;
        }
        //rendering portion
        clearscreen(screen);
        updatebuttons(screen,menulst,MENUITEMCOUNT,font);//
        //blit UFO sprite
        SDL_BlitSurface(UFO,0,screen,&UFORect);
        //blit platform
        SDL_BlitSurface(platformgreen,0,screen,&platformrect);
        //blit doodle
        SDL_BlitSurface(doodle,0,screen,&doodlerect);
        SDL_Flip(screen);
        //frme rate handling
        if (SDL_GetTicks() - tick < 1000/FRAME_RATE) SDL_Delay(1000/FRAME_RATE-SDL_GetTicks()+tick);
    }

    return 0;
}
