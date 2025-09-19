#include "main.h"

int AUX_WaitEventTimeout(SDL_Event* evt, Uint32* ms) {
    Uint32 startTime = SDL_GetTicks();

    int ret = SDL_WaitEventTimeout(evt, *ms);

    Uint32 endTime = SDL_GetTicks();
    Uint32 elapsed = endTime - startTime;

    if (elapsed >= *ms) {
        *ms = 0;
    } else {
        *ms -= elapsed;
    }

    return ret;
}

void AUX_Delay(Uint32 ms) {
    Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < ms) {
       
    }
}
