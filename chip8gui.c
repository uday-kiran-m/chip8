#include <SDL2/SDL.h>
#include <stdio.h>
#define XCORDS 66
#define YCORDS 32
SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;
SDL_Event event;

extern unsigned char *MEMptr;
extern unsigned int inputchar;

unsigned char character[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};
char video[32 * 8];
unsigned char vidbuf = 0;

int initgui()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error starting gui\n");
        return -1;
    };

    window = SDL_CreateWindow("CHIP8 EMULATOR", 0, 0, XCORDS * 10, YCORDS * 10, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 214, 0, SDL_ALPHA_OPAQUE);
    return 0;
}

void clearSDLscr()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 130, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 214, 0, SDL_ALPHA_OPAQUE);
}
void clearscr()
{
    memset(video, 0, sizeof video);
    clearSDLscr();
}

int drawchar(int addr, uint8_t x_coord, uint8_t y_coord, int nbytes)
{
    // planning to use char, disp=>     char1
    //                                  char2
    //                                  char3
    //                                     :
    //                                  N bytes
    // total 8 chars width*32 rows
    // use 0s to ofset and allign them to above chars so easier to XOR
    // x= 12, bytes = 1101 =>0000 0000 0011 0100

    int vf = 0;
    int shift = (x_coord % 8);
    vidbuf = 0; // clearing buf
    for (int i = 0; i < nbytes; i++)
    {
        unsigned char data = 0;
        vidbuf = 0;
        data = (MEMptr[addr + i] >> shift);
        if (shift)
        {
            vidbuf = MEMptr[addr + i] << (8 - shift);
        }
        if (video[8 * (y_coord + i) + (x_coord / 8)] & data)
        {
            vf = 1; // turning of pix in XOR mode
        }
        if (video[8 * (y_coord + i) + (x_coord / 8) + 1] & vidbuf)
        {
            vf = 1; // turning of pix in XOR mode
        }
        video[8 * (y_coord + i) + (x_coord / 8)] = video[8 * (y_coord + i) + (x_coord / 8)] ^ data;
        video[8 * (y_coord + i) + (x_coord / 8) + 1] = video[8 * (y_coord + i) + (x_coord / 8) + 1] ^ vidbuf;
    }

    vidbuf = 0;
    clearSDLscr();
    for (int ele = 0; ele < 32 * 8; ele++)
    {

        if (video[ele])
        {
            int x = ((ele % 8) * 8) * 10;
            int y = (ele / 8) * 10;
            unsigned char pix = video[ele];

            while (pix)
            {

                if (pix & 128)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        for (int k = 0; k < 10; k++)
                        {
                            SDL_SetRenderDrawColor(renderer, 255, 214, 0, SDL_ALPHA_OPAQUE);
                            SDL_RenderDrawPoint(renderer, x + k, y + j);
                            // SDL_RenderPresent(renderer);
                        }
                    }
                }
                pix = pix << 1;
                x += 10;
            }
        }
    }
    SDL_RenderPresent(renderer);

    return vf;
}

void drawcharold(int addr, uint8_t x_coord, uint8_t y_coord, int nbytes)
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
                        SDL_SetRenderDrawColor(renderer, 255, 214, 0, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer, x + j, y + k);
                        // SDL_RenderPresent(renderer);
                    }
                }

                x += 10;
                pix = pix << 1;
            }
            x = x_coord * 10;
        }
        y += 10;
    }
    SDL_RenderPresent(renderer);
}

int getevent()
{
    if (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return 0;
        case SDL_KEYUP:
            inputchar = 0xf+1;
            break;
        
        case SDL_KEYDOWN:
            // 0-9
            switch(event.key.keysym.sym){
                case SDLK_1:
                inputchar = 0x1;
                break;
                case SDLK_2:
                inputchar = 0x2;
                break;
                case SDLK_3:
                inputchar = 0x3;
                break;
                case SDLK_4:
                inputchar = 0xc;
                break;
                case SDLK_q:
                inputchar = 0x4;
                break;
                case SDLK_w:
                inputchar = 0x5;
                break;
                case SDLK_e:
                inputchar = 0x6;
                break;
                case SDLK_r:
                inputchar = 0xd;
                break;
                case SDLK_a:
                inputchar = 0x7;
                break;
                case SDLK_s:
                inputchar = 0x8;
                break;
                case SDLK_d:
                inputchar = 0x9;
                break;
                case SDLK_f:
                inputchar = 0xe;
                break;
                case SDLK_z:
                inputchar = 0xa;
                break;
                case SDLK_x:
                inputchar = 0x0;
                break;
                case SDLK_c:
                inputchar = 0xb;
                break;
                case SDLK_v:
                inputchar = 0xf;
                break;
            }
        
            break;
        
    }
    }
    return -1;
}
