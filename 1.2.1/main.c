// 1.2.1 - Faça um desenho qualquer com linhas, pontos e retângulos, usando cores diversas.

#include <SDL2/SDL.h>

int main (int argc, char* args[])
{
    /* INICIALIZACAO */
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("1.2.1 - Estrelas",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         1280, 640, SDL_WINDOW_SHOWN
                      );
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);

    /* EXECUÇÃO */
    SDL_SetRenderDrawColor(ren, 0,0,0,0x00);
    SDL_RenderClear(ren);
    // linha 1 - azul
    SDL_SetRenderDrawColor(ren, 0, 0, 255, 0xFF);
    SDL_RenderDrawLine(ren, 640, 60, 540, 580);

    // linha 2 - vermelha
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 0xFF);
    SDL_RenderDrawLine(ren, 540, 580, 840, 310);

    // linha 3 - verde
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 0xFF);
    SDL_RenderDrawLine(ren, 840, 310, 440, 310);

    // linha 4 - amarela
    SDL_SetRenderDrawColor(ren, 255, 255, 0, 0xFF);
    SDL_RenderDrawLine(ren, 440, 310, 740, 580);

    // linha 5 - magenta
    SDL_SetRenderDrawColor(ren, 255, 0, 255, 0xFF);
    SDL_RenderDrawLine(ren, 740, 580, 640, 60);

    // Retangulo no meio da estrela - cinza
    SDL_SetRenderDrawColor(ren, 200, 200, 200, 0xFF);
    SDL_Rect r = { 600, 335, 80, 50 };
    SDL_RenderDrawRect(ren, &r);

    // Letra C
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 0xFF);
    SDL_RenderDrawLine(ren, 605, 340, 605, 375); 
    SDL_RenderDrawLine(ren, 605, 340, 620, 340); 
    SDL_RenderDrawLine(ren, 605, 375, 620, 375);

    // Letra A
    SDL_RenderDrawLine(ren, 625, 375, 635, 340);
    SDL_RenderDrawLine(ren, 635, 340, 645, 375);
    SDL_RenderDrawLine(ren, 628, 358, 642, 358);

    // Letra I
    SDL_RenderDrawLine(ren, 650, 340, 650, 375); 

    // Letra O
    SDL_RenderDrawLine(ren, 655, 340, 655, 375);
    SDL_RenderDrawLine(ren, 655, 340, 670, 340);
    SDL_RenderDrawLine(ren, 670, 340, 670, 375);
    SDL_RenderDrawLine(ren, 655, 375, 670, 375);

    //Estrelas de fundo - Amarelas
    SDL_SetRenderDrawColor(ren, 255, 255, 0, 0xFF);
    SDL_RenderFillRect(ren, &(SDL_Rect){100, 50, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){200, 150, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){300, 100, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){400, 200, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){500, 50, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){600, 100, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){700, 150, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){800, 50, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){900, 200, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){1000, 100, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){100, 250, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){200, 350, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){300, 300, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){400, 400, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){500, 250, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){800, 250, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){900, 400, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){1000, 300, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){100, 450, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){200, 550, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){300, 500, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){400, 600, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){500, 550, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){700, 650, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){800, 450, 3, 3});
    SDL_RenderFillRect(ren, &(SDL_Rect){900, 600, 2, 2});
    SDL_RenderFillRect(ren, &(SDL_Rect){1000, 500, 3, 3});
    

    SDL_RenderPresent(ren);
    SDL_Delay(5000);

    /* FINALIZACAO */
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
