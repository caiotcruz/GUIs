// 1.2.2 - Faça um desenho qualquer usando a biblioteca SDL2_gfx, com figuras mais complexas.
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

int main(int argc, char* args[])
{
    /* INICIALIZAÇÃO */
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("1.2.2 - Foguete com astronauta",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);

    /* EXECUÇÃO */
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    // Estrelas
    for(int i=0; i<200; i++){
        filledCircleRGBA(ren, rand()%800, rand()%600, 1+rand()%2, 255, 255, 0, 255);
    }

    // Corpo do foguete (branco)
    boxRGBA(ren, 350, 200, 450, 450, 255, 255, 255, 255);

    // Ponta do foguete (triângulo vermelho)
    Sint16 vx[3] = {350, 450, 400};
    Sint16 vy[3] = {200, 200, 120};
    filledPolygonRGBA(ren, vx, vy, 3, 255, 0, 0, 255);

    // Asa esquerda (cinza)
    Sint16 vx2[3] = {350, 300, 350};
    Sint16 vy2[3] = {300, 450, 450};
    filledPolygonRGBA(ren, vx2, vy2, 3, 180, 180, 180, 255);

    // Asa direita (cinza)
    Sint16 vx3[3] = {450, 500, 450};
    Sint16 vy3[3] = {300, 450, 450};
    filledPolygonRGBA(ren, vx3, vy3, 3, 180, 180, 180, 255);

    // Chamas (amarelo + laranja)
    Sint16 vx4[3] = {360, 440, 400};
    Sint16 vy4[3] = {450, 450, 520};
    filledPolygonRGBA(ren, vx4, vy4, 3, 255, 140, 0, 255);
    filledCircleRGBA(ren, 400, 510, 15, 255, 255, 0, 255);

    // ======= JANELA =======
    filledCircleRGBA(ren, 400, 280, 40, 0, 100, 255, 255);  // vidro azul
    circleRGBA(ren, 400, 280, 40, 255, 255, 255, 255);      // borda branca

    // Cabeça do astronauta
    filledCircleRGBA(ren, 400, 270, 10, 255, 255, 255, 255);  // capacete branco
    ellipseRGBA(ren, 400, 270, 8, 5, 0, 0, 255, 255);         // visor azul
    
    // Corpo do astronauta
    boxRGBA(ren, 395, 280, 405, 300, 200, 200, 200, 255);     // corpo cinza claro
    
    // Braços do astronauta
    thickLineRGBA(ren, 395, 285, 380, 295, 2, 255, 255, 255, 255); // braço esq
    thickLineRGBA(ren, 405, 285, 420, 295, 2, 255, 255, 255, 255); // braço dir

    // Pernas do astronauta
    thickLineRGBA(ren, 397, 300, 390, 315, 2, 255, 255, 255, 255); // perna esq
    thickLineRGBA(ren, 403, 300, 410, 315, 2, 255, 255, 255, 255); // perna dir

    SDL_RenderPresent(ren);
    SDL_Delay(5000);

    /* FINALIZAÇÃO */
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
