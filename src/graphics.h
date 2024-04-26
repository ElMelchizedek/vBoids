#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

int graphicsInit(SDL_Window* selectWindow, SDL_Renderer* selectRenderer, int* screenWidth, int* screenHeight);

#endif // GRAPHICS_H