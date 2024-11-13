#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;
SDL_Event event;
extern unsigned char *MEMptr;

unsigned char character[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};

int initgui()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error starting gui\n");
        return -1;
    };

    window = SDL_CreateWindow("CHIP8 EMULATOR", 0, 0, 640, 320, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    return 0;
}

void clearscr()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

void drawchar(int addr, uint8_t x_coord, uint8_t y_coord, int nbytes)
{
    // creating display 10x the original
    int x = x_coord * 10;
    int y = y_coord * 10;
    uint8_t pix;
    for (int i = addr; i < (addr + nbytes); i++)
    {
        for (int k = 0; k < 10; k++)
        {
            pix = MEMptr[i]; // change to MEMptr
            while (pix != 0)
            {
                for (int j = 0; j < 10; j++)
                {
                    if (pix & 0x80)
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer, x + j, y + k);
                        // SDL_RenderPresent(renderer);
                    }
                }

                x += 10;
                pix = pix << 1;
            }
            // printf("%d",x);
            x = x_coord * 10;
        }
        y += 10;
    }

    SDL_RenderPresent(renderer);
    // printf("%d",y);
}

int getevent()
{
    if (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return 0;
        case SDL_KEYDOWN:
            clearscr();
        case SDL_TEXTINPUT:
            printf("%s", event.text.text);
        }
    }
    return -1;
}

// int main()
// {
//     initgui();
//     int run = 1;
//     int c = 50;

//     while (run)
//     {
//         while (SDL_PollEvent(&event) != 0)
//         {
//             switch (event.type)
//             {
//             case SDL_QUIT:
//                 run = 0;
//                 break;
//             case SDL_KEYDOWN:
//                 clearscr();
//             case SDL_TEXTINPUT:
//                 printf("%s",event.text.text);
//             default:
//                 break;
//             }
//         }
//         if (SDL_RenderDrawPoint(renderer, 60, 57) != 0)
//         {
//             printf("%s", SDL_GetError());
//         }
//         SDL_RenderPresent(renderer);
//         SDL_Delay(100);
//     };
//     return 0;
// }