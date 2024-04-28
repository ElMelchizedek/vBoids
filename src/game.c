// Standard library
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <complex.h>
#include <time.h>

// Third party libaries
#include <SDL2/SDL.h>

// Own code
#include "game.h"
#include "error.h"

int determineMagnitude(int x, int y)
{
    return sqrt(pow(x, 2) + pow(y, 2));
}

// Find the distance between the current boid in the loop, and the closest one to it.
// If the closest boid is closer than the boid's bubble variable will allow, return true, else false.
bool determineAvoid(boid* currentBoid, boid** boidsList, const int* boidsAmount)
{
    //boid closestBoid = {0, 0, 0, 0, 0, 0, 0, false, 0.0};
    float closestDistance = 0.0;
    for (int j = 0; j < *boidsAmount; j++)
    {
        if (boidsList[j] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* compareBoid = boidsList[j];
        int distance = determineMagnitude((currentBoid->x - compareBoid->x), (currentBoid->y - compareBoid->y));        
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
int* averagePositionNearbyBoids(boid* currentBoid, boid** boidsList, const int* boidsAmount)
{
    int* averagePosition = (int*)malloc(2 * sizeof(int)); 

    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList");};
        boid* compareBoid = boidsList[i];
        int dotProduct = ((currentBoid->x * compareBoid->x) + (currentBoid->y * compareBoid->y));
        int currentBoidMagnitude = determineMagnitude(currentBoid->x, currentBoid->y);
        int compareBoidMagnitude = determineMagnitude(compareBoid->x, compareBoid->y);
        float angleBetweenRaw = cacos((dotProduct) / (currentBoidMagnitude * compareBoidMagnitude));
        float angleBetween = (angleBetweenRaw * (180 / M_PI));

        if (currentBoid->view <= angleBetween && angleBetween <= 360)
        {
            averagePosition[0] = ((compareBoid->x + averagePosition[0]) / 2);
            averagePosition[1] = ((compareBoid->y + averagePosition[1]) / 2);
        } 
    }

    return averagePosition; 
}

// Uses special vector maths provided by Ada that first generates a vector whose magnitude is in comparison to the average position of the visible nearby boids,
// affecting the specified boid's velocity down the line.
int determineAcceleration(boid* currentBoid, int* averagePosition, bool avoid)
{
    int vectorBetween[2] = {(averagePosition[0] - currentBoid->x), (averagePosition[1] - currentBoid->y)};
    int vectorBetweenLength = determineMagnitude(vectorBetween[0], vectorBetween[1]);
    int unitVector[2] = {(vectorBetween[0] / vectorBetweenLength), (vectorBetween[1] / vectorBetweenLength)};
    int accelerateVector[2] = {(unitVector[0] / vectorBetweenLength), (unitVector[1] / vectorBetweenLength)};
    int accelerateVectorMagnitude = determineMagnitude(accelerateVector[0], accelerateVector[1]);
    
    if (avoid)
    {
        accelerateVectorMagnitude = (-accelerateVectorMagnitude);
    }
    return accelerateVectorMagnitude;
}

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidsList, const int* boidsAmount)
{ for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* currentBoid = boidsList[i];

        // Calculate based on nearby boids whether selected boid should avoid the flock or follow it.        
        currentBoid->avoid = determineAvoid(currentBoid, boidsList, boidsAmount);

        // Get average position of nearby boids to determine orientation of selected boid.
        int* boidFocusPosition = (int*)malloc(2 * sizeof(int));
        boidFocusPosition = averagePositionNearbyBoids(currentBoid, boidsList, boidsAmount);
        currentBoid->directionX = boidFocusPosition[0];
        currentBoid->directionY = boidFocusPosition[1];

        // Determine the acceleration using the ascertained direction vectors.
        currentBoid->acceleration = determineAcceleration(currentBoid, boidFocusPosition, currentBoid->avoid);
        // Then calculate velocity based on the acceleration, basic physics.
        currentBoid->velocity = (currentBoid->velocity * currentBoid->acceleration);
    }

    return;
}

// Using the data updated via calculate(), simulate the environment for one tick based on the specified rules.
void simulate(boid** boidsList, const int* boidsAmount)
{
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* currentBoid = boidsList[i];
        currentBoid->x = (currentBoid->directionX * currentBoid->velocity);
        currentBoid->y = (currentBoid->directionY * currentBoid->velocity);
    }
}
// game.c

void initialiseBoidList(boid** boidList, const int* boidsAmount, const int* screenWidth, const int* screenHeight)
{
    int generateXUpperLimit = ((*screenWidth / 2) + (*screenWidth / 10));
    int generateYUpperLimit = ((*screenHeight / 2) + (*screenHeight / 10));
    int generateXLowerLimit = ((*screenWidth / 2) - (*screenWidth / 10));
    int generateYLowerLimit = ((*screenHeight / 2) - (*screenHeight / 10));

    for (int i = 0; i < *boidsAmount; i++) 
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* currentBoid = boidList[i];
        srand(time(0));

        currentBoid->x = (rand() % (generateXUpperLimit - generateXLowerLimit + 1) + generateXLowerLimit);
        currentBoid->y = (rand() % (generateYUpperLimit - generateYLowerLimit + 1) + generateYLowerLimit);

        currentBoid->velocity = (rand() % (10 - 0 + 1) + 0);
        currentBoid->directionX = (rand() % (20 - 10 + 1) + 10);
        currentBoid->directionY = (rand() % (20 - 10 + 1) + 10);
        currentBoid->acceleration = 10;
        currentBoid->bubble = 100;
        currentBoid->avoid = false;
        currentBoid->view = 300;
    }
}
