// 1.5.1 - Animação orientada a eventos com meteoro
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../1.5.2/main.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_STARS 200
#define SPEED 10           
#define SWAY_AMPLITUDE 30 
#define SWAY_SPEED 2  

typedef struct {
    int x, y, r;
} Star;

typedef struct {
    int x, y;
    int active;
    int speed;
} Meteor;

int main(int argc, char* args[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("Foguete com Meteoro",
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

    Meteor meteor = {0, 0, 0, 400};

    int rocketY = 200;
    float swayTime = 0.0f;

    int quit = 0;
    SDL_Event evt;
    Uint32 timeout = 16;

    Uint32 lastTime = SDL_GetTicks();

    // Estado do foguete
    int rocketHit = 0;
    float tremorTime = 0.0f;

    while(!quit){

        if (AUX_WaitEventTimeout(&evt, &timeout)) {

            if(evt.type == SDL_QUIT) quit = 1;

            //Clique gera meteoro na coordenada X do mouse, Y=0 (Apenas 1 por vez)
            if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT && !meteor.active){
                meteor.active = 1;
                int mouseX, mouseY;
                Uint32 buttonState = SDL_GetMouseState(&mouseX, &mouseY);
                meteor.x = mouseX;
                meteor.y = -50;
            }
        }

        // Tempo
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        swayTime += deltaTime;

        // Movimento do foguete
        int rocketX = SCREEN_WIDTH/2 + SWAY_AMPLITUDE * sinf(swayTime * SWAY_SPEED);

        // Atualizar estrelas
        for(int i=0; i<NUM_STARS; i++){
            stars[i].y += SPEED * deltaTime * 50;
            if(stars[i].y > SCREEN_HEIGHT){
                stars[i].y = 0;
                stars[i].x = rand() % SCREEN_WIDTH;
            }
        }

        // Atualizar meteoro
        if(meteor.active){
            meteor.y += meteor.speed * deltaTime;
            if(meteor.y > SCREEN_HEIGHT) meteor.active = 0;

            // Checagem de colisão simples
            if(meteor.y+20 > rocketY && meteor.y < rocketY+250 &&
               meteor.x > rocketX-60 && meteor.x < rocketX+60){
                rocketHit = 1;
                tremorTime = 0.5f;
                meteor.active = 0;
            }
        }

        // Se foguete atingido: treme e desce
        if(rocketHit){
            rocketY += 100 * deltaTime;
            swayTime += 30 * deltaTime;
            tremorTime -= deltaTime;
            if(tremorTime <= 0){
                rocketHit = 0;
                rocketY = 200; // volta posição
            }
        }

        // Renderização
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Estrelas
        for(int i=0; i<NUM_STARS; i++){
            filledCircleRGBA(ren, stars[i].x, stars[i].y, stars[i].r, 255, 255, 0, 255);
        }

        // Meteoro
        if(meteor.active){
            filledCircleRGBA(ren, meteor.x, meteor.y, 20, 200, 100, 50, 255);
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

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
