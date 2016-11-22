/**
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define GPIO_Port_LED GPIOA

#define GPIO_Pin_LED GPIO_Pin_1

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LED_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
//	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE); // 管脚重映像
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_LED;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port_LED, &GPIO_InitStruct);
	
	TIM_DeInit(TIM2); // 置为缺省值
	TIM_InternalClockConfig(TIM2); // 使用内部时钟源
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 7200 - 1; // PWM Frequency: 72MHz / 7200 = 10KHz
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); // 预装载配置
	TIM_Cmd(TIM2, ENABLE);
}

void LED_On(void) {
	GPIO_SetBits(GPIO_Port_LED, GPIO_Pin_LED);
}

void LED_Off(void) {
	GPIO_ResetBits(GPIO_Port_LED, GPIO_Pin_LED);
}

void LED_SetIntensity(u16 intensity) {
	if (intensity > 100) intensity = 100; // intensity范围0~100
	if (intensity == 0) LED_Off();
	else {
		TIM_SetCompare2(TIM2, TIM2->ARR * intensity / 100);
		LED_On();
	}
}
