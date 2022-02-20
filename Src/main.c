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
#define APB1_Prescaler 4
#define APB1_Tim HCLK/APB1_Prescaler*2
#define PWM_freq 25000
#define TIM4_PSC 0
#define TIM4_ARR APB1_Tim/PWM_freq-1


void SystemClock_Config(void);
void GPIO_Config(void);
void SysTick_Handler(void);
void TIM_Config(void);


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
	GPIOD->MODER |= GPIO_MODER_MODE14_1;
	GPIOD->AFR[1] &= ~GPIO_AFRH_AFSEL14;
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL14_1;
}


void SysTick_Handler(void)
{
	static uint16_t count = 0;
	static uint16_t TIM4_CCR3 = 0;
	static int8_t add_value = 100;
	if (count > SysTicksClk)
	{
		/* change blue LED */
		if (GPIOD->ODR & GPIO_ODR_ODR_15)
			GPIOD->ODR &= ~GPIO_ODR_ODR_15;
		else
			GPIOD->ODR |= GPIO_ODR_ODR_15;

		/* PWM test on PA1 */
		TIM4_CCR3 = TIM4_CCR3 + add_value;
		if ((TIM4_CCR3 > TIM4_ARR) | (TIM4_CCR3 < 0))
		{
			add_value = -add_value;
			TIM4_CCR3 = TIM4_CCR3 + add_value;
		}
		TIM4->CCR3 = TIM4_CCR3;

		count = 0;
	}
	count++;
}


void TIM_Config(void)
{
	/* enable TIM4 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	/* set prescaler and auto-reload */
	TIM4->PSC = TIM4_PSC;
	TIM4->ARR = TIM4_ARR;

	/* set PWM mode 1 */
	TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3M;
	TIM4->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE);
	TIM4->CR1 |= TIM_CR1_ARPE;

	/* initialize all the registers */
	TIM4->EGR |= TIM_EGR_UG;

	/* disable slave mode */
	TIM4->SMCR &= ~TIM_SMCR_SMS;

	/* set initial CCR3 */
	TIM4->CCR3 = 0;

	/* enable OC3 */
	TIM4->CCER |= (TIM_CCER_CC3P | TIM_CCER_CC3E)

	/* enable TIM4 counter */
	TIM4->CR1 |= TIM_CR1_CEN;
}


