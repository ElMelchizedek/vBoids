#include <stdio.h>
#include <stdarg.h>

#include "error.h"

// Handles errors, obviously.
int errorHandle(int error, ...)
{
    va_list args;
    char *message;
    printf("ERROR: ");

    switch(error)
    {
        case (E_GRAPHICS_INIT):
            message = "Failed to initialise graphics for the programme.\n";
            break;
        case (E_SDL_VIDEO_INIT):
            message = "Failed to initialise SDL Video\nSDL Error: %s\n";
            break;
        case (E_SDL_WINDOW_CREATE):
            message = "SDL failed to create window\nSDL Error: %s\n";
            break;
        case (E_SDL_RENDERER_CREATE):
            message = "SDL failed to create renderer\nSDL Error: %s\n";
            break;
        case (E_MEM):
            message = "Failed to allocate memory for variable %s\n";
            break;
        case (E_REALLOC):
            message = "Failed to reallocate memory for variable: %s\n";
            break;
        case (E_NULL):
            message = "Failed to access element of array %s\n";
    }

    va_start(args, error);
    vprintf(message, args);
    va_end(args);
    return 0;
}
// error.c
