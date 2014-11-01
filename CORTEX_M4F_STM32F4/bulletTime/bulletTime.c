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

#define TargetRange 100

#define BulletNum 5
#define BulletRADIUS 5

#define BallRADIUS 5

#define RANGEWIDTH (LCD_PIXEL_WIDTH - ((BulletRADIUS + 2) * 2))
#define RANGEHEIGHT (LCD_PIXEL_HEIGHT - ((BulletRADIUS + 2) * 2))


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

void CheckCollision(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1)
{
        float d1 = sqrtf(pow(X0 - ballX, 2) + pow(Y0 - ballY, 2));
        float d2 = sqrtf(pow(X1 - ballX, 2) + pow(Y1 - ballY, 2));

        isCollision = !((d1 - (BulletRADIUS + BallRADIUS) > 0) && (d2 - (BulletRADIUS + BallRADIUS) > 0));
}

void BulletEventTask()
{
	while (1)
	{
	        if (isCollision)        return;

	        for(int i = 0; i < BulletNum; i++)
        	{
                	if (Bullet[i].XPos < 0 || Bullet[i].YPos < 0)
	                {
        	                uint16_t XPos, YPos;
                	        switch(rand() % 4)
                        	{
	                                case 0:
        	                                XPos = BulletRADIUS + 1;
                	                        YPos = rand() % RANGEHEIGHT + BulletRADIUS;
                        	                break;
                                	case 1:
	                                        XPos = rand() % RANGEWIDTH + BulletRADIUS;
        	                                YPos = BulletRADIUS + 1;
                	                        break;
                        	        case 2:
                                	        XPos = RANGEWIDTH + BulletRADIUS - 1;
                                        	YPos = rand() % RANGEHEIGHT + BulletRADIUS;
	                                        break;
	                                default:
        	                                XPos = rand() % RANGEWIDTH + BulletRADIUS;
                	                        YPos = RANGEHEIGHT + BulletRADIUS - 1;
                        	                break;
	                        }

        	                Bullet[i].XPos = XPos;
                	        Bullet[i].YPos = YPos;

                        	int16_t TargetXPos = rand() % TargetRange - (TargetRange / 2) + ballX;
	                        int16_t TargetYPos = rand() % TargetRange - (TargetRange / 2) + ballY;

        	                int16_t XLen = XPos - TargetXPos;
	                        int16_t YLen = YPos - TargetYPos;
        	                float r = sqrtf(pow(XLen, 2) + pow(YLen, 2)) / (rand() % 5 + BasicSpeed);

	                        if (r >= 1.0)
        	                {
                	                Bullet[i].XSpeed = (XLen / r);
	                                Bullet[i].YSpeed = (YLen / r);
        	                }
                	        else
	                        {
        	                        Bullet[i].XSpeed = XLen;
                	                Bullet[i].YSpeed = YLen;
                        	}

	                        DrawBall(&Bullet[i]);
        	                BasicSpeed += 0.01;
                	}
	                else
        	        {
                	        uint16_t cX = (uint16_t)Bullet[i].XPos, cY = (uint16_t)Bullet[i].YPos;
	                        Ball PreBullet = Bullet[i];

        	                Bullet[i].XPos -= Bullet[i].XSpeed;
                	        Bullet[i].YPos -= Bullet[i].YSpeed;

	                        if ((Bullet[i].XPos > BulletRADIUS && Bullet[i].XPos < RANGEWIDTH + BulletRADIUS) &&
        	                    (Bullet[i].YPos > BulletRADIUS && Bullet[i].YPos < RANGEHEIGHT + BulletRADIUS))
                	        {

	                                CheckCollision(cX, cY, (uint16_t)Bullet[i].XPos, (uint16_t)Bullet[i].YPos);
	
        	                        EraseBall(&PreBullet);
                	                DrawBall(&Bullet[i]);

                        	        if(isCollision) break;
	                        }
        	                else
                	        {
                        	        EraseBall(&PreBullet);
                                	Bullet[i].XPos = Bullet[i].YPos = -1.0f;
                        	}
                	}
		}
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

	xTaskCreate( BulletEventTask, (char*) "Bullet Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        xTaskCreate( MainBallEventTask, (char*) "MainBall Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        xTaskCreate( DrawBallEventTask, (char*) "DrawBall Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        //xTaskCreate( GameEventTask3, (signed char*) "GameEventTask3", 128, NULL, tskIDLE_PRIORITY + 1, NULL );

        //Call Scheduler
        vTaskStartScheduler();

}

void StopBulletTime()
{
}
