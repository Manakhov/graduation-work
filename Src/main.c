#include "main.h"


int main(void)
{
	SystemClock_Config();
	SysTick_Config(SysTicks);
	TIM_Config();
	GPIO_Config();

    /* Loop forever */
	while(1);
}


void SystemClock_Config(void)
{
	/* set 5 WS latency */
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_5WS;

	/* enable and select HSE */
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

	/* set PLLM, PLLN */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= PLLM << RCC_PLLCFGR_PLLM_Pos;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= PLLN << RCC_PLLCFGR_PLLN_Pos;

	/* PLLP = 2 */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;

	/* AHB_prescaler = 1, APB1_prescaler = 4, APB2_prescaler = 2 */
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	/* enable and select PLL */
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}


void GPIO_Config(void)
{
	/* PD15 config */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER &= ~GPIO_MODER_MODER15;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;

	/* PD14 config */
	GPIOD->MODER &= ~GPIO_MODER_MODER14;
	GPIOD->MODER |= GPIO_MODER_MODER14_1;
	GPIOD->AFR[1] &= ~GPIO_AFRH_AFSEL14;
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL14_1;

	/* PA2 config */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODER2;
	GPIOA->MODER |= GPIO_MODER_MODER2_1;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2);
}


void TIM_Config(void)
{
	/* enable TIM4 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	/* set prescaler and auto-reload */
	TIM4->PSC = TIM4_PSC;
	TIM4->ARR = TIM4_ARR;

	/* configure CR1 register */
	TIM4->CR1 &= ~TIM_CR1_CMS;
	TIM4->CR1 &= ~TIM_CR1_DIR;
	TIM4->CR1 |= TIM_CR1_ARPE;

	/* set PWM mode 2 */
	TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
//	TIM4->CCMR2 &= ~TIM_CCMR2_OC3M;
	TIM4->CCMR2 |= (TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);

	/* initialize all the registers */
	TIM4->EGR |= TIM_EGR_UG;

	/* disable slave mode */
	TIM4->SMCR &= ~TIM_SMCR_SMS;

	/* set initial CCR3 */
	TIM4->CCR3 = 0;

	/* enable OC3 */
	TIM4->CCER |= (TIM_CCER_CC3P | TIM_CCER_CC3E);

	/* enable TIM4 counter */
	TIM4->CR1 |= TIM_CR1_CEN;
}


void SysTick_Handler(void)
{
	static uint16_t count = 0;
	static uint8_t add_value = 100;
	if (count > SysTicksClk)
	{
		/* change blue LED and PWM change red LED */
		if (GPIOD->ODR & GPIO_ODR_ODR_15)
		{
			GPIOD->ODR &= ~GPIO_ODR_ODR_15;
			TIM4->CCR3 = add_value;
		}
		else
		{
			GPIOD->ODR |= GPIO_ODR_ODR_15;
			TIM4->CCR3 = TIM4_ARR - add_value;
		}

		count = 0;
	}
	count++;
}


