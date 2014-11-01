#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <malloc.h>

#include "ball.h"
#include "task.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ioe.h"

#define BulletNum 5
#define BulletRADIUS 5

#define BallRADIUS 5

static TP_STATE* TP_State;

// Ball
Ball Bullet[BulletNum];

uint16_t ballSize = BallRADIUS;
float ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
float ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;


float BasicSpeed = 2.0f;
uint32_t BallCount = 0;
bool isCollision = false;

void Init()
{
	for(int i = 0; i < BulletNum; i++)
        {
                if (Bullet[i].XPos > 0 && Bullet[i].YPos > 0)
                        EraseBall(&Bullet[i]);

                Bullet[i].XPos = Bullet[i].YPos = -1.0f;
                Bullet[i].TextColor = LCD_COLOR_YELLOW;
                Bullet[i].Radius = BulletRADIUS;
        }

        srand(rand());
        BasicSpeed = 2.0f;
        BallCount = 0;
        isCollision = false;

}

void MainBallEventTask()
{
	while (1) 
	{
	        TP_State = IOE_TP_GetState();

        	if (TP_State->TouchDetected && !isCollision)
        	{
                	int16_t XLen = ballX - TP_State->X;
	                int16_t YLen = ballY - TP_State->Y;
        	        float r = sqrtf(pow(XLen, 2) + pow(YLen, 2)) / 10.0;

                	if (r >= 1.0)
                	{
                        	ballX -= (XLen / r);
	                        ballY -= (YLen / r);
        	        }
                	else {
                        	ballX = TP_State->X;
	                        ballY = TP_State->Y;
        	        }
        	}
		
		vTaskDelay( 10 );
	}
}

void DrawBallEventTask()
{
	while (1)
	{
		LCD_SetTextColor( LCD_COLOR_WHITE );
		LCD_DrawFullCircle( (int16_t)ballX, (int16_t)ballY, ballSize);
	}	
}

void StartBulletTime()
{
	Init();
	//xTaskCreate( GameTask, (signed char*) "GameTask", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        xTaskCreate( MainBallEventTask, (signed char*) "MainBall Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        xTaskCreate( DrawBallEventTask, (signed char*) "DrawBall Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        //xTaskCreate( GameEventTask3, (signed char*) "GameEventTask3", 128, NULL, tskIDLE_PRIORITY + 1, NULL );

        //Call Scheduler
        vTaskStartScheduler();

}

void StopBulletTime()
{
}
