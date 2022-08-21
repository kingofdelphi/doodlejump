#include "sdldecl.h"
#include "maingame.h"
#include "time.h"

int checkcollision(SDL_Rect *rc1,SDL_Rect *rc2) {
    if (rc1->x + rc1->w <=rc2->x) return 0;
    if (rc2->x + rc2->w <=rc1->x) return 0;
    if (rc1->y + rc1->h <=rc2->y) return 0;
    if (rc2->y  <=rc1->y) return 0;
    return 1;

}

void placeplatformrandom(PLATFORM *platform,int *poweruptime,int*slidertime,int*fallingplatformtime)//place platform in a row
{
    platform->visible =1;
    if (*fallingplatformtime > 20) {
        platform->platformstyle =  rand() % 2;
        *fallingplatformtime=0;//reset time
        if (platform->platformstyle==nonstandable) platform->platformcolor=brown;
    } else  platform->platformstyle =  standable;
    if (*slidertime > 10 && platform->platformstyle==standable)//time =10
    {
        platform->platformcolor =  rand() % 2;
        *slidertime=0;//reset time
    } else if (platform->platformstyle ==standable) platform->platformcolor =  0;
    platform->powerup = none;
    platform->platformrect.y =-BRICKH;
    platform->platformrect.w=BRICKW;
    platform->platformrect.h=BRICKH;
    platform->direction = rand() %2;
    platform->platformrect.x = rand() % (SCREEN_WIDTH-BRICKW);
    if (platform->platformcolor==green && *poweruptime >50)//for powerups only are placed in green platforms
    {
        *poweruptime=0;
        platform->powerup = rand() %6;//0 for none, 1 for rocket, 2 for copter
        if (platform->powerup== singlespringpressed) platform->powerup=singlespring;
        if (platform->powerup==rocket) {
            platform->poweruprect.w= 26;
            platform->poweruprect.h= 37;
        } else if (platform->powerup==copter) {
            platform->poweruprect.w= 28;
            platform->poweruprect.h= 23;
        } else if (platform->powerup==singlespring) {
            platform->poweruprect.w= 19;
            platform->poweruprect.h= 29;
        } else if (platform->powerup==springshoeunpressed) {
            platform->poweruprect.w= 28;
            platform->poweruprect.h= 21;
        }
        platform->poweruprect.x = platform->platformrect.x + rand() % (BRICKW-platform->poweruprect.w);
    }
}

