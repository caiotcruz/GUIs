// 2.1
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../1.5.2/main.h"

//Defines para configuração de tela
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

//Define para as estrelas ao fundo
#define NUM_STARS 200
#define SPEED 10

//Define para balanço do foguete
#define SWAY_AMPLITUDE 30
#define SWAY_SPEED 2

//Define para Contadores de frame
#define FOGOETE_FRAME_COUNT 6 
#define SHOOT_FRAME_COUNT 5
#define FAIL_SHOOT_FRAME_COUNT 8

//Escala dos frames
#define shoot_SCALE 2.5f
#define ROCKET_SCALE 2.0f        

//Máximo de tiros na tela
#define MAX_SHOOTS  16

// Definições para o sistema de multi-clique
#define MULTICLICK_TIMEOUT 250

typedef struct {
    int x, y, r;
} Star;

typedef struct {
    int x, y; 
    int active;
    int speed;
    float scale;
    int frame;  
    float frameTime;  
    int isFailShoot; //Boolean para se tiro funcionou ou não
    float lifeTime; // TimeSpan do FailedShoot (Se não falhar não precisa de lifetime, pois acaba somente ao fim da tela.)
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
    SDL_Texture* rocketTex = IMG_LoadTexture(ren, "2.1/fogueteSprite.png");
    int rocketTexW, rocketTexH;
    SDL_QueryTexture(rocketTex, NULL, NULL, &rocketTexW, &rocketTexH);

    SDL_Texture* shootTex = IMG_LoadTexture(ren, "2.1/ShootSpriteSheet.png");
    int shootTexW, shootTexH;
    SDL_QueryTexture(shootTex, NULL, NULL, &shootTexW, &shootTexH);

    SDL_Texture* failShootTex = IMG_LoadTexture(ren, "2.1/FailShoot.png");
    int failShootTexW, failShootTexH;
    SDL_QueryTexture(failShootTex, NULL, NULL, &failShootTexW, &failShootTexH);

    Star stars[NUM_STARS];
    for(int i=0; i<NUM_STARS; i++){
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].r = 1 + rand()%2;
    }

    //Inicializa array de shoots, inativos inicialmente
    Shoot shoots[MAX_SHOOTS];
    for (int i = 0; i < MAX_SHOOTS; i++) {
        shoots[i].active = 0;
    }

    float swayTime = 0.0f;
    int rocketHit = 0;
    float tremorTime = 0.0f;

    int clickCount = 0;
    Uint32 lastClickTimestamp = 0;
     int firstClickX = 0;
    int firstClickY = 0;

    int quit = 0;
    SDL_Event evt;
    Uint32 timeout = 16;
    Uint32 lastTime = SDL_GetTicks();
    int rocketX = 200;

    while(!quit){
        Uint32 currentTime = SDL_GetTicks();
        timeout = 16;

        if (clickCount > 0 && currentTime - lastClickTimestamp > MULTICLICK_TIMEOUT) {
            if (clickCount >= 1) { 
                SDL_Event user_event;
                SDL_zero(user_event);
                user_event.type = SDL_USEREVENT;
                user_event.user.code = clickCount;
                SDL_PushEvent(&user_event);
            }
            clickCount = 0;
        }

        while (AUX_WaitEventTimeout(&evt, &timeout)) {
            if(evt.type == SDL_QUIT) quit = 1;

            if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
                if (clickCount == 0) {
                    clickCount = 1;
                    lastClickTimestamp = SDL_GetTicks();
                    firstClickX = evt.button.x;
                    firstClickY = evt.button.y;
                } else {
                    int currentClickX = evt.button.x;
                    int currentClickY = evt.button.y;

                    // Verifica se o novo clique está no local EXATO do primeiro
                    if (currentClickX == firstClickX && currentClickY == firstClickY) {
                        // Se estiver, continua a sequência
                        clickCount++;
                        lastClickTimestamp = SDL_GetTicks();
                    } else {
                        // Se não estiver, a sequência é quebrada.
                        SDL_Event user_event;
                        SDL_zero(user_event);
                        user_event.type = SDL_USEREVENT;
                        user_event.user.code = clickCount;
                        SDL_PushEvent(&user_event);

                        // E inicia uma nova sequência com o clique atual
                        clickCount = 1;
                        lastClickTimestamp = SDL_GetTicks();
                        firstClickX = currentClickX;
                        firstClickY = currentClickY;
                    }
                }
            }
            
            if (evt.type == SDL_USEREVENT) {
                int shoot_index = -1;
                for (int i = 0; i < MAX_SHOOTS; i++) {
                    if (!shoots[i].active) {
                        shoot_index = i;
                        break;
                    }
                }

                if (shoot_index != -1) {
                    int n_clicks = evt.user.code;

                    shoots[shoot_index].active = 1;
                    shoots[shoot_index].y = firstClickY; // Usa a posição Y do primeiro clique
                    shoots[shoot_index].x = SCREEN_WIDTH + 50;
                    shoots[shoot_index].frame = 0;
                    shoots[shoot_index].frameTime = 0.0f;
                    shoots[shoot_index].isFailShoot = 0;
                    shoots[shoot_index].lifeTime = 0.0f;

                    if (n_clicks == 1){
                        shoots[shoot_index].isFailShoot = 1;
                        shoots[shoot_index].speed = 300;
                        shoots[shoot_index].scale = 2.0f;
                        shoots[shoot_index].lifeTime = 0.8f;
                    }
                    else if (n_clicks == 2) {
                        shoots[shoot_index].speed = 400;
                        shoots[shoot_index].scale = 2.5f;
                    } else if (n_clicks == 3) {
                        shoots[shoot_index].speed = 650;
                        shoots[shoot_index].scale = 3.5f;
                    } else if (n_clicks == 4) {
                        shoots[shoot_index].speed = 900;
                        shoots[shoot_index].scale = 10.5f;
                    }
                    else if (n_clicks >= 5) {
                        shoots[shoot_index].isFailShoot = 1;
                        shoots[shoot_index].speed = 300;
                        shoots[shoot_index].scale = 2.0f;
                        shoots[shoot_index].lifeTime = 0.8f;
                    }
                }
            }
        }

        
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        swayTime += deltaTime;
        int rocketY = SCREEN_HEIGHT/2 + SWAY_AMPLITUDE * sinf(swayTime * SWAY_SPEED);

        for(int i=0; i<NUM_STARS; i++){
            stars[i].x -= SPEED * deltaTime * 50;
            if(stars[i].x < 0){
                stars[i].x = SCREEN_WIDTH;
                stars[i].y = rand() % SCREEN_HEIGHT;
            }
        }

        //Atualização e colisão em um loop dos tiros.
        for (int i = 0; i < MAX_SHOOTS; i++) {
            
            if (!shoots[i].active) {
                continue;
            }

            //Se está ativo, mas foram 1 ou 5+ clicks
            if (shoots[i].isFailShoot) {
                shoots[i].x -= shoots[i].speed * deltaTime;
                shoots[i].lifeTime -= deltaTime;
                shoots[i].frameTime += deltaTime;

                if (shoots[i].frameTime >= 0.1f) {
                    shoots[i].frame = (shoots[i].frame + 1);
                    if (shoots[i].frame >= FAIL_SHOOT_FRAME_COUNT) {
                        shoots[i].frame = FAIL_SHOOT_FRAME_COUNT - 1;
                    }
                    shoots[i].frameTime = 0.0f;
                }
                //D esativar o tiro quando o tempo de vida acaba
                if (shoots[i].lifeTime <= 0) {
                    shoots[i].active = 0;
                }
            }
            else { //Se está ativo e foram 2,3 ou 4 clicks
                shoots[i].x -= shoots[i].speed * deltaTime;

                shoots[i].frameTime += deltaTime;
                if(shoots[i].frameTime >= 0.1f){
                    shoots[i].frame = (shoots[i].frame + 1) % SHOOT_FRAME_COUNT;
                    shoots[i].frameTime = 0.0f;
                }

                //Desativar o tiro se sair da tela
                if (shoots[i].x < -50) {
                    shoots[i].active = 0;
                }

                int rocketFrameW = rocketTexW / FOGOETE_FRAME_COUNT;
                int rocketFrameH = rocketTexH;

                if(shoots[i].x - 30 < rocketX + rocketFrameW * ROCKET_SCALE &&
                   shoots[i].x + 30 > rocketX &&
                   shoots[i].y > rocketY - (rocketFrameH * ROCKET_SCALE)/2 &&
                   shoots[i].y < rocketY + (rocketFrameH * ROCKET_SCALE)/2)
                {
                    rocketHit = 1;
                    tremorTime = 0.5f;
                    //Desativar o tiro ao colidir!
                    shoots[i].active = 0;
                }
            }
        }

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

        // Lógica de renderização
        for (int i = 0; i < MAX_SHOOTS; i++) {
            if (!shoots[i].active) {
                continue;
            }

            SDL_Texture* currentShootTex;
            int currentShootTexW, currentShootTexH;
            int currentShootFrameCount;

            if (shoots[i].isFailShoot) {
                currentShootTex = failShootTex;
                currentShootTexW = failShootTexW;
                currentShootTexH = failShootTexH;
                currentShootFrameCount = FAIL_SHOOT_FRAME_COUNT;
            } else {
                currentShootTex = shootTex;
                currentShootTexW = shootTexW;
                currentShootTexH = shootTexH;
                currentShootFrameCount = SHOOT_FRAME_COUNT;
            }

            int shootFrameW = currentShootTexW / currentShootFrameCount;
            int shootFrameH = currentShootTexH;
            SDL_Rect shootSrc = {
                shoots[i].frame * shootFrameW,
                0,
                shootFrameW,
                shootFrameH
            };
            SDL_Rect shootDst = {
                (int)(shoots[i].x - (shootFrameW * shoots[i].scale)/2),
                (int)(shoots[i].y - (shootFrameH * shoots[i].scale)/2),
                (int)(shootFrameW * shoots[i].scale),
                (int)(shootFrameH * shoots[i].scale)
            };
            SDL_RenderCopy(ren, currentShootTex, &shootSrc, &shootDst);
        }

        // Foguete
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
    }

    // Limpeza
    SDL_DestroyTexture(rocketTex);
    SDL_DestroyTexture(shootTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}