/**
  ******************************************************************************
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "HCSR04.h"
#include "port.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define GPIO_Port_TRIG GPIOC
#define GPIO_Port_ECHO GPIOA

#define GPIO_Pin_TRIG GPIO_Pin_4
#define GPIO_Pin_ECHO GPIO_Pin_6

#define SoundSpeed 340.0f
#define MaxDistance 4.0f

/* Private variables ---------------------------------------------------------*/
u8 overflow;
u16 time;

/* Private function prototypes -----------------------------------------------*/
void TIM2_IRQHandler(void);
	
/* Private functions ---------------------------------------------------------*/
void HCSR04_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_TRIG;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port_TRIG, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_ECHO;
	GPIO_Init(GPIO_Port_ECHO, &GPIO_InitStruct);
	
	TIM_DeInit(TIM2);
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 65535;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1; // Frequency: 72MHz / 72 = 1MHz = 1us/tick
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2; // 子优先级
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		overflow = 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

u8 HCSR04_Measure(float *dist) {
	u8 cnt = 5; // 测量cnt次取平均值
	float each[5], sum = 0.0f;
	u8 i;
	
	GPIO_ResetBits(GPIO_Port_TRIG, GPIO_Pin_TRIG);
	for (i = 0; i < cnt; ++i) {
		// 初始化
		overflow = 0;
		TIM_SetCounter(TIM2, 0);
		
		// 大于10us的高电平触发
		GPIO_SetBits(GPIO_Port_TRIG, GPIO_Pin_TRIG);
		Delay(50);
		GPIO_ResetBits(GPIO_Port_TRIG, GPIO_Pin_TRIG);
		
		while (GPIO_ReadInputDataBit(GPIO_Port_ECHO, GPIO_Pin_ECHO) == RESET); // 等待上升沿
		TIM_SetCounter(TIM2, 0); // 初始化计数值
		while (GPIO_ReadInputDataBit(GPIO_Port_ECHO, GPIO_Pin_ECHO) != RESET && !overflow); // 等待下降沿
		time = TIM_GetCounter(TIM2); // 获取计数值
		each[i] = (float)time * SoundSpeed / 2;
		
		overflow = overflow || each[i] > MaxDistance * 1e6f;
		if (overflow) {
			*dist = 0.0f;
			return 1;
		}
		
		Delay(100000); // 防止发射信号对回响信号的影响
	}
	for (i = 0; i < cnt; ++i) sum += each[i];
	*dist = sum / cnt / 1e3f; // 单位mm
	
	return 0;
}
