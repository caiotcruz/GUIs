#include "main.h"

int AUX_WaitEventTimeoutCount(SDL_Event* evt, Uint32* ms) {
    Uint32 startTime = SDL_GetTicks();

    if (SDL_PollEvent(evt)) {
        return 1; // evento capturado
    }

    Uint32 now = SDL_GetTicks();
    if (now - startTime >= *ms) {
        *ms = 0;
    } else {
        *ms -= (now - startTime);
    }

    // Pausa pra não fritar a CPU
    SDL_Delay(1);

    return 0;
}
