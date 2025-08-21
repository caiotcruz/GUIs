// 1.2.2 - Faça um desenho qualquer usando a biblioteca SDL2_gfx, com figuras mais complexas. 
//Estrela regular de 5 pontas girando e pulsando. Cada ponta é a junção da cor de suas arestas. O centro é a junção das cores das pontas.
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.14159265

int main(int argc, char* args[])
{
    /* INICIALIZACAO */
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("Estrela Colorida Animada",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       1280, 640,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));

    int running = 1;
    SDL_Event e;
    double angle = 0.0;
    double pulse = 1.0;
    int pulseDir = 1;

    // Centro da estrela
    int cx = 640;
    int cy = 310;

    // Raios da estrela
    double R_outer = 150;
    double R_inner = 60;

    // Cores das linhas
    SDL_Color colors[5] = {
        {0, 0, 255, 255},    // azul
        {255, 0, 0, 255},    // vermelho
        {0, 255, 0, 255},    // verde
        {255, 255, 0, 255},  // amarelo
        {255, 0, 255, 255}   // magenta
    };

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }

        // Fundo preto
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Estrelas de fundo
        for (int i = 0; i < 30; i++) {
            int x = rand() % 1280;
            int y = rand() % 640;
            int r = rand() % 3 + 1;
            filledCircleRGBA(ren, x, y, r, 255, 255, 0, 255);
        }

        // Calcula vértices externos e internos
        double outerX[5], outerY[5];
        double innerX[5], innerY[5];

        for (int i = 0; i < 5; i++) {
            double theta_outer = i * 2.0 * PI / 5.0 - PI / 2;
            double theta_inner = theta_outer + PI / 5.0;

            outerX[i] = cx + cos(theta_outer + angle) * R_outer * pulse;
            outerY[i] = cy + sin(theta_outer + angle) * R_outer * pulse;

            innerX[i] = cx + cos(theta_inner + angle) * R_inner * pulse;
            innerY[i] = cy + sin(theta_inner + angle) * R_inner * pulse;
        }

        // Pentágono central - Junção das cores das pontas.
        int rx = 0, gx = 0, bx = 0;
        for (int i = 0; i < 5; i++) {
            rx += colors[i].r;
            gx += colors[i].g;
            bx += colors[i].b;
        }
        rx /= 5; gx /= 5; bx /= 5;

        Sint16 px[5], py[5];
        for (int i = 0; i < 5; i++) {
            px[i] = (Sint16)innerX[i];
            py[i] = (Sint16)innerY[i];
        }
        filledPolygonRGBA(ren, px, py, 5, rx, gx, bx, 255);

        // Preenche as 5 pontas da estrela
        for (int i = 0; i < 5; i++) {
            Sint16 tx[3], ty[3];

            tx[0] = (Sint16)outerX[i];
            ty[0] = (Sint16)outerY[i];
            tx[1] = (Sint16)innerX[i];
            ty[1] = (Sint16)innerY[i];
            tx[2] = (Sint16)innerX[(i+4)%5];
            ty[2] = (Sint16)innerY[(i+4)%5];

            SDL_Color c1 = colors[i];
            SDL_Color c2 = colors[(i+4)%5];
            int r = (c1.r + c2.r) / 2;
            int g = (c1.g + c2.g) / 2;
            int b = (c1.b + c2.b) / 2;

            filledPolygonRGBA(ren, tx, ty, 3, r, g, b, 255);
        }

        // Desenha as linhas da estrela
        for (int i = 0; i < 5; i++) {
            int next = (i + 2) % 5;
            aalineRGBA(ren, (int)outerX[i], (int)outerY[i],
                       (int)outerX[next], (int)outerY[next],
                       colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(50);

        // Atualiza ângulo e pulso
        angle += 0.05;
        pulse += 0.01 * pulseDir;
        if (pulse > 1.2 || pulse < 0.8) pulseDir = -pulseDir;
    }

    /* FINALIZACAO */
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
