#ifndef GAME_H
#define GAME_H

// CUSTOM TYPES
// Basic Entity: Use is required for any graphical or graphical-interacting variable to be manipulated in the game world.
typedef struct entity
{
    void *thing;
    int form;
    int x;
    int y;
    int w;
    int h;
} entity;


// CUSTOM MACROS
// Form macros
#define F_NULL 0


#endif // GAME_H
