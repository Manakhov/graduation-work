#include <stdint.h>
#include <stm32f407xx.h>


#define HSE 8000000
#define PLLM 4
#define PLLN 168
#define PLLP 2
#define AHB_Prescaler 1
#define HCLK HSE/PLLM*PLLN/PLLP/AHB_Prescaler
#define SysTicksClk 6000
#define SysTicks HCLK/SysTicksClk
#define TIM2_PSC 0
#define TIM2_ARR 3359


void SystemClock_Config(void);
void GPIO_Config(void);
void SysTick_Handler(void);
void TIM_Config(void);


int main(void)
{
	SystemClock_Config();
	SysTick_Config(SysTicks);
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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE15_0;
}


void SysTick_Handler(void)
{
	static uint16_t count = 0;
	if (count > SysTicksClk)
	{
		if (GPIOD->ODR & GPIO_ODR_ODR_15)
			GPIOD->ODR &= ~GPIO_ODR_ODR_15;
		else
			GPIOD->ODR |= GPIO_ODR_ODR_15;
		count = 0;
	}
	count++;
}


void TIM_Config(void)
{
	/* enable TIM2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* set prescaler and auto-reload */
	TIM2->PSC = TIM2_PSC;
	TIM2->ARR = TIM2_ARR;

	/* set PWM mode 1 */
	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM2->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);

	/* set initial CCR2 */
	TIM2->CCR2 = 0;

	/* enable OC2 */
	TIM2->CCER |= TIM_CCER_CC2E;

	/* enable TIM2 counter */
	TIM2->CR1 |= TIM_CR1_CEN;
}


