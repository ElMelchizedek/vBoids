#ifndef ERROR_H
#define ERROR_H

// Error macros
#define E_MEM 0
#define E_GRAPHICS_INIT 1
#define E_SDL_VIDEO_INIT 2
#define E_SDL_WINDOW_CREATE 3
#define E_SDL_RENDERER_CREATE 4
#define E_REALLOC 5

int errorHandle(int error, ...);

#endif // ERROR_H