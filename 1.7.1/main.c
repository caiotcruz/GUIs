// 1.7.1
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
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

#define FOGOETE_FRAME_COUNT 6 
#define SHOOT_FRAME_COUNT 5
#define shoot_SCALE 2.5f
#define ROCKET_SCALE 2.0f          

typedef struct {
    int x, y, r;
} Star;

typedef struct {
    int x, y;
    int active;
    int speed;
    int frame;  
    float frameTime;  
} Shoot;

int main(int argc, char* args[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* win = SDL_CreateWindow("Foguete Idle com Tiro",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // Carregar sprites
    SDL_Texture* rocketTex = IMG_LoadTexture(ren, "1.7.1/fogueteSprite.png");
    int rocketTexW, rocketTexH;
    SDL_QueryTexture(rocketTex, NULL, NULL, &rocketTexW, &rocketTexH);

    SDL_Texture* shootTex = IMG_LoadTexture(ren, "1.7.1/ShootSpriteSheet.png");
    int shootTexW, shootTexH;
    SDL_QueryTexture(shootTex, NULL, NULL, &shootTexW, &shootTexH);

    // Criar estrelas
    Star stars[NUM_STARS];
    for(int i=0; i<NUM_STARS; i++){
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].r = 1 + rand()%2;
    }

    Shoot shoot = {0,0,0,400,0,0.0f};

    float swayTime = 0.0f;
    int rocketHit = 0;
    float tremorTime = 0.0f;

    int quit = 0;
    SDL_Event evt;
    Uint32 timeout = 16;
    Uint32 lastTime = SDL_GetTicks();

    int rocketX = 200;

    while(!quit){

        if (AUX_WaitEventTimeout(&evt, &timeout)) {
            if(evt.type == SDL_QUIT) quit = 1;

            if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT && !shoot.active){
                shoot.active = 1;
                int mouseX, mouseY;
                Uint32 buttonState = SDL_GetMouseState(&mouseX, &mouseY);
                shoot.y = mouseY;
                shoot.x = SCREEN_WIDTH + 50;
                shoot.frame = 0;
                shoot.frameTime = 0.0f;
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        swayTime += deltaTime;

        int rocketY = SCREEN_HEIGHT/2 + SWAY_AMPLITUDE * sinf(swayTime * SWAY_SPEED);

        // Atualizar estrelas
        for(int i=0; i<NUM_STARS; i++){
            stars[i].x -= SPEED * deltaTime * 50;
            if(stars[i].x < 0){
                stars[i].x = SCREEN_WIDTH;
                stars[i].y = rand() % SCREEN_HEIGHT;
            }
        }

        // Atualizar SHOOT
        if(shoot.active){
            shoot.x -= shoot.speed * deltaTime;

            // Animação em loop
            shoot.frameTime += deltaTime;
            if(shoot.frameTime >= 0.1f){ // muda frame a cada 0.1s
                shoot.frame = (shoot.frame + 1) % SHOOT_FRAME_COUNT;
                shoot.frameTime = 0.0f;
            }

            if(shoot.x < 0) shoot.active = 0;

            // Checagem de colisão
            int rocketFrameW = rocketTexW / FOGOETE_FRAME_COUNT;
            int rocketFrameH = rocketTexH;

            if(shoot.x - 30 < rocketX + rocketFrameW * ROCKET_SCALE &&
               shoot.x + 30 > rocketX &&
               shoot.y > rocketY - (rocketFrameH * ROCKET_SCALE)/2 &&
               shoot.y < rocketY + (rocketFrameH * ROCKET_SCALE)/2){
                rocketHit = 1;
                tremorTime = 0.5f;
                shoot.active = 0;
            }
        }

        // Tremor do foguete
        if(rocketHit){
            int tremorOffset = (int)(20 * sinf(swayTime * 50));
            rocketY += tremorOffset;
            tremorTime -= deltaTime;
            if(tremorTime <= 0){
                rocketHit = 0;
                rocketY = SCREEN_HEIGHT/2 + SWAY_AMPLITUDE * sinf(swayTime * SWAY_SPEED);
            }
        }

        // Renderização
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        for(int i=0;i<NUM_STARS;i++){
            filledCircleRGBA(ren, stars[i].x, stars[i].y, stars[i].r, 255,255,0,255);
        }

        // Shoot
        if(shoot.active){
            int shootFrameW = shootTexW / SHOOT_FRAME_COUNT;
            int shootFrameH = shootTexH;
            SDL_Rect shootSrc = {
                shoot.frame * shootFrameW,
                0,
                shootFrameW,
                shootFrameH
            };
            SDL_Rect shootDst = { 
                shoot.x - (shootFrameW * shoot_SCALE)/2, 
                shoot.y - (shootFrameH * shoot_SCALE)/2, 
                shootFrameW * shoot_SCALE, 
                shootFrameH * shoot_SCALE 
            };
            SDL_RenderCopy(ren, shootTex, &shootSrc, &shootDst);
        }

        // Foguete (loop em idle)
        static float rocketAnimTime = 0.0f;
        static int rocketFrame = 0;
        rocketAnimTime += deltaTime;
        if(rocketAnimTime >= 0.1f){
            rocketFrame = (rocketFrame + 1) % FOGOETE_FRAME_COUNT;
            rocketAnimTime = 0.0f;
        }

        int rocketFrameW = rocketTexW / FOGOETE_FRAME_COUNT;
        int rocketFrameH = rocketTexH;
        SDL_Rect rocketSrc = {
            rocketFrame * rocketFrameW,
            0,
            rocketFrameW,
            rocketFrameH
        };
        SDL_Rect rocketDst = { 
            rocketX, 
            rocketY - (rocketFrameH * ROCKET_SCALE)/2, 
            rocketFrameW * ROCKET_SCALE, 
            rocketFrameH * ROCKET_SCALE 
        };
        SDL_RenderCopy(ren, rocketTex, &rocketSrc, &rocketDst);

        SDL_RenderPresent(ren);
        AUX_Delay(16);
    }

    SDL_DestroyTexture(rocketTex);
    SDL_DestroyTexture(shootTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
