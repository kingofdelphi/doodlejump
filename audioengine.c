#include "sdldecl.h"


void playaudio(int audiotype,Mix_Chunk **snd) {

Mix_PlayChannel(1,snd[audiotype],0);

//Mix_FreeChunk(snd);
}
