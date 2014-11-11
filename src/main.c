#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

void RCC_Configuration(void)
{
	/* GPIOA clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

int main()
{
	RCC_Configuration();
	GPIO_Configuration();

	GPIO_ToggleBits(GPIOG, GPIO_Pin_14);

	while(1)
	{
		GPIO_ToggleBits(GPIOG, GPIO_Pin_13);

		GPIO_ToggleBits(GPIOG, GPIO_Pin_14);

		for(int i=0; i<1000000; i++);
	}	
	return 0;
}

