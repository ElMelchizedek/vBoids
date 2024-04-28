#include <stdio.h>

#include <SDL2/SDL.h>

#include "error.h"

int graphicsInit(SDL_Window** selectWindow, SDL_Renderer** selectRenderer, const int* screenWidth, const int* screenHeight)
{
    // Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        errorHandle(E_SDL_VIDEO_INIT, SDL_GetError());
        return 1;
    } else {
        // Create window
        SDL_Window* newWindow = SDL_CreateWindow("Verl's Boids",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        *screenWidth,
        *screenHeight,
        SDL_WINDOW_SHOWN);
        if (newWindow == NULL)
        {
            errorHandle(E_SDL_WINDOW_CREATE, SDL_GetError());
            return 1;
        } else {
            // Create renderer
            SDL_Renderer* newRenderer = SDL_CreateRenderer(newWindow, -1, SDL_RENDERER_ACCELERATED);
            if (selectRenderer == NULL)
            {
                errorHandle(E_SDL_RENDERER_CREATE, SDL_GetError());
                return 1;
            }
           else {
                // Assign fresh windows + renderers to pointers to the originals back home.
                *selectWindow = newWindow;
                *selectRenderer = newRenderer;                

                // Initialise renderer colour
                SDL_SetRenderDrawColor(*selectRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    return 0;
}
// graphics.c
