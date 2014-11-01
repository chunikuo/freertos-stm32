#include "ball.h"
#include "stm32f429i_discovery_lcd.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


void DrawBall(pBall Balls)
{
        LCD_SetTextColor( Balls->TextColor );

        LCD_DrawFullCircle( (uint16_t)Balls->XPos, (uint16_t)Balls->YPos, Balls->Radius );
}

void EraseBall(pBall Balls)
{
        LCD_SetTextColor( LCD_COLOR_BLACK );

        LCD_DrawFullCircle( (uint16_t)Balls->XPos, (uint16_t)Balls->YPos, Balls->Radius );

}

