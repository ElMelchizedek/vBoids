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
        newBoid->velocity[0] = (rand() % (50 - (-50) + 1) + (-20)) + 1;
        newBoid->velocity[1] = (rand() % (50 - (-50) + 1) + (-20)) + 1;
        
        newBoid->speed = 1.0;

        newBoid->bubble = determineMagnitude(50, 50);
        newBoid->view = 90 * (M_PI / 180); 

        boidList[*boidsCount] = newBoid;
        *boidsCount = *boidsCount + 1;
    }
    return;
}

// Find the distance between the current boid in the loop, and the closest one to it.
// Using this, it will determine the acceleration based on the veracity of its the boid's closeness to their neighbours:
// When they're in its bubble, the closer they are the more negative acceleration, otherwise the further it is from its friends,
// the more positive acceleration it will have.
int determineAcceleration(boid* currentBoid, boid** boidsList, const int* boidsAmount, float* nearbyBoidCentre, const int* screenWidth, 
                            const int* screenHeight)
{
    // Alter acceleration
    float accelerationModifiers[6] = {0, 0, 0, 0, 0, 0};
    float accelerationLeftVar[6] = {2, -1, -1, -1, -1, -1};
    float accelerationRightVar[6] = {(*nearbyBoidCentre), (*nearbyBoidCentre), (currentBoid->y), (*screenHeight - currentBoid->y), (currentBoid->x),
                                        (*screenWidth - currentBoid->x)};
    float acceleration = 0.0;
    for (int i = 0; i < 6; i++)
    {
        if (accelerationRightVar[i] != 0)
        {
            accelerationModifiers[i] = (accelerationLeftVar[i] / accelerationRightVar[i]);
        }
        acceleration = acceleration + accelerationModifiers[i];
    }
    return (acceleration * 100);

}

// Figures out what the angle to the averge position of the nearby boids within the selected boid's visual range is.
double determineAngleNearbyBoids(boid* currentBoid, boid** boidsList, const int* boidsAmount, int currentBoidIndex, const int* screenWidth,
                                    const int* screenHeight)
{
    double angle = 0.0;
    int* nearbyBoidCentreCoords = (int*)malloc(2 * sizeof(int));
    nearbyBoidCentreCoords[0] = 0;
    nearbyBoidCentreCoords[1] = 0;

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
                nearbyBoidCentreCoords[0] = ((nearbyBoidCentreCoords[0] + compareBoid->x) / 2);
                nearbyBoidCentreCoords[1] = ((nearbyBoidCentreCoords[1] + compareBoid->y) / 2);
                angle = ((angle + newAngle) / 2); 
            }
        }
    }
    float nearbyBoidCentre = determineMagnitude(nearbyBoidCentreCoords[0], nearbyBoidCentreCoords[1]);
    currentBoid->acceleration = determineAcceleration(currentBoid, boidsList, boidsAmount, &nearbyBoidCentre, screenWidth, screenHeight);
    return angle; 
}

int* determineVelocity(boid* currentBoid, double* angle)
{
    int* newVelocity = (int*)malloc(2 * sizeof(int));
    double newVelX = (currentBoid->tickSpeed * cos(*angle));
    double newVelY = (currentBoid->tickSpeed * sin(*angle));
    newVelocity[0] = (int)lround(newVelX); 
    newVelocity[1] = (int)lround(newVelY); 
    return newVelocity;
}

// int determineSpeed(int currentSpeed, bool avoid)
// {  
//     int newSpeed = 0;
//     if (avoid) { newSpeed = currentSpeed * -2; } else { newSpeed = currentSpeed * 2; }
//     return newSpeed;
// }

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidsList, const int* boidsAmount, const int* screenWidth, const int* screenHeight)
{ 
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList"); };
        int currentBoidIndex = i;
        double angle = 0.0;
        // Get angle to nearby boids in visual range, for calculating new velocity.
        angle = determineAngleNearbyBoids(boidsList[i], boidsList, boidsAmount, currentBoidIndex, screenWidth, screenHeight);
        // Calculate speed
        boidsList[i]->tickSpeed = (int)roundf((boidsList[i]->speed * boidsList[i]->acceleration));

        // Calculate velocity if recieved new angle
        printf("%d %d\n", boidsList[i]->velocity[0], boidsList[i]->velocity[1]);
        if (!isnan(angle))
        {
            boidsList[i]->velocity = determineVelocity(boidsList[i], &angle);
        }
    }
    return;
}

// Using the data updated via calculate(), simulate the environment for one tick based on the specified rules.
void simulate(boid** boidsList, const int* boidsAmount, const int* screenWidth, const int* screenHeight)
{
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidsList[i] == NULL) { errorHandle(E_NULL, "boidsList"); };
        boid* currentBoid = boidsList[i];
        int newX = (currentBoid->x + currentBoid->velocity[0]);
        int newY = (currentBoid->y + currentBoid->velocity[1]);

        currentBoid->x = (newX);
        currentBoid->y = (newY);
    }
}
