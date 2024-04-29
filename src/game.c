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

void initialiseBoidList(boid** boidList, const int* boidsAmount, int* boidsCount, const int* screenWidth, const int* screenHeight)
{
    int generateXUpperLimit = ((*screenWidth / 2) + (*screenWidth / 10));
    int generateYUpperLimit = ((*screenHeight / 2) + (*screenHeight / 10));
    int generateXLowerLimit = ((*screenWidth / 2) - (*screenWidth / 10));
    int generateYLowerLimit = ((*screenHeight / 2) - (*screenHeight / 10));

    for (int i = 0; i < *boidsAmount; i++) 
    {
        boid* newBoid = (boid*)malloc(sizeof(boid));
        srand(time(0));

        newBoid->x = (rand() % (generateXUpperLimit - generateXLowerLimit + 1) + generateXLowerLimit);
        newBoid->y = (rand() % (generateYUpperLimit - generateYLowerLimit + 1) + generateYLowerLimit);

        newBoid->velocity = (int*)malloc(2 * sizeof(int));
        newBoid->velocity[0] = (rand() % (10 - 1 + 1) + 1);
        newBoid->velocity[1] = (rand() % (10 - 1 + 1) + 1);
        
        newBoid->speed = 1;

        newBoid->bubble = 10;
        newBoid->avoid = false;
        newBoid->view = 90;

        boidList[*boidsCount] = newBoid;
        *boidsCount = *boidsCount + 1;
    }
    return;
}

int determineMagnitude(int x, int y)
{
    return sqrt(pow(x, 2) + pow(y, 2));
}

// Find the distance between the current boid in the loop, and the closest one to it.
// If the closest boid is closer than the boid's bubble variable will allow, return true, else false.
bool determineAvoid(boid* currentBoid, boid** boidsList, const int* boidsAmount, int currentBoidIndex)
{
    int closestDistance = 0;
    for (int j = 0; j < *boidsAmount; j++)
    {
        if (boidsList[j] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* compareBoid = boidsList[j];
        if (j != currentBoidIndex)
        {
            int distance = determineMagnitude((compareBoid->x - currentBoid->x), (compareBoid->y - currentBoid->y));
            if (distance < closestDistance) { closestDistance = distance; }
        }
    }
    // If the closest boid is closer than the current boid's bubble will allow, make it avoid the flock.
    if (closestDistance > currentBoid->bubble)
    { return true; } else { return false; }
}

// Figures out what the angle to the averge position of the nearby boids within the selected boid's visual range is.
double determineAngleNearbyBoids(boid* currentBoid, boid** boidsList, const int* boidsAmount, int currentBoidIndex)
{
    double angle = 0.0;

    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList");};
        boid* compareBoid = boidsList[i];
        if (i != currentBoidIndex)
        {
            double newAngle = 0.0;
            double pointVector[2] = {(compareBoid->x - currentBoid->x), (compareBoid->y - currentBoid->y)};
            double dotProduct = ((currentBoid->velocity[0] * pointVector[0]) + (currentBoid->velocity[1] * pointVector[1]));
            double velocityMagnitude = determineMagnitude(currentBoid->velocity[0], currentBoid->velocity[1]);
            double pointMagnitude = determineMagnitude(pointVector[0], pointVector[1]);
            double acosData = (double)(dotProduct / (velocityMagnitude * pointMagnitude));
            newAngle = acos(acosData);
            if (angle <= (currentBoid->view / 2) && angle >= ((currentBoid->view / 2) * -1))
            {
                angle = ((angle + newAngle) / 2); 
            }
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
        int currentBoidIndex = i;
        double angle = 0.0;
        // Calculate based on nearby boids whether selected boid should avoid the flock or follow it.        
        boidsList[i]->avoid = determineAvoid(boidsList[i], boidsList, boidsAmount, currentBoidIndex);
        // Get angle to nearby boids in visual range, for calculating new velocity.
        angle = determineAngleNearbyBoids(boidsList[i], boidsList, boidsAmount, currentBoidIndex);
        // Calculate speed
        boidsList[i]->speed = determineSpeed(boidsList[i]->speed, boidsList[i]->avoid);
        // Calculate velocity 
        boidsList[i]->velocity = determineVelocity(boidsList[i], &angle);
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
