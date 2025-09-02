// 1.5.1 - Três retângulos: tempo (bate na parede), teclado e mouse (com limites)
#include <SDL2/SDL.h>
#include <stdlib.h>
#include "../1.5.2/main.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* args[])
{
    /* INICIALIZAÇÃO */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("1.5.1", 
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event evt;
    int quit = 0;
    Uint32 timeout = 16;

    // Retângulo 1 (Tempo)
    SDL_Rect rectTime = {100, 50, 50, 50};
    int velX = 5;
    Uint32 lastTime = SDL_GetTicks();

    // Retângulo 2 (Teclado)
    SDL_Rect rectKey = {100, 200, 50, 50};

    // Retângulo 3 (Mouse)
    SDL_Rect rectMouse = {100, 350, 50, 50};

    /* EXECUÇÃO */
    while (!quit) {

       if (AUX_WaitEventTimeoutCount(&evt, &timeout)) {
            if (evt.type == SDL_QUIT) quit = 1;

            // Teclado
            if (evt.type == SDL_KEYDOWN) {
                switch (evt.key.keysym.sym) {
                    case SDLK_UP:    
                        rectKey.y -= 5; 
                        break;
                    case SDLK_DOWN:  
                        rectKey.y += 5; 
                        break;
                    case SDLK_LEFT:  
                        rectKey.x -= 5; 
                        break;
                    case SDLK_RIGHT: 
                        rectKey.x += 5; 
                        break;
                }

                // Limites de tela
                if (rectKey.x < 0) rectKey.x = 0;
                if (rectKey.y < 0) rectKey.y = 0;
                if (rectKey.x + rectKey.w > SCREEN_WIDTH) rectKey.x = SCREEN_WIDTH - rectKey.w;
                if (rectKey.y + rectKey.h > SCREEN_HEIGHT) rectKey.y = SCREEN_HEIGHT - rectKey.h;
            }

            // Mouse
            if (evt.type == SDL_MOUSEMOTION) {
                rectMouse.x = evt.motion.x - rectMouse.w / 2;
                rectMouse.y = evt.motion.y - rectMouse.h / 2;

                // Limites de tela
                if (rectMouse.x < 0) rectMouse.x = 0;
                if (rectMouse.y < 0) rectMouse.y = 0;
                if (rectMouse.x + rectMouse.w > SCREEN_WIDTH) rectMouse.x = SCREEN_WIDTH - rectMouse.w;
                if (rectMouse.y + rectMouse.h > SCREEN_HEIGHT) rectMouse.y = SCREEN_HEIGHT - rectMouse.h;
            }
        }

        // Tempo
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastTime + 16) {
            rectTime.x += velX;

            // Verifica colisão com as paredes para "Bater e voltar"
            if (rectTime.x <= 0 || rectTime.x + rectTime.w >= SCREEN_WIDTH) {
                velX = -velX; 
            }

            lastTime = currentTime;
        }

        // Renderização
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        // Retângulo 1 (tempo) - vermelho
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &rectTime);

        // Retângulo 2 (teclado) - azul
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
        SDL_RenderFillRect(ren, &rectKey);

        // Retângulo 3 (mouse) - verde
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        SDL_RenderFillRect(ren, &rectMouse);

        SDL_RenderPresent(ren);
    }

    /* FINALIZAÇÃO */
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
