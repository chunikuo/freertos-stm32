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

// IOE, Touch information
static TP_STATE* TP_State;

// the position of current Ball
// BObj[0] is MainBall
// BObj[1 ~ BulletNum] are Bullets
Ball BObj[BulletNum + 1];

// the position of all shown balls in two layers
// LayerBuffer[0] is LCD_FOREGROUND_LAYER
// LayerBuffer[1] is LCD_BACKGROUND_LAYER
Ball LayerBuffer[2][BulletNum + 1];

uint16_t ballSize = BallRADIUS;

// the initial speed of bullets
float BasicSpeed = 2.0f;

// record how many bullets generatored
uint32_t BallCount = 0;

bool isCollision = false;
bool isLoop = true;

// Initial function
void Init()
{
	// Clear two layer screen
	LCD_SetLayer(LCD_BACKGROUND_LAYER);
	LCD_Clear( LCD_COLOR_BLACK );
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_Clear( LCD_COLOR_BLACK );
	
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < BulletNum + 1; j++)
		{
			if (i)	// initial BObj
			{
	                	BObj[j].XPos = j == 0 ? ( LCD_PIXEL_WIDTH - 5 ) / 2 : -1.0f;
				BObj[j].YPos = j == 0 ? ( LCD_PIXEL_HEIGHT - 5 ) / 2 : -1.0f;
                		BObj[j].TextColor = j == 0 ? LCD_COLOR_WHITE : LCD_COLOR_YELLOW;
                		BObj[j].Radius = j == 0 ? BallRADIUS : BulletRADIUS;
			}
			
			// inital LayerBuffer
			LayerBuffer[i][j].XPos = LayerBuffer[i][j].YPos = -1.0f;
			LayerBuffer[i][j].TextColor = j == 0 ? LCD_COLOR_WHITE : LCD_COLOR_YELLOW;
			LayerBuffer[i][j].Radius = j == 0 ? BallRADIUS : BulletRADIUS;
		}
	}

	// Sorftware random number
        srand(rand());

        BasicSpeed = 2.0f;
        BallCount = 0;
        isCollision = false;

}

void MainBallEventTask()
{
	while (isLoop) 
	{
		// detect touch point		
	        TP_State = IOE_TP_GetState();

        	if (TP_State->TouchDetected && !isCollision)
        	{
			// calculate the basic distance
                	int16_t XLen = BObj[0].XPos - TP_State->X;
	                int16_t YLen = BObj[0].YPos - TP_State->Y;
        	        float r = sqrtf(pow(XLen, 2) + pow(YLen, 2)) / 10.0;

			// save the next point to BObj[0]
                	if (r >= 1.0)
                	{
                        	BObj[0].XPos -= (XLen / r);
	                        BObj[0].YPos -= (YLen / r);
        	        }
                	else {
                        	BObj[0].XPos = TP_State->X;
	                        BObj[0].YPos = TP_State->Y;
        	        }
        	}
		
		vTaskDelay( 10 );
	}
}

void CheckCollision(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1)
{
	// if the distance between ball(X0, Y0) and bullet(X1, Y1) is less than sum of bullet's and ball's radius
	// flag isCollision
        float d1 = sqrtf(pow(X0 - BObj[0].XPos, 2) + pow(Y0 - BObj[0].YPos, 2));
        float d2 = sqrtf(pow(X1 - BObj[0].XPos, 2) + pow(Y1 - BObj[0].YPos, 2));

        isCollision = !((d1 - (BulletRADIUS + BallRADIUS) > 0) && (d2 - (BulletRADIUS + BallRADIUS) > 0));
}

