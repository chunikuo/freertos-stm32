#ifndef BALL_H
#define BALL_H

#include "FreeRTOS.h"

typedef struct {
        float XPos;
        float YPos;
        uint16_t Radius;
        float XSpeed;
        float YSpeed;

        uint16_t TextColor;

} Ball, * pBall;

void DrawBall(pBall Balls);
void EraseBall(pBall Balls);

#endif

