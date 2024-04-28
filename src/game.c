// Standard library
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// Third party libaries
#include <SDL2/SDL.h>

// Own code
#include "game.h"

// Find the distance between the current boid in the loop, and the closest one to it.
// If the closest boid is closer than the boid's bubble variable will allow, return true, else false.
bool determineAvoid(boid* currentBoid, boid** boidsList, const int* boidsAmount)
{
    boid* closestBoid = {0, 0, 0.0, 0.0, 0.0};
    float closestDistance = 0.0;
    for (int j = 0; j < *boidsAmount; j++)
    {
        boid* compareBoid = boidsList[j];
        int distance = sqrt((pow((currentBoid->x - compareBoid->x), 2) + pow((currentBoid->y - compareBoid->y), 2)));
        if ((distance < closestDistance) && distance)
        {
            closestDistance = distance;
        }
    }
    // If the closest boid is closer than the current boid's bubble will allow, make it avoid the flock.
    if (closestDistance > currentBoid->bubble)
    { return true; } else { return false; }
}

// Calculate the average position of all boids within the 
// selected boid's visual range, for use in orientating the birds (either towards to, or away from).
int averagePositionNearbyBoids(boid* currentBoid, boid** boidsList, int* boidsAmount)
{
    int averagePosition[2] = {0, 0};

    for (int i = 0; i < *boidsAmount; i++)
    {
        boid* compareBoid = boidsList[i];
        int dotProduct = ((currentBoid->x * currentBoid->y) + (compareBoid->x * compareBoid->y));
        float currentBoidMagnitude = sqrt(pow(currentBoid->x, 2) + pow(currentBoid->y, 2));
        float compareBoidMagnitude = sqrt(pow(compareBoid->x, 2) + pow(compareBoid->y, 2));
        float angleBetweenRaw = cacos((dotProduct) / (currentBoidMagnitude * compareBoidMagnitude));
        float angleBetween = (angleBetweenRaw * (180 / M_PI));

        if (currentBoid->view <= angleBetween <= 360)
        {
            averagePosition[0] = ((compareBoid->x + averagePosition[0]) / 2);
            averagePosition[1] = ((compareBoid->y + averagePosition[1]) / 2);
        } 
    }

    return averagePosition; 
}

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidsList, const int* boidsAmount)
{
    for (int i = 0; i < *boidsAmount; i++)
    {
        boid* currentBoid = boidsList[i];

        // Calculate based on nearby boids whether selected boid should avoid the flock or follow it.        
        currentBoid->avoid = determineAvoid(currentBoid, boidsList, boidsAmount);

        // Get average position of nearby boids to determine orientation of selected boid.
        int boidFocusPosition[2] = averagePositionNearbyBoids(currentBoid, boidsList, boidsAmount);
        currentBoid->directionX = boidFocusPosition[0];
        currentBoid->directionY = boidFocusPosition[1];
    }
}

// Using the data updated via calculate(), simulate the environment for one tick based on the specified rules.
void simulate(boid** boidsList, const int* boidsAmount)
{
    for (int i = 0; i < *boidsAmount; i++)
    {
        boid* currentBoid = boidsList[i];

    }
}