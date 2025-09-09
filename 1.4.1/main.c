// 1.4.1
#include <SDL2/SDL.h>
#include <stdlib.h>
#include "../1.5.2/main.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_RECTS 10

typedef struct {
    int r, g, b;
    SDL_Rect rect;
} FixedRect;

int main(int argc, char* args[])
{
    /* INICIALIZAÇÃO */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("1.4.1",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event evt;
    int quit = 0;
    Uint32 timeout = 16;

    SDL_Rect rectMain = { 100, 100, 50, 50 };

    FixedRect fixedRects[MAX_RECTS];
    int rectCount = 0;

    /* EXECUÇÃO */
    while (!quit) {

        if (AUX_WaitEventTimeout(&evt, &timeout)){
            if(evt.type == SDL_QUIT) quit = 1;

            // Mouse
            //Botao esquerdo - Cria
            if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT && rectCount < MAX_RECTS) {
                fixedRects[rectCount].rect.x = evt.button.x - 25;
                fixedRects[rectCount].rect.y = evt.button.y - 25;
                fixedRects[rectCount].rect.w = 50;
                fixedRects[rectCount].rect.h = 50;

                fixedRects[rectCount].r = rand() % 256;
                fixedRects[rectCount].g = rand() % 256;
                fixedRects[rectCount].b = rand() % 256;

                rectCount++;
            }

            //Botao direito - remove
            if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_RIGHT) {
                int mx = evt.button.x;
                int my = evt.button.y;

                for(int i=0; i<rectCount; i++) {
                    SDL_Rect rect = fixedRects[i].rect;
                    if(mx >= rect.x && mx <= rect.x + rect.w &&
                       my >= rect.y && my <= rect.y + rect.h) {
                        for(int j=i; j<rectCount-1; j++) {
                            fixedRects[j] = fixedRects[j+1];
                        }
                        rectCount--;
                        break; 
                    }
                }
            }


            // Teclado
            if (evt.type == SDL_KEYDOWN) {
                switch (evt.key.keysym.sym) {
                    case SDLK_UP:
                        rectMain.y -= 5;
                        break;
                    case SDLK_DOWN:
                        rectMain.y += 5;
                        break;
                    case SDLK_LEFT:
                        rectMain.x -= 5;
                        break;
                    case SDLK_RIGHT:
                        rectMain.x += 5;
                        break;
                }

                // Manter dentro dos limites da tela
                if (rectMain.x < 0) rectMain.x = 0;
                if (rectMain.y < 0) rectMain.y = 0;
                if (rectMain.x + rectMain.w > SCREEN_WIDTH) rectMain.x = SCREEN_WIDTH - rectMain.w;
                if (rectMain.y + rectMain.h > SCREEN_HEIGHT) rectMain.y = SCREEN_HEIGHT - rectMain.h;
            }
        }

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        // Desenhar retângulos fixos
        for(int i=0; i<rectCount; i++){
            SDL_SetRenderDrawColor(ren, fixedRects[i].r, fixedRects[i].g, fixedRects[i].b, 255);
            SDL_RenderFillRect(ren, &fixedRects[i].rect);
        }

        // Desenhar retângulo principal
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
        SDL_RenderFillRect(ren, &rectMain);

        SDL_RenderPresent(ren);
    }

    /* FINALIZAÇÃO */
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
