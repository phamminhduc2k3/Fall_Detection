#include "timer.h"
#include "stm32f10x.h"

void timer_init(void)
{
	  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	  TIM4->PSC = (SystemCoreClock / 1000) - 1; // = 71999 => tao ra mot xung tren moi ms;
    TIM4->ARR = 10000 - 1;      // 1s ngat 1 lan
    TIM4->DIER |= TIM_DIER_UIE;  
    TIM4->CR1 |= TIM_CR1_CEN;  
	   
	  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	  TIM3->PSC = (SystemCoreClock / 1000) - 1;  
    TIM3->ARR = 5000 - 1;			// 0.5s ngat 1 lan
    TIM3->DIER |= TIM_DIER_UIE;  
    TIM3->CR1 |= TIM_CR1_CEN; 
		NVIC_EnableIRQ(TIM4_IRQn);
		NVIC_SetPriority(TIM4_IRQn, 1);
		NVIC_EnableIRQ(TIM3_IRQn);
		NVIC_SetPriority(TIM3_IRQn, 1);

}