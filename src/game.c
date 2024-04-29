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
    int closestDistance = 0;
    for (int j = 0; j < *boidsAmount; j++)
    {
        if (boidsList[j] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* compareBoid = boidsList[j];
        int distance = determineMagnitude((compareBoid->x - currentBoid->x), (compareBoid->y - currentBoid->y));
        if (distance < closestDistance) { closestDistance = distance; }
    }
    // If the closest boid is closer than the current boid's bubble will allow, make it avoid the flock.
    if (closestDistance > currentBoid->bubble)
    { return true; } else { return false; }
}

// Figures out what the angle to the averge position of the nearby boids within the selected boid's visual range is.
double determineAngleNearbyBoids(boid* currentBoid, boid** boidsList, const int* boidsAmount)
{
    double angle = 0.0;

    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList");};
        boid* compareBoid = boidsList[i];
        int pointVector[2] = {(compareBoid->x - currentBoid->x), (compareBoid->y - currentBoid->y)};
        int dotProduct = ((currentBoid->velocity[0] * pointVector[0]) + (currentBoid->velocity[1] * pointVector[1]));
        int velocityMagnitude = determineMagnitude(currentBoid->velocity[0], currentBoid->velocity[1]);
        int pointMagnitude = determineMagnitude(pointVector[0], pointVector[1]);
        double newAngle = acos(dotProduct / (velocityMagnitude * pointMagnitude));
        if (angle <= (currentBoid->view / 2) && angle >= ((currentBoid->view / 2) * -1))
        {
            angle = ((angle + newAngle) / 2); 
        }
    }
    return angle; 
}

int* determineVelocity(boid* currentBoid, double* angle)
{
    int* newVelocity = (int*)malloc(2 * sizeof(int));
    newVelocity[0] = (currentBoid->speed * cos(*angle));
    newVelocity[1] = (currentBoid->speed * sin(*angle));
    return newVelocity;
}

int determineSpeed(int currentSpeed, bool avoid)
{  
    int newSpeed = 0;
    if (avoid) { newSpeed = currentSpeed * -2; } else { newSpeed = currentSpeed * 2; }
    return newSpeed;
}

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidsList, const int* boidsAmount)
{ 
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* currentBoid = boidsList[i];
        // Calculate based on nearby boids whether selected boid should avoid the flock or follow it.        
        currentBoid->avoid = determineAvoid(currentBoid, boidsList, boidsAmount);
        // Get angle to nearby boids in visual range, for calculating new velocity.
        double angle = determineAngleNearbyBoids(currentBoid, boidsList, boidsAmount);
        // Calculate speed
        currentBoid->speed = determineSpeed(currentBoid->speed, currentBoid->avoid);
        // Calculate velocity 
        currentBoid->velocity = determineVelocity(currentBoid, &angle);
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
        currentBoid->x = (currentBoid->x + currentBoid->velocity[0]);
        currentBoid->y = (currentBoid->y + currentBoid->velocity[1]);
    }
}

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

        currentBoid->velocity[0] = (rand() % (10 - 1 + 1) + 1);
        currentBoid->velocity[1] = (rand() % (10 - 1 + 1) + 1);
        
        currentBoid->speed = 1;

        currentBoid->bubble = 10;
        currentBoid->avoid = false;
        currentBoid->view = 90;
    }
}
