#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"

void RCC_Configuration(void)
{
	/* GPIOA clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
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


	GPIO_InitTypeDef GPIO_InitStructure_b;
	GPIO_InitStructure_b.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure_b.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure_b.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure_b.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure_b.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure_b);
}

void Interrupts_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	/* Connect EXTI Line 0 to the button GPIO Pin */
   	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource0);
 
	/* Configure the EXTI line to generate an interrupt when the button is
	 * pressed.  The button pin is high when pressed, so it needs to trigger
	 * when rising from low to high. */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	 
	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line0) != RESET)
	{
		GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

int main()
{
	RCC_Configuration();
	GPIO_Configuration();
	Interrupts_Configuration();

	GPIO_ToggleBits(GPIOG, GPIO_Pin_14);

	while(1)
	{
		GPIO_ToggleBits(GPIOG, GPIO_Pin_14);

		for(int i=0; i<1000000; i++);
	}	
	return 0;
}

