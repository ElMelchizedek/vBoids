#ifndef GAME_H
#define GAME_H

// CUSTOM TYPES
// Basic Entity: Use is required for any graphical or graphical-interacting variable to be manipulated in the game world.
// typedef struct entity
// {
//     void *thing;
//     int form;
//     int x;
//     int y;
//     int w;
//     int h;
// } entity;

// Boid: The simulated computer bird, rendered using SDL_RenderDrawPoint()
typedef struct boid
{
    int x;
    int y;
    int velocity;
    int directionX;
    int directionY;
    int bubble; // The distance which a boid requires between itself and other boids to prevent overcrowding.
    bool avoid; // If set to true, the boid will direct itself opposite of the flock's centre, rather than towards it.
    float view; // The boid's visible range, in degrees.
} boid;


// CUSTOM MACROS
// Form macros
#define F_NULL 0

// FUNCTIONS
void simulate(boid** boidsList, const int* boidsAmount);
void calculate(boid** boidsList, const int* boidsAmount);


#endif // GAME_H
