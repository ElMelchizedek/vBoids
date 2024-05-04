#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// CUSTOM TYPES
// Boid: The simulated computer bird, rendered using SDL_RenderDrawPoint()
typedef struct boid
{
    int x;
    int y;
    int speed;
    int* velocity;
    int* acceleration;
    int bubble; // The distance which a boid requires between itself and other boids to prevent overcrowding.
    int view;
    double angle; 
} boid;

typedef struct boids
{
    boid** boidArray;
    int boidCount;
} boids;

// CUSTOM MACROS
// Form macros
#define F_NULL 0

// FUNCTIONS
void simulate(boid** boidList, const int* boidsAmount, const int* screenWidth, const int* screenHeight);
void calculate(boid** boidList, const int* boidsAmount, const int* screenWidth, const int* screenHeight, int lastSeconds);
void initialiseBoidList(boid** boidList, const int* boidsAmount, int* boidsCount, const int* screenWidth, const int* screenHeight);


#endif 
// game.h