int gameloop(SDL_Surface *screen, SDL_Window *window,Mix_Chunk** snd) {
    SDL_Event event;
    int jumping=0,falling=1;
    int left=0,right=0;
    enum ROCKETSTAT {starting,maximspeed} rocketstatus,copterstatus;
    enum doodle_type {normal,rocket} doodletype=normal;//for normal rocket
    int i;
    int poweruptime=0,fallingplatformtime=23,slidertime=13;//>20 check placeplatformrandom function
    int currentfloor=1;
    int height=MAXHEIGHT;
    srand(time(0));
    int cposy=0;
    double xspeed=0;
    int score=0;
    int gameover=0;
    int inputhighscore=0;
    int springshoehits=0;
    int gamepause=0;
    SDL_Rect scorerect[] = {{SCORELEVELX,SCORELEVELY},{SCREEN_WIDTH/2,SCREEN_HEIGHT/2}};
    SDL_Surface *doodlenormal[] = {
        IMG_Load("Images/Doodle Origleft.png"),
        IMG_Load("Images/Doodle Origright.png"),
        IMG_Load("Images/Doodle rocketleft.png"),
        IMG_Load("Images/Doodle rocketright.png"),
        IMG_Load("Images/Doodle helileft1.png"),
        IMG_Load("Images/Doodle helileft2.png"),
        IMG_Load("Images/Doodle heliright1.png"),
        IMG_Load("Images/Doodle heliright2.png"),
        IMG_Load("Images/Doodle springshoepressedleft.png"),
        IMG_Load("Images/Doodle springshoepressedright.png"),
        IMG_Load("Images/Doodle springshoeunpressedleft.png"),
        IMG_Load("Images/Doodle springshoeunpressedright.png")
    };
    SDL_Surface *platforms[] = {
        IMG_Load("Images/Platform green.gif"),
        IMG_Load("Images/Platform blue.gif"),
        IMG_Load("Images/platform unstandable.gif"),
        IMG_Load("Images/platformfalling1.png")

    };
    //         SDL_SetColorKey(platforms[3],SDL_SRCCOLORKEY,SDL_MapRGB(platforms[3]->format,255,255,255));

    SDL_Surface *powerups[] = {
        IMG_Load("Images/rocket.gif"),
        IMG_Load("Images/copter.gif"),
        IMG_Load("Images/single spring1.gif"),
        IMG_Load("Images/single spring2.gif"),
        IMG_Load("Images/springshoe.gif"),

    };
    PLATFORMFALL fallingrects[BRICKROWS];
    for (int j=0;j<BRICKROWS;j++) fallingrects[j].falling=0;
    int fallingcount=0;
    int curheliframe=0,springframe=0;
    TTF_Font *scorefont= TTF_OpenFont("Font/calibri.ttf",SCORE_FONT_SIZE);
    PLATFORM platformlist[BRICKROWS];
    int curdir=1;

    SDL_Rect doodle={330,430-40,doodlenormal[curdir]->clip_rect.w,doodlenormal[curdir]->clip_rect.h};
    initallplatforms(platformlist,&doodle,&poweruptime,&slidertime,&fallingplatformtime);
    cposy = doodle.y - doodle.h - 50;
    int doodlefalling=0;
    char highscorename[255]={0};
    int highscorelength;
    int pressed=0;
    while(1) {
        int tick=SDL_GetTicks();
        if (SDL_PollEvent(&event)) {
            if (event.type==SDL_QUIT) {
                cleanupresources(doodlenormal,12,platforms,4,powerups,5,scorefont);
                return SDL_QUIT;
            } else if (event.type==SDL_KEYDOWN && !inputhighscore) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (!gameover && !gamepause) {
                            left=1;
                            right=0;
                            curdir=0;
                        }
                        break;

                    case SDLK_RIGHT:
                        if (!gameover  && !gamepause) {
                            left=0;

                            right=1;

                            curdir=1;
                        }
                        break;
                    case SDLK_ESCAPE:
                    case SDLK_RETURN:
                        cleanupresources(doodlenormal,12,platforms,4,powerups,5,scorefont);
                        return 0;

                    case SDLK_SPACE:
                        if (!gameover) {
                            gamepause =(gamepause?0:1);
                            if (gamepause) Mix_Pause(-1); else Mix_Resume(-1);
                        }
                        break;
                    default:
                        break;
                }
            } else if (event.type==SDL_KEYDOWN && inputhighscore) {
                if (highscorelength <MAXDISPLAYNAME && (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z))
                {
                    highscorename[highscorelength++] = (char)(event.key.keysym.sym -  ('a' - 'A')* ((event.key.keysym.mod & KMOD_SHIFT) > 0));
                } else if (event.key.keysym.sym ==SDLK_BACKSPACE && highscorelength) {
                    highscorename[--highscorelength] = '\0';
                } else if (event.key.keysym.sym ==SDLK_ESCAPE || event.key.keysym.sym ==SDLK_RETURN) {
                    if (strlen(highscorename) ==0) writehighscore(score,"Anonymous",0);
                    else writehighscore(score,highscorename,0);
                    cleanupresources(doodlenormal,12,platforms,4,powerups,5,scorefont);
                    return 0;
                }
            }
            else if (event.type==SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:
                        left=0;

                        break;
                    case SDLK_RIGHT:
                        right=0;

                        break;

                    default :
                        break;

                }
            }

        }
        if (jumping && !gamepause) {
            doodle.y=doodle.y-height;
            if ( doodle.y < LEVELY-CAMERALEVEL) {//scroll platforms, doodle moves out of camera level
                doodle.y=LEVELY-CAMERALEVEL;//271 is the maximum jump height

                if (platformlist[currentfloor].platformrect.y <LEVELY || doodletype==normal || doodletype==rocket|| doodletype==copter || doodletype==singlespring || doodletype==springshoeunpressed)
                {
                    for (i=0;i<BRICKROWS;i++) {
                        if (fallingrects[i].falling) fallingrects[i].rect.y+=height;
                    }
                    for (i=BRICKROWS-1;i>=0;i--) {
                        platformlist[i].platformrect.y+=height;
                        score+=1;
                        if (platformlist[i].powerup !=none) platformlist[i].poweruprect.y +=height;
                        if (platformlist[i].platformrect.y >= SCREEN_HEIGHT)
                        {

                            placeplatformrandom(&platformlist[i],&poweruptime,&slidertime,&fallingplatformtime);
                            if (i ==BRICKROWS-1 )
                            {
                                platformlist[i].platformrect.y = platformlist[0].platformrect.y - BRICKH-BRICKGAP;//orig
                            }else  if (i ==0)
                            {
                                platformlist[i].platformrect.y = platformlist[1].platformrect.y - BRICKH-BRICKGAP;//orig
                            } else
                                platformlist[i].platformrect.y = platformlist[i+1].platformrect.y - BRICKGAP - BRICKH;//illusional cyclic scrolling and this one is better one
                            if (platformlist[i].powerup !=none) platformlist[i].poweruprect.y = platformlist[i].platformrect.y - platformlist[i].poweruprect.h;//illusional cyclic scrolling and this one is better one

                        }
                    }

                }
            }
            if (doodletype==normal || doodletype==singlespring ||doodletype==springshoeunpressed) height--;
            else if (doodletype==rocket && rocketstatus==starting) height++;//phase1 of rocket
            else if (doodletype==rocket && rocketstatus==maximspeed) height--;//phase2 of rocket
            else if (doodletype==copter && copterstatus==starting) height++;//phase1 of rocket
            else if (doodletype==copter && copterstatus==maximspeed) height--;//phase2 of rocket
            if (doodletype==rocket) {
                if (height >=MAXROCKETHEIGHT) {
                    rocketstatus=maximspeed;
                    height =MAXROCKETHEIGHT;
                }
            } else if (doodletype==copter) {
                if (height >=MAXHELIHEIGHT) {
                    copterstatus=maximspeed;
                    height =MAXHELIHEIGHT;
                }
            }
            if (height<=0) {
                jumping=0;
                falling=1;
                cposy = doodle.y;
                height=1;

                if (doodletype == rocket || doodletype == copter|| doodletype==singlespring) // release power ups
                {
                    doodletype=normal;

                }
            }

        }
        if (falling && doodlefalling !=1 && !gameover && !gamepause) {
            doodle.y+=height;

            height++;

            for (i=0;i<BRICKROWS;i++) {
                SDL_Rect collisionrect;

                if (doodletype!=springshoeunpressed) {
                    collisionrect.x = doodle.x;
                    collisionrect.y = doodle.y;
                    collisionrect.w = doodle.w;
                    collisionrect.h = doodlenormal[0]->clip_rect.h;
                } else if (doodletype ==springshoeunpressed) {

                    collisionrect.x = doodle.x;
                    collisionrect.y = doodle.y - 6;
                    collisionrect.w = doodle.w;
                    collisionrect.h = doodlenormal[8]->clip_rect.h;
                }
                if (platformlist[i].visible==1 && platformlist[i].platformcolor==brown && cposy+collisionrect.h <=platformlist[i].platformrect.y && \
                        checkcollision(&collisionrect,&(SDL_Rect){platformlist[i].platformrect.x,platformlist[i].platformrect.y,BRICKW,1}))
                {
                    platformlist[i].visible=0;
                    fallingrects[i].falling=1;
                    fallingrects[i].rect = platformlist[i].platformrect;
                    playaudio(FALLINGPLATFORMSOUND,snd);
                } else
                    if ( platformlist[i].visible==1 &&cposy+collisionrect.h <=platformlist[i].platformrect.y && \
                            checkcollision(&collisionrect,&(SDL_Rect){platformlist[i].platformrect.x,platformlist[i].platformrect.y,BRICKW,1}))
                    {
                        playaudio(JUMPSOUND,snd);
                        doodle.y=platformlist[i].platformrect.y-collisionrect.h;

                        collisionrect.y=doodle.y;//we must set this equal to doodle.y because after collision we have \
                        set new position of doodle
                            //SDL_WM_SetCaption("platform collision",0);

                            if ( doodletype ==normal && platformlist[i].powerup !=singlespring&& platformlist[i].powerup !=none && checkcollision(&collisionrect,&platformlist[i].poweruprect))
                            {
                                doodletype = platformlist[i].powerup ;
                                if (doodletype==rocket)  playaudio(ROCKETSOUND,snd);
                                else if (doodletype==copter)  playaudio(COPTERSOUND,snd);
                                else if (doodletype==springshoeunpressed) {
                                    playaudio(SPRINGSHOESOUND,snd);
                                    doodle.y = platformlist[i].platformrect.y-doodlenormal[8]->clip_rect.h;//at first doodletype

                                }
                                // if (doodletype==singlespring)MessageBox(0,"single spring",0,0);
                                platformlist[i].powerup=none;

                            } else //separate collision checking in case of single spring i.e. collision takes place only when player jumps from up
                                if ( doodletype ==normal && platformlist[i].powerup ==singlespring &&cposy+collisionrect.h <=platformlist[i].poweruprect.y&& checkcollision(&collisionrect,&platformlist[i].poweruprect))
                                {

                                    playaudio(SINGLESPRINGSOUND,snd);
                                    doodletype = platformlist[i].powerup ;
                                    doodle.y = platformlist[i].platformrect.y - platformlist[i].poweruprect.h-collisionrect.h;
                                    platformlist[i].powerup=singlespringpressed;
                                    springframe =0;//for spring pressing effect
                                    platformlist[i].poweruprect.w=17;//small spring sprite dimensions
                                    platformlist[i].poweruprect.h=9;
                                    platformlist[i].poweruprect.y=platformlist[i].platformrect.y- platformlist[i].poweruprect.h;

                                } else if (doodletype==springshoeunpressed) playaudio(SPRINGSHOESOUND,snd);
                        if (currentfloor!=i) {

                            currentfloor=i;
                        }
                        falling=0;
                        jumping=1;
                        if (doodletype==normal) height=MAXHEIGHT;
                        else if (doodletype == rocket)//now rocket has to be launched
                        {
                            rocketstatus = starting;//rocket has just started
                            height=1;
                        } else if (doodletype == copter)//now rocket has to be launched
                        {
                            copterstatus = starting;//rocket has just started
                            height=1;
                            curheliframe=0;//helcopter frame is initialized to zero
                        } else if (doodletype==singlespring) {

                            height=MAXSINGLESPRINGHEIGHT;

                        }  else if (doodletype==springshoeunpressed) {

                            height=MAXSPRINGSHOEHEIGHT;
                            ++springshoehits;
                            pressed=1;
                            springframe=0;

                            if (springshoehits >=SPRINGSHOEHITS) {
                                doodletype=normal;
                                springshoehits=0;
                            }


                        }

                        break;//collides only with one so no need to check other collisions in this loop
                    }
            }
        } else if (doodlefalling==1 && !gameover && !gamepause)
        {
            for (i=0;i<BRICKROWS;i++) // if doodle is falling down
            {
                platformlist[i].platformrect.y-=height;
                platformlist[i].poweruprect.y-=height;
                doodle.y-=1;//scrolling effect
            }

            //falling begin

            if (doodle.y <=80)//where should scrolling be stopped
            {
                gameover=1;//now display score
                if (writehighscore(score,0,1)) {
                    inputhighscore=1;
                    highscorelength=0;
                }
            }//falling end

        }

        if (!gameover && !gamepause) {
            if (left) {
                doodle.x+=xspeed;
                xspeed-=.5;
                if (fabs(xspeed)>=MAXSPEED) xspeed=-MAXSPEED;
            } else if (right) {
                doodle.x+=(int)xspeed;
                xspeed+=.5;
                if (fabs(xspeed)>=MAXSPEED) xspeed=MAXSPEED;
            } else {//decay speed
                if (xspeed >0) xspeed-=0.5;
                else if (xspeed<0) xspeed+=0.5;
                doodle.x+=(int)xspeed;
            }
            if (doodle.x + doodle.w <0) doodle.x = SCREEN_WIDTH;
            else if (doodle.x >SCREEN_WIDTH) doodle.x = -doodle.w;
            if (doodle.y +doodle.h > SCREEN_HEIGHT) {
                doodlefalling=1;//1 for falling
                doodle.y  = SCREEN_HEIGHT-doodle.h;
            }

            //increase time so that we can spawn another powerup or sliding platform
            poweruptime++;
            slidertime++;
            fallingplatformtime++;
            //handle sliding movement
            updateslidingmovement(platformlist);
        }
        //render
        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,255,255,255));//clear screen

        displayplatforms(screen,platformlist,platforms,powerups,&springframe,gamepause);
        displayfallingplatforms( screen,fallingrects,platforms[3],gamepause);
        //SDL_FillRect(screen,&doodle,0);/* player rect*/
        if (doodletype==normal || doodletype==singlespring)
            SDL_BlitSurface(doodlenormal[curdir],0,screen,&(SDL_Rect){doodle.x,doodle.y});
        else if (doodletype==springshoeunpressed) {
            if (pressed)    SDL_BlitSurface(doodlenormal[8 +curdir],0,screen,&(SDL_Rect){doodle.x,doodle.y});
            else SDL_BlitSurface(doodlenormal[10 +curdir],0,screen,&(SDL_Rect){doodle.x,doodle.y});
            if (!gamepause && ++springframe >4)
            {
                pressed =0;
                springframe=0;
            }
        }
        else if (doodletype==copter) {
            SDL_BlitSurface(doodlenormal[(doodletype+curdir)*2+curheliframe],0,screen,&(SDL_Rect){doodle.x,doodle.y-15});//helicopter doodle sprite is a bit larger than the normal doodle sprite so -15 to blit doodle a little up
            if (!gamepause)
            {curheliframe++;
                if (curheliframe >1) curheliframe=0;
            }
        }  else
            if (doodletype!=copter) SDL_BlitSurface(doodlenormal[doodletype *2+curdir],0,screen,&(SDL_Rect){doodle.x,doodle.y});

        displayscore(screen,score,scorefont,&scorerect[gameover]);
        //input name for highscore if score is good
        if (inputhighscore) {
            //  MessageBox(0,"GOOD score",0,0);
            SDL_Surface *msg  = TTF_RenderText_Blended(scorefont,"Enter your name:",(SDL_Color){0,0,0});
            SDL_BlitSurface(msg,0,screen,&(SDL_Rect){20,SCREEN_HEIGHT/2+msg->clip_rect.h+10});
            char tmpx[500];

            sprintf(tmpx,"%s_",highscorename);

            SDL_Surface *msg2 = TTF_RenderText_Blended(scorefont,tmpx,(SDL_Color){0,0,0});

            SDL_BlitSurface(msg2,0,screen,&(SDL_Rect){20+msg->clip_rect.w,SCREEN_HEIGHT/2+msg->clip_rect.h+10});
            SDL_FreeSurface(msg2);
            SDL_FreeSurface(msg);
        }
        if (gamepause)//handling paused game status
        {

            SDL_Surface *srf=SDL_CreateRGBSurfaceFrom(screen->pixels,SCREEN_WIDTH+SCORE_WIDTH,SCREEN_HEIGHT,32,0,0,0,0,0);
            SDL_FillRect(srf,&srf->clip_rect,SDL_MapRGB(srf->format,255,255,255));
            SDL_SetSurfaceAlphaMod(srf, 235);
            SDL_BlitSurface(srf,0,screen,&(SDL_Rect){0,0});
            SDL_FreeSurface(srf);
            srf = TTF_RenderText_Blended(scorefont,"Paused",(SDL_Color){0,0,0});
            SDL_BlitSurface(srf,0,screen,&(SDL_Rect){(SCREEN_WIDTH+SCORE_WIDTH-srf->clip_rect.w)/2,(SCREEN_HEIGHT-srf->clip_rect.h) /2});
            SDL_FreeSurface(srf);
        } else SDL_SetSurfaceAlphaMod(screen, SDL_ALPHA_OPAQUE);
		SDL_UpdateWindowSurface(window);
        //frame rate handling

        if (SDL_GetTicks() - tick < 1000/FRAME_RATE) SDL_Delay(1000/FRAME_RATE-SDL_GetTicks()+tick);
    }
    return 0;
}

