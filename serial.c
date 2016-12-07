/**
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "serial.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define GPIO_Port_TX GPIOA
#define GPIO_Port_RX GPIOA

#define GPIO_Pin_TX GPIO_Pin_9
#define GPIO_Pin_RX GPIO_Pin_10

/* Private variables ---------------------------------------------------------*/
u8 USART_RX_BUF[256];
u16 USART_RX_STA;

/* Private function prototypes -----------------------------------------------*/
void USART1_IRQHandler(void);

/* Private functions ---------------------------------------------------------*/
void Serial_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_TX;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port_TX, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_RX;
	GPIO_Init(GPIO_Port_RX, &GPIO_InitStruct);
	
	USART_DeInit(USART1); // 置为缺省值
	USART_InitStruct.USART_BaudRate = 115200; // 波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // 收发模式
	USART_InitStruct.USART_Parity = USART_Parity_No; // 无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 8位字长
	USART_Init(USART1, &USART_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3; // 子优先级
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启中断
	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void) {
	u8 data;
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		data = USART_ReceiveData(USART1);
		if ((USART_RX_STA & 0x8000) == 0) {
			if ((USART_RX_STA & 0x4000) == 0) {
				if (data == 0x0D) USART_RX_STA |= 0x4000;
				else {
					USART_RX_BUF[USART_RX_STA & 0x3FFF] = data;
					if (++USART_RX_STA > 255) USART_RX_STA = 0;
				}
			}
			else {
				if (data != 0x0A) USART_RX_STA = 0;
				else USART_RX_STA |= 0x8000;
			}
		}
	}
}

void Serial_Read(u8 *data) {
	u8 i;
	
	for (i = 0; i < (USART_RX_STA & 0x3F); ++i) data[i] = USART_RX_BUF[i];
}

void Serial_Write(u8 *data) {
	u8 i;
	
	for (i = 0; i < (USART_RX_STA & 0x3F); ++i) {
		USART_SendData(USART1, USART_RX_BUF[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
	}
	USART_RX_STA = 0;
}

/**** support printf ****/
#pragma import(__use_no_semihosting)
struct __FILE { int handle; };
FILE __stdout;
_sys_exit(int x) { x = x; }
int fputc(int ch, FILE *f) {
	while ((USART1->SR & 0x40) == RESET);
	return USART1->DR = (u8)ch;
}
/************************/

void Serial_Print(char *data) {
	printf("%s\r\n", data);
}
