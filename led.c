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
#define GPIO_Port_LED GPIOD

#define GPIO_Pin_LED GPIO_Pin_2

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LED_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_LED;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port_LED, &GPIO_InitStruct);
}

void LED_On(void) {
	GPIO_SetBits(GPIO_Port_LED, GPIO_Pin_LED);
}

void LED_Off(void) {
	GPIO_ResetBits(GPIO_Port_LED, GPIO_Pin_LED);
}