void displayplatforms(SDL_Surface* screen, PLATFORM *plist,SDL_Surface **platforms,SDL_Surface**powerups,int* springframe,int gamepause) {
    int i;
    for ( i=0;i<BRICKROWS;i++) {
        if (plist[i].visible) {
            SDL_BlitSurface(platforms[ plist[i].platformcolor ],0,screen,&(SDL_Rect){plist[i].platformrect.x,plist[i].platformrect.y});
            if (plist[i].powerup !=none && plist[i].powerup !=singlespringpressed &&plist[i].powerup !=singlespring &&plist[i].powerup !=springshoeunpressed) {
                SDL_BlitSurface(powerups[ plist[i].powerup-1 ],0,screen,&(SDL_Rect){plist[i].poweruprect.x,plist[i].poweruprect.y});

            } else if (plist[i].powerup !=none &&plist[i].powerup ==springshoeunpressed) {
                SDL_BlitSurface(powerups[4 ],0,screen,&(SDL_Rect){plist[i].poweruprect.x,plist[i].poweruprect.y});

            } else if (plist[i].powerup ==singlespringpressed ||plist[i].powerup ==singlespring ) {
                SDL_BlitSurface(powerups[ plist[i].powerup-1 ],0,screen,&(SDL_Rect){plist[i].poweruprect.x,plist[i].poweruprect.y});
                if (!gamepause) (*springframe)++;
                if (*springframe >3&&plist[i].powerup ==singlespringpressed)//display pressed spring sprite for 3 frames then restore normal spring to give spring effect
                {
                    *springframe=0;
                    plist[i].powerup =singlespring;
                    plist[i].poweruprect.w= 19;
                    plist[i].poweruprect.h= 29;
                    plist[i].poweruprect.y = plist[i].platformrect.y-plist[i].poweruprect.h;
                }
            }
        }
    }
}

