// Standard library
#include <stdio.h>
#include <stdbool.h>

// Third party libraries
#include <SDL2/SDL.h>

// Own code
#include "graphics.h"
#include "error.h"
#include "game.h"

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BOIDS_AMOUNT = 100;

// Quit, exit programme
void end(SDL_Window* selectWindow, SDL_Renderer* selectRenderer)
{
    // Deallocate all of the basic stuff that SDL has specific functions for.
    SDL_DestroyRenderer(selectRenderer);
    selectRenderer = NULL;
    SDL_DestroyWindow(selectWindow);
    selectWindow = NULL;
    SDL_Quit();
}

// Saves address of variable to addressList
void saveAddress(void*** addressList, int* addressCount, void* selectThing)
{
    *addressCount = (*addressCount + 1);
    *addressList = (void**)realloc(*addressList, *addressCount * sizeof(void*));
    if (addressList == NULL)
    {
        errorHandle(E_REALLOC, "addressList");
    }
    (*addressList)[*addressCount - 1] = selectThing;
    return;
}

int main(int argc, char* argv[])
{
    // Base variables required for graphics.
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Other basic variables
    bool quit = false;

    // Initialise graphics
    if (graphicsInit(window, renderer, &SCREEN_WIDTH, &SCREEN_HEIGHT))
    {
        errorHandle(E_GRAPHICS_INIT);
    }

    // Address list: Array of pointers to every variable with dynamically allocated memory,
    // so their memory can be freed on quit in an orderly manner.
    void **addressList = (void**)malloc(sizeof(void*));
    if (addressList == NULL)
    {
        errorHandle(E_MEM, "addressList");
    }
    int addressListCount = 0;

    // Entity list: List of every graphically interacting entity, used for rendering.
    entity** entityList = (entity**)malloc(BOIDS_AMOUNT * sizeof(entity*));
    if (entityList == NULL)
    {
        errorHandle(E_MEM, "entityList");
    }

    // Initialise the entity list with dummy values to prevent memory issues.
    for (int i = 0; i < BOIDS_AMOUNT; i++)
    {
        entityList[i] = malloc(sizeof(entity));
        entityList[i]->thing = NULL;
        entityList[i]->form = F_NULL;
        entityList[i]->x = 0;
        entityList[i]->y = 0;
        entityList[i]->w = 0;
        entityList[i]->h = 0;
    }
    saveAddress(&addressList, &addressListCount, (void*)entityList);

    // Event to be handled by loop
    SDL_Event e;

    // Main loop
    while(!quit)
    {
        // Event handler loop
        while (SDL_PollEvent(&e) != 0)
        {
            // Free memory on quit
            if (e.type == SDL_QUIT)
            {
                for (int i = 0; i <= addressListCount; i++)
                {
                    free(addressList[i]);
                }
                quit = true;
            }

            // Make screen white, clearing.
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            // Display the selected visuals for this tick.
            SDL_RenderPresent(renderer);
            // Delay to prevent GPU Overlod
            // NOTE: Replace this with proper implementation that Ada told me to do
            SDL_Delay(17);
        }
    }

    // Exit programme since quit has been activated
    end(window, renderer);
    return 0;
}