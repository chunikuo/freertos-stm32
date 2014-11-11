#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f429i_discovery_lcd.h"

#include "main.h"

#include "traffic.h"
#include "bulletTime.h"

#include "FreeRTOS.h"
#include "task.h"

void DrawMenu()
{
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	LCD_DisplayStringLine(90, "    Traffic");
			
	LCD_DisplayStringLine(190, "  BulletTime");

	LCD_DrawRect(20, 50, 100, 180);
	LCD_DrawRect(20, 150, 100, 180);
}

void TouchEventTask()
{
	static TP_STATE* TP_State;
        while (1)
        {
                TP_State = IOE_TP_GetState();

                if (TP_State->TouchDetected)
                {
                        if (TP_State->X >= 20 && TP_State->X <= 200 && TP_State->Y >= 50 && TP_State->Y <= 150)
			{
				vTaskEndScheduler();
				StartTraffic();

				break;
			}
			
                        if (TP_State->X >= 20 && TP_State->X <= 200 && TP_State->Y >= 150 && TP_State->Y <= 250)
				StartBulletTime();
                }

                vTaskDelay( 10 );
        }
}

void StartMenu()
{
	DrawMenu();
	xTaskCreate(TouchEventTask, "Touch Evnet Task", 256, ( void * ) NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();
}

