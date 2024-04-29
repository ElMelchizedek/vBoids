#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// CUSTOM TYPES
// Boid: The simulated computer bird, rendered using SDL_RenderDrawPoint()
typedef struct boid
{
    int x;
    int y;
    int* velocity;
    int speed;
    int bubble; // The distance which a boid requires between itself and other boids to prevent overcrowding.
    bool avoid; // If set to true, the boid will direct itself opposite of the flock's centre, rather than towards it.
    float view; // The boid's visible range, in degrees.
} boid;


// CUSTOM MACROS
// Form macros
#define F_NULL 0

// FUNCTIONS
void simulate(boid** boidsList, const int* boidsAmount, const int* screenWidth, const int* screenHeight);
void calculate(boid** boidsList, const int* boidsAmount);
void initialiseBoidList(boid** boidsList, const int* boidsAmount, int* boidsCount, const int* screenWidth, const int* screenHeight);


#endif 
// game.h
