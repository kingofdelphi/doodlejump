#include "sdldecl.h"
#include "about.h"

int aboutloop(SDL_Surface *screen, SDL_Window *window) {
    SDL_Event event;
    TTF_Font *bfont = TTF_OpenFont("Font/calibri.ttf",BIGFONT);//20 is the about font size
    TTF_Font *sfont = TTF_OpenFont("Font/calibri.ttf",SMALLFONT);//20 is the about font size
    SDL_Surface *caption =TTF_RenderText_Blended(bfont,"Doodle Jump",(SDL_Color){0,0,0});
    SDL_Surface *txt =TTF_RenderText_Blended(sfont,"Created by Uttamraj Khanal",(SDL_Color){0,0,0});

    while(1) {
        if (SDL_PollEvent(&event)) {
            if (event.type==SDL_QUIT) {
                SDL_FreeSurface(txt);
                SDL_FreeSurface(caption);
                TTF_CloseFont(bfont);
                TTF_CloseFont(sfont);
                return SDL_QUIT;
            } else if (event.type==SDL_KEYDOWN && (event.key.keysym.sym ==SDLK_ESCAPE || SDLK_RETURN)) {
                SDL_FreeSurface(txt);
                SDL_FreeSurface(caption);
                TTF_CloseFont(bfont);
                TTF_CloseFont(sfont);
                return 0;
            }
        }
        //rendering
        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,255,255,255));
        SDL_BlitSurface(caption,0,screen,&(SDL_Rect){(SCREEN_WIDTH-caption->clip_rect.w)/2,(SCREEN_HEIGHT-caption->clip_rect.h-txt->clip_rect.h-MSGGAP)/2});
        SDL_BlitSurface(txt,0,screen,&(SDL_Rect){(SCREEN_WIDTH-txt->clip_rect.w)/2,(SCREEN_HEIGHT-caption->clip_rect.h-txt->clip_rect.h-MSGGAP)/2 + caption->clip_rect.h + MSGGAP});
		SDL_UpdateWindowSurface(window);

    }
}
