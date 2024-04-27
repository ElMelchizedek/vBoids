// Standard library
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// Third party libaries
#include <SDL2/SDL.h>

// Own code
#include "game.h"

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidsList, const int* boidsAmount, int* flockCentre)
{
    int boidPositionMean[2] = {0, 0};

    for (int i =0; i < *boidsAmount; i++)
    {
        boid* currentBoid = boidsList[i];
        
        // Find the distance between the current boid in the loop, and the closest one to it.
        boid* closestBoid = {0, 0, 0.0, 0.0, 0.0};
        float closestDistance = 0.0;
        for (int j = 0; j < *boidsAmount; j++)
        {
            boid* compareBoid = boidsList[i];
            float distance = sqrt((pow((currentBoid->x - compareBoid->x), 2) + pow((currentBoid->y - compareBoid->y), 2)));
            if ((distance < closestDistance) && distance)
            {
                closestDistance = distance;
            }
        }
        // If the closest boid is closer than the current boid's bubble will allow, make it avoid the flock.
        if (closestDistance > currentBoid->bubble)
        {
            currentBoid->avoid = true;
        }

        // Calculate the flock's central position, for use in orientating the birds (either towards to, or away from).
        boidPositionMean[0] = ((boidPositionMean[0] + currentBoid->x) / 2);
        boidPositionMean[1] = ((boidPositionMean[1] + currentBoid->y) / 2);
    }
}

void simulate(boid** boidsList, const int* boidsAmount)
{
    for (int i = 0; i < *boidsAmount; i++)
    {
        boid* currentBoid = boidsList[i];

    }
}