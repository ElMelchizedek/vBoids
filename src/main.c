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
    if (graphicsInit(&window, &renderer, &SCREEN_WIDTH, &SCREEN_HEIGHT))
    {
        errorHandle(E_GRAPHICS_INIT);
    }

    // Address list: Array of pointers to every variable with dynamically allocated memory,
    // so their memory can be freed on quit in an orderly manner.
    void** addressList = (void**)malloc(sizeof(void*));
    if (addressList == NULL)
    {
        errorHandle(E_MEM, "addressList");
    }
    int addressListCount = 0;

    // Entity list: List of every graphically interacting entity, used for rendering.
    boid** boidList = (boid**)malloc(BOIDS_AMOUNT * sizeof(boid*));
    if (boidList == NULL)
    {
        errorHandle(E_MEM, "boidList");
    }

    // Initialise the boid list with dummy values to prevent memory issues.
    for (int i = 0; i < BOIDS_AMOUNT; i++)
    {
        boidList[i]                     = malloc(sizeof(boid));
        boidList[i]->x                  = 0;
        boidList[i]->y                  = 0;
        boidList[i]->velocity           = (int*)malloc(2 * sizeof(int));
        boidList[i]->speed              = 0;
        boidList[i]->bubble             = 0;
        boidList[i]->avoid              = false;
        boidList[i]->view               = 0.0;
    }
    initialiseBoidList(boidList, &BOIDS_AMOUNT, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    saveAddress(&addressList, &addressListCount, (void*)boidList);

    // Variables required for simulation, that will be passed to functions in game.c/graphics.c

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
        }
        // Make screen white, clearing.
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        // Simulate for a tick
        calculate(boidList, &BOIDS_AMOUNT);
        simulate(boidList, &BOIDS_AMOUNT);
        // Create this tick's boidChoord
        int** boidCoords = (int**)malloc(BOIDS_AMOUNT * sizeof(int*)); 
        for (int i = 0; i < BOIDS_AMOUNT; i++)
        {
            boidCoords[i] = (int*)malloc(2 * sizeof(int));
        }
        // Collect the coordinates of each boid int boidChoords and then send them to drawBoids()
        for (int i = 0; i < BOIDS_AMOUNT; i++)
        {
            boidCoords[i][0] = boidList[i]->x;
            boidCoords[i][1] = boidList[i]->y;   
        }
        // Draw the boids
        drawBoids(boidCoords, &BOIDS_AMOUNT, window, renderer);
        SDL_RenderPresent(renderer);
        // Delay to prevent GPU overlod
        // NOTE: Replace this with proper implementation that Ada told me to do
        SDL_Delay(1000);
    }
    // Exit programme since quit has been activated
    end(window, renderer);
    return 0;
}
// main.c
