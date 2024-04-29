#include <stdio.h>

#include <SDL2/SDL.h>

#include "error.h"

void drawBoids(int** boidCoords, const int* boidsAmount, SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    for (int i = 0; i < *boidsAmount; i++)
    {
        SDL_RenderDrawPoint(renderer, boidCoords[i][0], boidCoords[i][1]);
    }
    return;
}


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
