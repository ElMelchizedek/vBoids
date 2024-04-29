#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

void drawBoids(int** boidCoords, const int* boidsAmount, SDL_Window* window, SDL_Renderer* renderer); 
int graphicsInit(SDL_Window** selectWindow, SDL_Renderer** selectRenderer, const int* screenWidth, const int* screenHeight);

#endif 
// graphics.h
