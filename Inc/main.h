#ifndef MAIN_H_
#define MAIN_H_


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
#define APB1 HCLK/APB1_Prescaler
#define APB1_Tim APB1*2
#define PWM_freq 25000
#define TIM4_PSC 0
#define TIM4_ARR APB1_Tim/PWM_freq-1
#define BAUDRATE


void SystemClock_Config(void);
void GPIO_Config(void);
void TIM_Config(void);
void USART_Config(void);
void SysTick_Handler(void);
void USART2_IRQHandler(void);


#endif /* MAIN_H_ */
