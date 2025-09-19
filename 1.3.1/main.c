// 1.3.1 - - Faça uma animação em ciclo com algum padrão (ex, andar em círculo).
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../1.5.2/main.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_STARS 200
#define SPEED 10           // pixels/segundo
#define SWAY_AMPLITUDE 30  // amplitude do balanço
#define SWAY_SPEED 2       // frequência do balanço

typedef struct {
    int x, y, r;
} Star;

int main(int argc, char* args[])
{
    /* INICIALIZAÇÃO */
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("Foguete Animado com Balanço",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // Criar estrelas
    Star stars[NUM_STARS];
    for(int i=0; i<NUM_STARS; i++){
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].r = 1 + rand()%2;
    }

    int rocketY = 200;      // posição vertical fixa
    float swayTime = 0.0f;  // tempo acumulado para balanço

    int quit = 0;
    SDL_Event evt;
    Uint32 timeout = 16;
    Uint32 lastTime = SDL_GetTicks();

    /* EXECUÇÃO */
    while(!quit){

        // Necessário para sair sem crashar
        if (AUX_WaitEventTimeout(&evt, &timeout)){
            if(evt.type == SDL_QUIT) quit = 1;
        }

        // Tempo
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        swayTime += deltaTime;

        // Movimento do balanço do foguete
        int rocketX = SCREEN_WIDTH/2 + SWAY_AMPLITUDE * sinf(swayTime * SWAY_SPEED);

        // Atualizar estrelas
        for(int i=0; i<NUM_STARS; i++){
            stars[i].y += SPEED * deltaTime * 50;
            if(stars[i].y > SCREEN_HEIGHT){
                stars[i].y = 0;
                stars[i].x = rand() % SCREEN_WIDTH;
            }
        }

        // Render
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Estrelas
        for(int i=0; i<NUM_STARS; i++){
            filledCircleRGBA(ren, stars[i].x, stars[i].y, stars[i].r, 255, 255, 0, 255);
        }

        // Corpo do foguete
        boxRGBA(ren, rocketX-50, rocketY, rocketX+50, rocketY+250, 255, 255, 255, 255);

        // Ponta do foguete
        Sint16 vx[3] = {rocketX-50, rocketX+50, rocketX};
        Sint16 vy[3] = {rocketY, rocketY, rocketY-80};
        filledPolygonRGBA(ren, vx, vy, 3, 255, 0, 0, 255);

        // Asas
        Sint16 vx2[3] = {rocketX-50, rocketX-100, rocketX-50};
        Sint16 vy2[3] = {rocketY+100, rocketY+250, rocketY+250};
        filledPolygonRGBA(ren, vx2, vy2, 3, 180, 180, 180, 255);

        Sint16 vx3[3] = {rocketX+50, rocketX+100, rocketX+50};
        Sint16 vy3[3] = {rocketY+100, rocketY+250, rocketY+250};
        filledPolygonRGBA(ren, vx3, vy3, 3, 180, 180, 180, 255);

        // Chamas
        Sint16 vx4[3] = {rocketX-40, rocketX+40, rocketX};
        Sint16 vy4[3] = {rocketY+250, rocketY+250, rocketY+320};
        filledPolygonRGBA(ren, vx4, vy4, 3, 255, 140, 0, 255);
        filledCircleRGBA(ren, rocketX, rocketY+310, 15, 255, 255, 0, 255);

        // Janela
        filledCircleRGBA(ren, rocketX, rocketY+80, 40, 0, 100, 255, 255);
        circleRGBA(ren, rocketX, rocketY+80, 40, 255, 255, 255, 255);

        SDL_RenderPresent(ren);
        AUX_Delay(16);
    }

    /* FINALIZAÇÃO */
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
