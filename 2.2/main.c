// 2.2 - Adaptado para SDL_USEREVENT
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../1.5.2/main.h"

// Defines para configuração de tela
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Define para as estrelas ao fundo
#define NUM_STARS 200
#define SPEED 10

// Define para balanço do foguete
#define SWAY_AMPLITUDE 30
#define SWAY_SPEED 2

// Define para Contadores de frame
#define FOGOETE_FRAME_COUNT 6 
#define SHOOT_FRAME_COUNT 5
#define FAIL_SHOOT_FRAME_COUNT 8

// Escala dos frames
#define ROCKET_SCALE 2.0f

// Máximo de tiros na tela
#define MAX_SHOOTS  16

// Definições para o sistema de multi-clique
#define MULTICLICK_TIMEOUT 250
#define DRAG_START_DISTANCE 5 

// ## NOVOS DEFINES: Códigos para os eventos de usuário ##
#define USER_EVENT_SHOOT       1
#define USER_EVENT_DRAG_START  2
#define USER_EVENT_DRAG_END    3

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
    int isFailShoot; 
    float lifeTime; // TimeSpan do FailedShoot
} Shoot;

int main(int argc, char* args[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* win = SDL_CreateWindow("Foguete Idle com Tiro e Arraste (SDL_USEREVENT)",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // Carregar sprites
    SDL_Texture* rocketTex = IMG_LoadTexture(ren, "2.2/fogueteSprite.png");
    int rocketTexW, rocketTexH;
    SDL_QueryTexture(rocketTex, NULL, NULL, &rocketTexW, &rocketTexH);

    SDL_Texture* shootTex = IMG_LoadTexture(ren, "2.2/ShootSpriteSheet.png");
    int shootTexW, shootTexH;
    SDL_QueryTexture(shootTex, NULL, NULL, &shootTexW, &shootTexH);

    SDL_Texture* failShootTex = IMG_LoadTexture(ren, "2.2/FailShoot.png");
    int failShootTexW, failShootTexH;
    SDL_QueryTexture(failShootTex, NULL, NULL, &failShootTexW, &failShootTexH);

    Star stars[NUM_STARS];
    for(int i=0; i<NUM_STARS; i++){
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].r = 1 + rand()%2;
    }

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
    
    // Variáveis para o arrasto
    int isDragging = 0;
    int isClickOnRocket = 0;
    int clickDownX = 0, clickDownY = 0;
    int originalRocketX, originalRocketY;

    int quit = 0;
    SDL_Event evt;
    Uint32 timeout = 16;
    Uint32 lastTime = SDL_GetTicks();

    // Posição do foguete
    int rocketX = 200;
    int rocketY = SCREEN_HEIGHT / 2;
    int rocketSwayCenterY = SCREEN_HEIGHT / 2;


    while(!quit){
        Uint32 currentTime = SDL_GetTicks();
        timeout = 16;

        // Lógica de timeout para o multi-clique
        if (clickCount > 0 && currentTime - lastClickTimestamp > MULTICLICK_TIMEOUT) {
            SDL_Event user_event;
            SDL_zero(user_event);
            user_event.type = SDL_USEREVENT;
            user_event.user.code = USER_EVENT_SHOOT; // Dispara evento de tiro
            user_event.user.data1 = (void*)(intptr_t)clickCount;
            user_event.user.data2 = (void*)(intptr_t)firstClickY;
            SDL_PushEvent(&user_event);
            clickCount = 0;
        }

        int rocketFrameW = rocketTexW / FOGOETE_FRAME_COUNT;
        int rocketFrameH = rocketTexH;
        SDL_Rect rocketDstRect = { 
            rocketX, 
            rocketY - (int)(rocketFrameH * ROCKET_SCALE)/2, 
            (int)(rocketFrameW * ROCKET_SCALE), 
            (int)(rocketFrameH * ROCKET_SCALE) 
        };

        while (AUX_WaitEventTimeout(&evt, &timeout)) {
            if(evt.type == SDL_QUIT) quit = 1;
            
            if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
                int mouseX = evt.button.x;
                int mouseY = evt.button.y;

                if (mouseX >= rocketDstRect.x && mouseX <= (rocketDstRect.x + rocketDstRect.w) &&
                    mouseY >= rocketDstRect.y && mouseY <= (rocketDstRect.y + rocketDstRect.h))
                {
                    isClickOnRocket = 1;
                    clickDownX = mouseX;
                    clickDownY = mouseY;
                }

                if (clickCount == 0) {
                    clickCount = 1;
                    lastClickTimestamp = SDL_GetTicks();
                    firstClickX = mouseX;
                    firstClickY = mouseY;
                } else {
                    if (mouseX == firstClickX && mouseY == firstClickY) {
                        clickCount++;
                        lastClickTimestamp = SDL_GetTicks();
                    } else {
                        SDL_Event user_event;
                        SDL_zero(user_event);
                        user_event.type = SDL_USEREVENT;
                        user_event.user.code = USER_EVENT_SHOOT;
                        user_event.user.data1 = (void*)(intptr_t)clickCount;
                        user_event.user.data2 = (void*)(intptr_t)firstClickY;
                        SDL_PushEvent(&user_event);

                        clickCount = 1;
                        lastClickTimestamp = SDL_GetTicks();
                        firstClickX = mouseX;
                        firstClickY = mouseY;
                    }
                }
            }

            if (evt.type == SDL_MOUSEMOTION) {
                if (isClickOnRocket && !isDragging) {
                    int dx = evt.motion.x - clickDownX;
                    int dy = evt.motion.y - clickDownY;
                    if ((dx*dx + dy*dy) > (DRAG_START_DISTANCE * DRAG_START_DISTANCE)) {
                        // Envia evento para iniciar o arrasto
                        SDL_Event user_event;
                        SDL_zero(user_event);
                        user_event.type = SDL_USEREVENT;
                        user_event.user.code = USER_EVENT_DRAG_START;
                        SDL_PushEvent(&user_event);
                        
                        isClickOnRocket = 0;
                        clickCount = 0;
                    }
                }

                if (isDragging) {
                    rocketX = evt.motion.x - rocketDstRect.w / 2;
                    rocketY = evt.motion.y;
                }
            }

            if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT) {
                if (isDragging) {
                    //Envia evento para terminar o arrasto
                    SDL_Event user_event;
                    SDL_zero(user_event);
                    user_event.type = SDL_USEREVENT;
                    user_event.user.code = USER_EVENT_DRAG_END;
                    SDL_PushEvent(&user_event);
                }
                isClickOnRocket = 0;
            }

            if (evt.type == SDL_KEYDOWN) {
                if (evt.key.keysym.sym == SDLK_ESCAPE) {
                    if (isDragging) {
                        //Envia evento para TERMINAR e restaurar a posição 
                        SDL_Event user_event;
                        SDL_zero(user_event);
                        user_event.type = SDL_USEREVENT;
                        user_event.user.code = USER_EVENT_DRAG_END;
                        SDL_PushEvent(&user_event);
                        
                        rocketX = originalRocketX;
                        rocketY = originalRocketY;
                        rocketSwayCenterY = originalRocketY;
                    }
                    isClickOnRocket = 0;
                    clickCount = 0;
                }
            }
            
            // Processar Eventos
            if (evt.type == SDL_USEREVENT) {
                // Evento de Atirar
                if (evt.user.code == USER_EVENT_SHOOT) {
                    int n_clicks = (int)(intptr_t)evt.user.data1;
                    int click_y_pos = (int)(intptr_t)evt.user.data2;
                    int shoot_index = -1;
                    for (int i = 0; i < MAX_SHOOTS; i++) {
                        if (!shoots[i].active) { shoot_index = i; break; }
                    }

                    if (shoot_index != -1) {
                        shoots[shoot_index].active = 1;
                        shoots[shoot_index].y = click_y_pos;
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
                        } else if (n_clicks == 2) {
                            shoots[shoot_index].speed = 400;
                            shoots[shoot_index].scale = 2.5f;
                        } else if (n_clicks == 3) {
                            shoots[shoot_index].speed = 650;
                            shoots[shoot_index].scale = 3.5f;
                        } else if (n_clicks == 4) {
                            shoots[shoot_index].speed = 900;
                            shoots[shoot_index].scale = 10.5f;
                        } else if (n_clicks >= 5) {
                            shoots[shoot_index].isFailShoot = 1;
                            shoots[shoot_index].speed = 300;
                            shoots[shoot_index].scale = 2.0f;
                            shoots[shoot_index].lifeTime = 0.8f;
                        }
                    }
                } 
                // Evento de Iniciar Arrasto
                else if (evt.user.code == USER_EVENT_DRAG_START) {
                    isDragging = 1;
                    originalRocketX = rocketX;
                    originalRocketY = rocketY;
                }
                // Evento de Terminar Arrasto
                else if (evt.user.code == USER_EVENT_DRAG_END) {
                    isDragging = 0;
                    rocketSwayCenterY = rocketY;
                }
            }
        }

        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        swayTime += deltaTime;
        
        if (!isDragging) {
            rocketY = rocketSwayCenterY + (int)(SWAY_AMPLITUDE * sinf(swayTime * SWAY_SPEED));
        }

        // Atualiza estrelas
        for(int i=0; i<NUM_STARS; i++){
            stars[i].x -= SPEED * deltaTime * 50;
            if(stars[i].x < 0){
                stars[i].x = SCREEN_WIDTH;
                stars[i].y = rand() % SCREEN_HEIGHT;
            }
        }

        // Atualiza e checa colisão dos tiros
        for (int i = 0; i < MAX_SHOOTS; i++) {
            if (!shoots[i].active) continue;

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
                if (shoots[i].lifeTime <= 0) shoots[i].active = 0;
            }
            else { 
                shoots[i].x -= shoots[i].speed * deltaTime;

                shoots[i].frameTime += deltaTime;
                if(shoots[i].frameTime >= 0.1f){
                    shoots[i].frame = (shoots[i].frame + 1) % SHOOT_FRAME_COUNT;
                    shoots[i].frameTime = 0.0f;
                }

                if (shoots[i].x < -50) shoots[i].active = 0;
                
                SDL_Rect currentRocketRect = { 
                    rocketX, 
                    rocketY - (int)(rocketFrameH * ROCKET_SCALE)/2, 
                    (int)(rocketFrameW * ROCKET_SCALE), 
                    (int)(rocketFrameH * ROCKET_SCALE) 
                };

                if(shoots[i].x - 30 < currentRocketRect.x + currentRocketRect.w &&
                   shoots[i].x + 30 > currentRocketRect.x &&
                   shoots[i].y > currentRocketRect.y &&
                   shoots[i].y < currentRocketRect.y + currentRocketRect.h)
                {
                    rocketHit = 1;
                    tremorTime = 0.5f;
                    shoots[i].active = 0;
                }
            }
        }

        if(rocketHit){
            int tremorOffset = (int)(20 * sinf(swayTime * 50));
            tremorTime -= deltaTime;
            if(tremorTime <= 0){
                rocketHit = 0;
            } else {
                 rocketDstRect.y += tremorOffset;
            }
        }

        // Renderização
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        for(int i=0;i<NUM_STARS;i++){
            filledCircleRGBA(ren, stars[i].x, stars[i].y, stars[i].r, 255,255,0,255);
        }

        for (int i = 0; i < MAX_SHOOTS; i++) {
            if (!shoots[i].active) continue;

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
            SDL_Rect shootSrc = { shoots[i].frame * shootFrameW, 0, shootFrameW, shootFrameH };
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

        SDL_Rect rocketSrc = { rocketFrame * rocketFrameW, 0, rocketFrameW, rocketFrameH };
        SDL_RenderCopy(ren, rocketTex, &rocketSrc, &rocketDstRect);

        SDL_RenderPresent(ren);
    }

    // Limpeza
    SDL_DestroyTexture(rocketTex);
    SDL_DestroyTexture(shootTex);
    SDL_DestroyTexture(failShootTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}