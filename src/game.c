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

boids getBoidsInRange(boid** boidList, boid* currentBoid, const int* boidsAmount, int currentIndex, int range)
{
    boid** selectBoids = (boid**)malloc(*boidsAmount * sizeof(boid*));
    int countSelectBoids = 0;
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (!(i == currentIndex))
        {
            boid* compareBoid = boidList[i];
            int* vectorBetween = (int*)malloc(2 * sizeof(int));
            vectorBetween[0] = (compareBoid->x - currentBoid->x);
            vectorBetween[1] = (compareBoid->y - currentBoid->y);           
            int magnitudeBetween = determineMagnitude(vectorBetween[0], vectorBetween[1]); 
            if (magnitudeBetween <= range)
            {
                selectBoids[countSelectBoids] = compareBoid;
                countSelectBoids++;
            }
        }
    }
    
    boids returnBoids = {selectBoids, countSelectBoids};
    return returnBoids;
}

int* getAveragePositionBoids(boids boidsGroup)
{
    int* aggregatePosition = (int*)malloc(2 * sizeof(int));
    for (int i = 0; i < boidsGroup.boidCount; i++)
    {
        aggregatePosition[0] = (aggregatePosition[0] + boidsGroup.boidArray[i]->x);
        aggregatePosition[1] = (aggregatePosition[1] + boidsGroup.boidArray[i]->y);
    }
    int* averagePosition = (int*)malloc(2 * sizeof(int));
    for (int j = 0; j < 2; j++) { averagePosition[j] = (aggregatePosition[j] / boidsGroup.boidCount); }
    return averagePosition;
}

void initialiseBoidList(boid** boidList, const int* boidsAmount, int* boidsCount, const int* screenWidth, const int* screenHeight, params* setParams)
{
    int generateXUpperLimit = ((*screenWidth / 2) + (*screenWidth / 10));
    int generateYUpperLimit = ((*screenHeight / 2) + (*screenHeight / 10));
    int generateXLowerLimit = ((*screenWidth / 2) - (*screenWidth / 10));
    int generateYLowerLimit = ((*screenHeight / 2) - (*screenHeight / 10));
    srand(time(0));

    double setAngle = (((float)rand() / (float)RAND_MAX) * (2 * M_PI));

    for (int i = 0; i < *boidsAmount; i++) 
    {
        boid* newBoid = (boid*)malloc(sizeof(boid));

        newBoid->x = (rand() % (generateXUpperLimit - generateXLowerLimit + 1) + generateXLowerLimit);
        newBoid->y = (rand() % (generateYUpperLimit - generateYLowerLimit + 1) + generateYLowerLimit);

        // newBoid->x = (*screenWidth / 2);
        // newBoid->y = (*screenHeight / 2);

        newBoid->speed = setParams->speed;

        newBoid->angle = setAngle;

        newBoid->velocity = (int*)malloc(2 * sizeof(int));
        newBoid->velocity[0] = (int)round(newBoid->speed * cos(newBoid->angle));
        newBoid->velocity[1] = (int)round(newBoid->speed * sin(newBoid->angle));
        
        newBoid->acceleration = (int*)malloc(2 * sizeof(int)); 
        newBoid->acceleration[0] = 0;
        newBoid->acceleration[1] = 0;

        newBoid->bubble = setParams->bubble; 
        newBoid->view = setParams->view;

        boidList[*boidsCount] = newBoid;
        *boidsCount = *boidsCount + 1;
    }
    return;
}

// Alignment
// Get all of the boids the current boid can see; Find average directions of said boids; Nudge current boid to face that direction by a tenth of it.
double alignment(boid** boidList, boid* currentBoid, const int* boidsAmount, int currentIndex)
{
    boids groupNearbyBoids = getBoidsInRange(boidList, currentBoid, boidsAmount, currentIndex, currentBoid->view);
    boid** nearbyBoids = groupNearbyBoids.boidArray;
    int countNearbyBoids = groupNearbyBoids.boidCount;
    if (countNearbyBoids == 0) { return 0.0; }

    double aggregateAngle = 0.0;
    for (int j = 0; j < countNearbyBoids; j++)
    {
        aggregateAngle = (aggregateAngle + nearbyBoids[j]->angle);
    }
    double averageAngle = (aggregateAngle / countNearbyBoids);

    double differenceAngle = (averageAngle - currentBoid->angle);
    return (differenceAngle / 10);
}