void BulletEventTask()
{
	while (isLoop)
	{
	        if (isCollision)        continue;

	        for(int i = 1; i <= BulletNum; i++)
        	{
                	if (BObj[i].XPos < 0 || BObj[i].YPos < 0)	// the bullet does not exist
	                {
        	                uint16_t XPos, YPos;
				// allocate a new bullet from 4 edge of screen
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

        	                BObj[i].XPos = XPos;
                	        BObj[i].YPos = YPos;

				// allocate a target position which is near to current main ball
                        	int16_t TargetXPos = rand() % TargetRange - (TargetRange / 2) + BObj[0].XPos;
	                        int16_t TargetYPos = rand() % TargetRange - (TargetRange / 2) + BObj[0].YPos;

        	                int16_t XLen = XPos - TargetXPos;
	                        int16_t YLen = YPos - TargetYPos;
        	                float r = sqrtf(pow(XLen, 2) + pow(YLen, 2)) / (rand() % 5 + BasicSpeed);

	                        if (r >= 1.0)
        	                {
                	                BObj[i].XSpeed = (XLen / r);
	                                BObj[i].YSpeed = (YLen / r);
        	                }
                	        else
	                        {
        	                        BObj[i].XSpeed = XLen;
                	                BObj[i].YSpeed = YLen;
                        	}

        	                BasicSpeed += 0.01;
                	}
	                else
        	        {	// the bullet had existed, continue move
                	        uint16_t cX = (uint16_t)BObj[i].XPos, cY = (uint16_t)BObj[i].YPos;

        	                BObj[i].XPos -= BObj[i].XSpeed;
                	        BObj[i].YPos -= BObj[i].YSpeed;

	                        if ((BObj[i].XPos > BulletRADIUS && BObj[i].XPos < RANGEWIDTH + BulletRADIUS) &&
        	                    (BObj[i].YPos > BulletRADIUS && BObj[i].YPos < RANGEHEIGHT + BulletRADIUS))
                	        {

	                                CheckCollision(cX, cY, (uint16_t)BObj[i].XPos, (uint16_t)BObj[i].YPos);
	
                        	        if(isCollision) break;
	                        }
        	                else
                	        {	// out of screen, reset the bullet's position
                                	BObj[i].XPos = BObj[i].YPos = -1.0f;
                        	}
                	}
		}
		vTaskDelay( 10 );
        }

}

void UpdateLayerInfo(int index)
{
	// update the current ball and bullets to LayerBuffer
	for( int i = 0; i < BulletNum + 1; i++)
	{	
		if ( LayerBuffer[index][i].XPos != BObj[i].XPos || LayerBuffer[index][i].YPos != BObj[i].YPos )
		{
			if ( LayerBuffer[index][i].XPos >= 0 || LayerBuffer[index][i].YPos >= 0 )
				EraseBall(&LayerBuffer[index][i]);

			LayerBuffer[index][i].XPos = BObj[i].XPos;
			LayerBuffer[index][i].YPos = BObj[i].YPos;

			
			if ( LayerBuffer[index][i].XPos >= 0 && LayerBuffer[index][i].YPos >= 0 )
				DrawBall(&LayerBuffer[index][i]);
		}			
	}
}

void DrawBallEventTask()
{
	bool isForeGround = true;

	LCD_SetLayer(LCD_BACKGROUND_LAYER);
	LCD_Clear( LCD_COLOR_BLACK );

	LCD_SetLayer( LCD_FOREGROUND_LAYER );
	while (isLoop)
	{
		if (isCollision) continue;

		if (isForeGround)	// Fore Ground
		{			
			UpdateLayerInfo(0);
			
			// Show ForeGround
			LCD_SetTransparency(0xFF);
		}
		else			// Back Ground
		{
			LCD_SetLayer( LCD_BACKGROUND_LAYER );
			UpdateLayerInfo(1);
			
			// Hide ForeGround
			LCD_SetLayer( LCD_FOREGROUND_LAYER );
			LCD_SetTransparency(0x00);		
		}	
		
		isForeGround = !isForeGround;
		vTaskDelay(5);
	}	
}

void ButtonEventTask()
{
        while (isLoop) {
                if( STM_EVAL_PBGetState( BUTTON_USER ) ){

			isCollision = true;
			vTaskDelay(10);
			Init();
                }
        }
}

void StartBulletTime()
{
	isLoop = true;

	Init();

	xTaskCreate( BulletEventTask, (char*) "Bullet Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        xTaskCreate( MainBallEventTask, (char*) "MainBall Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        xTaskCreate( DrawBallEventTask, (char*) "DrawBall Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
        //xTaskCreate( ButtonEventTask, (char*) "Button Event Task", 128, NULL, tskIDLE_PRIORITY + 1, NULL );

        //Call Scheduler
        vTaskStartScheduler();

}

void StopBulletTime()
{
        isLoop = false;

        vTaskEndScheduler();
}