void initallplatforms(PLATFORM *plist,SDL_Rect* doodle,int *poweruptime,int*slidertime,int *fallingplatformtime) {
    int i,ptop=0;
    for ( i=0;i<BRICKROWS;i++)//initialize random platforms before starting the game
    {
        placeplatformrandom(&plist[i],poweruptime,slidertime,fallingplatformtime);

        if (plist[i].visible==1) {
            plist[i].platformrect.y +=ptop;
            if (plist[i].powerup !=none) plist[i].poweruprect.y =plist[i].platformrect.y-plist[i].poweruprect.h;
            doodle->y =plist[i].platformrect.y-doodle->h;
            doodle->x = plist[i].platformrect.x + 10;
        }
        ptop += BRICKH+BRICKGAP;
    }
}

void updateslidingmovement(PLATFORM *plist) {
    int i;
    for ( i=0;i<BRICKROWS;i++) {
        if (plist[i].platformcolor ==blue) {
            if (plist[i].direction ==left) {
                plist[i].platformrect.x -=SLIDESPEED;
                if (plist[i].platformrect.x <0) {
                    plist[i].platformrect.x=0;
                    plist[i].direction = right;
                }
            } else if (plist[i].direction ==right) {
                plist[i].platformrect.x +=SLIDESPEED;
                if (plist[i].platformrect.x +BRICKW >SCREEN_WIDTH) {
                    plist[i].platformrect.x=SCREEN_WIDTH-BRICKW;
                    plist[i].direction = left;
                }
            }
        }
    }
}

