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

double determineAngleBetweenVectors(boid* selectBoid, double* pointVector, double* pointMagnitude)
{
    double dotProduct = ((selectBoid->velocity[0] * pointVector[0]) + (selectBoid->velocity[1] * pointVector[1]));
    double velocityMagnitude = determineMagnitude(selectBoid->velocity[0], selectBoid->velocity[1]);
    double acosData = (double)(dotProduct / (velocityMagnitude * *pointMagnitude));
    return acos(acosData);
}

void initialiseBoidList(boid** boidList, const int* boidsAmount, int* boidsCount, const int* screenWidth, const int* screenHeight)
{
    int generateXUpperLimit = ((*screenWidth / 2) + (*screenWidth / 10));
    int generateYUpperLimit = ((*screenHeight / 2) + (*screenHeight / 10));
    int generateXLowerLimit = ((*screenWidth / 2) - (*screenWidth / 10));
    int generateYLowerLimit = ((*screenHeight / 2) - (*screenHeight / 10));
    srand(time(0));

    for (int i = 0; i < *boidsAmount; i++) 
    {
        boid* newBoid = (boid*)malloc(sizeof(boid));

        newBoid->x = (rand() % (generateXUpperLimit - generateXLowerLimit + 1) + generateXLowerLimit);
        newBoid->y = (rand() % (generateYUpperLimit - generateYLowerLimit + 1) + generateYLowerLimit);

        newBoid->velocity = (int*)malloc(2 * sizeof(int));
        newBoid->velocity[0] = 0;
        newBoid->velocity[1] = 0;        
        newBoid->speed = 3;

        newBoid->bubble = determineMagnitude(10, 10);
        newBoid->view = 90 * (M_PI / 180); 

        boidList[*boidsCount] = newBoid;
        *boidsCount = *boidsCount + 1;
    }
    return;
}

// Figures out what the angle to the averge position of the nearby boids within the selected boid's visual range is.
double determineAngleNearbyBoids(boid* currentBoid, boid** boidList, const int* boidsAmount, int currentBoidIndex, const int* screenWidth,
                                    const int* screenHeight)
{
    double angle = 0.0;
    int* nearbyBoidCentreCoords = (int*)malloc(2 * sizeof(int));
    nearbyBoidCentreCoords[0] = 0;
    nearbyBoidCentreCoords[1] = 0;

    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidList");};
        boid* compareBoid = boidList[i];
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
                nearbyBoidCentreCoords[0] = ((nearbyBoidCentreCoords[0] + compareBoid->x) / 2);
                nearbyBoidCentreCoords[1] = ((nearbyBoidCentreCoords[1] + compareBoid->y) / 2);
                angle = ((angle + newAngle) / 2); 
            }
        }
    }
    return angle; 
}

// Get angle opposite of average vector towards boids that are inside bubble range.
double seperation(boid* selectBoid, boid** boidList, const int* boidsAmount, int selectBoidIndex)
{
    double deltaAngle = 0.0;

    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidLists"); }
        boid* compareBoid = boidList[i];
        if (i != selectBoidIndex)
        {
            double additiveAngle = 0.0;
            double pointVector[2] = {(compareBoid->x - selectBoid->x), (compareBoid->y - selectBoid->y)};
            double pointMagnitude = determineMagnitude(pointVector[0], pointVector[1]);
            if (pointMagnitude < selectBoid->bubble) { 
                double dotProduct = ((selectBoid->velocity[0] * pointVector[0]) + (selectBoid->velocity[1] * pointVector[1]));
                double velocityMagnitude = determineMagnitude(selectBoid->velocity[0], selectBoid->velocity[1]);
                double acosData = (double)(dotProduct / (velocityMagnitude * pointMagnitude));
                return acos(acosData);
            }
            deltaAngle = (deltaAngle + additiveAngle);
        }
    }
    // Get the angle on the opposite side of the unit circle, since we're trying to get selectBoid to avoid the boids intruding on its personal space.
    if (0 < deltaAngle && deltaAngle < M_PI) { deltaAngle = (deltaAngle + M_PI); }
    else if ((M_PI * -1) < deltaAngle && deltaAngle < 0){ deltaAngle = (deltaAngle - M_PI); }
    return deltaAngle;
}

// Get angle between the velocity of the selectBoid and the average velocity of the boids within its visual range.
double alignment(boid* selectBoid, boid** boidList, const int* boidsAmount, int selectBoidIndex)
{
    double deltaAngle = 0.0;

    for (int i =0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidLists"); }
        boid* compareBoid = boidList[i];
        if (i != selectBoidIndex)
        {
            double additiveAngle = 0.0;
            double pointVector[2] = {(compareBoid->velocity[0] - selectBoid->velocity[0]), (compareBoid->velocity[1] - selectBoid->velocity[1])};
            double pointMagnitude = determineMagnitude(pointVector[0], pointVector[1]);
            if (pointMagnitude < selectBoid->view) { additiveAngle = determineAngleBetweenVectors(selectBoid, &pointVector, &pointMagnitude); }
            deltaAngle = (deltaAngle + additiveAngle);
        }
    }
    return deltaAngle;
}

// Get angle towards the average position of nearby boids.
double cohesion(boid* selectBoid, boid** boidList, const int* boidsAmount, int selectBoidIndex)
{
    double deltaAngle = 0.0;

    for (int i =0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidLists"); }
        boid* compareBoid = boidList[i];
        if (i != selectBoidIndex)
        {
            double additiveAngle = 0.0;
            double pointVector[2] = {(compareBoid->x - selectBoid->x), (compareBoid->y - selectBoid->y)};
            double pointMagnitude = determineMagnitude(pointVector[0], pointVector[1]);
            if (pointMagnitude < selectBoid->view) { additiveAngle = determineAngleBetweenVectors(selectBoid, &pointVector, &pointMagnitude); }
            deltaAngle = (deltaAngle + additiveAngle);
        }
    }
    return deltaAngle;
}

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidList, const int* boidsAmount, const int* screenWidth, const int* screenHeight)
{ 
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidList"); };
        int currentBoidIndex = i;
        double dAngleList[3] = {0, 0, 0};
        dAngleList[0] = seperation(boidList[i], boidList, boidsAmount, i);
        dAngleList[1] = alignment(boidList[i], boidList, boidsAmount, i);

    }
    return;
}

// Using the data updated via calculate(), simulate the environment for one tick based on the specified rules.
void simulate(boid** boidList, const int* boidsAmount, const int* screenWidth, const int* screenHeight)
{
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidList"); };
        boid* currentBoid = boidList[i];
        int newX = (currentBoid->x + currentBoid->velocity[0]);
        int newY = (currentBoid->y + currentBoid->velocity[1]);

        if (newX <= (*screenWidth - (*screenWidth / 10)) || newX >= (*screenWidth + (*screenWidth / 10))) { currentBoid->x = (newX); }
        if (newY <= (*screenHeight - (*screenHeight / 10)) || newY >= (*screenWidth +(*screenWidth / 10))) { currentBoid->y = (newY); }
    }
}