// Cohesion
// Get all visible boids to current boid; Get average position of said boids; Get angle towards position from current boid;
// Nudge current boid to face it slightly.
double cohesion(boid** boidList, boid* currentBoid, const int* boidsAmount, int currentIndex)
{
    boids groupVisibleBoids = getBoidsInRange(boidList, currentBoid, boidsAmount, currentIndex, currentBoid->view);
    int countVisibleBoids = groupVisibleBoids.boidCount;

    if (countVisibleBoids == 0) { return 0.0; }

    int* averagePosition = getAveragePositionBoids(groupVisibleBoids);

    int averageVector[2] = {(averagePosition[0] - currentBoid->x), (averagePosition[1] - currentBoid->y)};
    double angleBetween = atan2(averageVector[1], averageVector[0]);

    double differenceAngle = (angleBetween - currentBoid->angle);
    return(differenceAngle / 10);

}

// Seperation
// Get all boids within bubble; Get average position of said boids; Get angle towards position from current boid;
// Nudge current boid away from it slightly.
double seperation(boid** boidList, boid* currentBoid, const int* boidsAmount, int currentIndex)
{
    boids invasiveBoids = getBoidsInRange(boidList, currentBoid, boidsAmount, currentIndex, currentBoid->bubble);

    if (invasiveBoids.boidCount == 0) { return 0.0; }

    int* averagePosition = getAveragePositionBoids(invasiveBoids);

    int averageVector[2] = {(averagePosition[0] - currentBoid->x), (averagePosition[1] - currentBoid->y)};
    double angleBetween = atan2(averageVector[1], averageVector[0]);

    double differenceAngle = (angleBetween - currentBoid->angle);
    double fractionDifferenceAngle = (differenceAngle / 10);
    return(-fractionDifferenceAngle);
}

// Does calculations based on previous tick (or at beginning, based on initialisation).
void calculate(boid** boidList, const int* boidsAmount, const int* screenWidth, const int* screenHeight, int lastSeconds)
{ 
    for (int i = 0; i < *boidsAmount; i++)
    {
        if (boidList[i] == NULL) { errorHandle(E_NULL, "boidList"); };

        // secondsIncrease: The new, increased amount of seconds since SDL init
        // newseconds: How many seconds have passed since last time we checked.
        // deltaseconds: The difference in seconds between when we last checked, and the amount that has passed since we last checked.
        // int secondsIncrease = (int)round((float)SDL_GetTicks() / 1000);
        // int newSeconds = (secondsIncrease - lastSeconds);

        // Update velocity with incrementation of the current boid's angle with 
        // the angles returned by alignment, cohesion, and seperation.
        boidList[i]->angle += alignment(boidList, boidList[i], boidsAmount, i);
        boidList[i]->angle += cohesion(boidList, boidList[i], boidsAmount, i);
        boidList[i]->angle += seperation(boidList, boidList[i], boidsAmount, i);
        boidList[i]->velocity[0] = (int)round(boidList[i]->speed * cos(boidList[i]->angle));
        boidList[i]->velocity[1] = (int)round(boidList[i]->speed * sin(boidList[i]->angle));

        // Alter the position of the boid based on velocity. If reach eastern or southern edge of screen, wrap the coords around back to 0.
        boidList[i]->x = (boidList[i]->x + boidList[i]->velocity[0]) % *screenWidth;
        boidList[i]->y = (boidList[i]->y + boidList[i]->velocity[1]) % *screenHeight;

        // Same wrapping but for western and northen edges of the screen. 
        if (boidList[i]->x < 0) { boidList[i]->x += *screenWidth; }
        if (boidList[i]->y < 0) { boidList[i]->y += *screenHeight; }

    }
    return;
}

void changeParams(boid** boidList, const int* boidsAmount, int keyChoice, params* selectParams)
{
    switch(keyChoice)
    {
        case SDLK_q:
            selectParams->speed++;
            break;
        case SDLK_w:
            selectParams->view++;
            break;
        case SDLK_e:
            selectParams->bubble++;
            break;
        case SDLK_a:
            selectParams->speed--;
            break;
        case SDLK_s:
            selectParams->view--;
            break;
        case SDLK_d:
            selectParams->bubble--;
            break;
        default:
            break;
    }
    for (int i = 0; i < *boidsAmount; i++)
    {
        boidList[i]->speed = selectParams->speed;
        boidList[i]->view = selectParams->view;
        boidList[i]->bubble = selectParams->bubble;
    }
    printf("NEW PARAMETERS\n");
    printf("SPEED: %d\n", selectParams->speed);
    printf("VIEW: %d\n", selectParams->view);
    printf("BUBBLE: %d\n", selectParams->bubble);
    return;
}