void displayscore(SDL_Surface *screen,int score,TTF_Font *font,SDL_Rect *scorerect) {
    SDL_FillRect(screen,&(SDL_Rect){SCREEN_WIDTH,0,SCORE_WIDTH,SCREEN_HEIGHT},SDL_MapRGB(screen->format,SCOREBACKGROUNDCOLOR));
    char scoretxt[20];
    sprintf(scoretxt,"%d",score);
    SDL_Surface *scoresrf=TTF_RenderText_Blended(font,scoretxt,(SDL_Color){0,0,0});
    SDL_BlitSurface(scoresrf,0,screen,scorerect);
    SDL_Rect scorerectcopy=*scorerect;
    scorerectcopy.y-=20;
    SDL_Surface *scoretx=TTF_RenderText_Blended(font,"Score:",(SDL_Color){0,0,0});
    SDL_BlitSurface(scoretx,0,screen,&scorerectcopy);
    SDL_FreeSurface(scoresrf);
    SDL_FreeSurface(scoretx);
}


void cleanupresources(SDL_Surface **s1,int c1,SDL_Surface **s2,int c2,SDL_Surface **s3,int c3,TTF_Font* font) {
    int i;
    for (i=0;i<c1;SDL_FreeSurface(s1[i]),i++);
    for (i=0;i<c2;SDL_FreeSurface(s2[i]),i++);
    for (i=0;i<c3;SDL_FreeSurface(s3[i]),i++);
    TTF_CloseFont(font);
}

