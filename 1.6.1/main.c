// 1.6.1
#include <SDL2/SDL.h>
#include <stdlib.h>
#include "../1.5.2/main.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FINISH_LINE_X 700

int main(int argc, char* args[])
{
    /* INICIALIZAÇÃO */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("1.6.1", 
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event evt;
    int quit = 0;
    Uint32 timeout = 16;
    Uint32 lastTime = SDL_GetTicks();
    Uint32 finishTimeStart = 0;

    // Retângulo 1 (Tempo)
    SDL_Rect rectTime = {100, 50, 50, 50};
    int velX = 3;
    int finishedTime = 0;

    // Retângulo 2 (Teclado)
    SDL_Rect rectKey = {100, 200, 50, 50};
    int finishedKey = 0;

    // Retângulo 3 (Mouse)
    SDL_Rect rectMouse = {100, 350, 50, 50};
    int finishedMouse = 0;

    // Vencedor
    char winner[20] = "";
    int allFinished = 0;

    /* EXECUÇÃO */
    while (!quit) {

       if (AUX_WaitEventTimeout(&evt, &timeout)) {
            if (evt.type == SDL_QUIT) quit = 1;

            // Teclado
            if (evt.type == SDL_KEYDOWN && !finishedKey) {
                switch (evt.key.keysym.sym) {
                    case SDLK_LEFT:  
                        rectKey.x -= 10; 
                        break;
                    case SDLK_RIGHT: 
                        rectKey.x += 10; 
                        break;
                }

                // Limites de tela
                if (rectKey.x < 0) rectKey.x = 0;
                if (rectKey.x + rectKey.w > SCREEN_WIDTH) rectKey.x = SCREEN_WIDTH - rectKey.w;
            }

            // Mouse
            if (evt.type == SDL_MOUSEMOTION && !finishedMouse) {
                rectMouse.x = evt.motion.x - rectMouse.w / 2;

                // Limites de tela
                if (rectMouse.x < 0) rectMouse.x = 0;
                if (rectMouse.x + rectMouse.w > SCREEN_WIDTH) rectMouse.x = SCREEN_WIDTH - rectMouse.w;
            }
        }

        // Tempo
        Uint32 currentTime = SDL_GetTicks();
        if (!finishedTime && currentTime > lastTime + 16) {
            rectTime.x += velX;

            // Verifica colisão com as paredes para "Bater e voltar"
            if (rectTime.x <= 0 || rectTime.x + rectTime.w >= SCREEN_WIDTH) {
                velX = -velX; 
            }

            lastTime = currentTime;
        }

         // Verifica linha de chegada
        if (!finishedTime && rectTime.x + rectTime.w >= FINISH_LINE_X + 50) {
            finishedTime = 1;
            if (strlen(winner) == 0) strcpy(winner, "Tempo (vermelho)");
        }
        if (!finishedKey && rectKey.x + rectKey.w >= FINISH_LINE_X + 50) {
            finishedKey = 1;
            if (strlen(winner) == 0) strcpy(winner, "Teclado (azul)");
        }
        if (!finishedMouse && rectMouse.x + rectMouse.w >= FINISH_LINE_X + 50) {
            finishedMouse = 1;
            if (strlen(winner) == 0) strcpy(winner, "Mouse (verde)");
        }

        if (finishedTime && finishedKey && finishedMouse && !allFinished) {
            allFinished = 1;
            printf("Vencedor: %s\n", winner);
            finishTimeStart = SDL_GetTicks();
        }

        if (allFinished && SDL_GetTicks() - finishTimeStart >= 2000) {
            rectTime.x = rectKey.x = rectMouse.x = 100;
            finishedTime = finishedKey = finishedMouse = 0;
            allFinished = 0;
            strcpy(winner, "");
            finishTimeStart = 0; // reset
        }


        // Renderização
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        // Linha de chegada
        int squareSize = 20;
        int finishWidth = SCREEN_WIDTH - FINISH_LINE_X;

        for (int y = 0; y < SCREEN_HEIGHT; y += squareSize) {
            for (int x = 0; x < finishWidth; x += squareSize) {
                int checker = ((x / squareSize) + (y / squareSize)) % 2;
                if (checker == 0) SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                else SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
                SDL_Rect sq = {FINISH_LINE_X + x, y, squareSize, squareSize};
                SDL_RenderFillRect(ren, &sq);
            }
        }

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
