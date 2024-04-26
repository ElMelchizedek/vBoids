#include <stdio.h>

#include <SDL2/SDL.h>

#include "error.h"

int graphicsInit(SDL_Window* selectWindow, SDL_Renderer* selectRenderer, int* screenWidth, int* screenHeight)
{
    // Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        errorHandle(E_SDL_VIDEO_INIT, SDL_GetError());
        return 1;
    } else {
        // Create window
        selectWindow = SDL_CreateWindow("Verl's Boids",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        *screenWidth,
        *screenHeight,
        SDL_WINDOW_SHOWN);
        if (selectWindow == NULL)
        {
            errorHandle(E_SDL_WINDOW_CREATE, SDL_GetError());
            return 1;
        } else {
            // Create renderer
            selectRenderer = SDL_CreateRenderer(selectWindow, -1, SDL_RENDERER_ACCELERATED);
            if (selectRenderer == NULL)
            {
                errorHandle(E_SDL_RENDERER_CREATE, SDL_GetError());
                return 1;
            }
            else {
                // Initialise renderer colour
                SDL_SetRenderDrawColor(selectRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    return 0;
}