int writehighscore(int score,char*name,int justcheck) {
    FILE *fp = fopen("highscores.dat","a+b");
    int i;
    SCOREDATA scorelist[MAXHIGHSCORE],hghscore;
    if (!justcheck) strcpy(hghscore.name,name);
    hghscore.score=score;
    int count=retrievelist(scorelist,fp);

    if (count <MAXHIGHSCORE && !justcheck) {
        fwrite(&hghscore,sizeof(hghscore),1,fp);
        fclose(fp);
    } else if (count <MAXHIGHSCORE && justcheck) {
        fclose(fp);
        return 1;

    } else { //check if this score is highscore

        for (i=0;i<count;i++) {
            if (scorelist[i].score <score) {
                if (justcheck) {
                    fclose(fp);
                    return 1;
                } else {
                    //we have a high score here
                    SCOREDATA tmpcopy;

                    tmpcopy= scorelist[i];
                    scorelist[i] = hghscore;
                    int j;
                    SCOREDATA cpy;
                    for (j=i+1;j<count;j++) {
                        cpy = scorelist[j];
                        scorelist[j] = tmpcopy;
                        tmpcopy =cpy;
                    }

                    break;
                }
            }
        }

        if (i<count && !justcheck) // a highscore was found
        {
            //so we have to rewrite all the scores
            fclose(fp);
            fp = fopen("highscores.dat","wb");
            for (i=0;i<count;i++) {
                fwrite(scorelist+i,sizeof(*scorelist),1,fp);
            }
            fclose(fp);
        }
    }
    return 0;
}

void displayfallingplatforms(SDL_Surface* screen,PLATFORMFALL *fallingrects,SDL_Surface*fallpicture,int gamepause) {
    int i;
    for (i=0;i<BRICKROWS;i++) {
        if (fallingrects[i].falling) {
            SDL_BlitSurface(fallpicture,0,screen,&(SDL_Rect){fallingrects[i].rect.x,fallingrects[i].rect.y});
            if (!gamepause) {
                fallingrects[i].rect.y+=PLATFORMFALLSPEED;
                if  (fallingrects[i].rect.y>=SCREEN_HEIGHT) fallingrects[i].falling=0;
            }
        }
    }
}
