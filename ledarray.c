/**
  ******************************************************************************
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ledarray.h"
#include "port.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define GPIO_Port_A GPIOC
#define GPIO_Port_B GPIOA
#define GPIO_Port_C	GPIOA
#define GPIO_Port_SER GPIOC
#define GPIO_Port_SCK GPIOA
#define GPIO_Port_RCK GPIOA

#define GPIO_Pin_A GPIO_Pin_5
#define GPIO_Pin_B GPIO_Pin_7
#define GPIO_Pin_C GPIO_Pin_5
#define GPIO_Pin_SER GPIO_Pin_4
#define GPIO_Pin_SCK GPIO_Pin_6
#define GPIO_Pin_RCK GPIO_Pin_4

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LEDArray_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_A;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port_A, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_B;
	GPIO_Init(GPIO_Port_B, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_C;
	GPIO_Init(GPIO_Port_C, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SER;
	GPIO_Init(GPIO_Port_SER, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SCK;
	GPIO_Init(GPIO_Port_SCK, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_RCK;
	GPIO_Init(GPIO_Port_RCK, &GPIO_InitStruct);
}

void LEDArray_Disp(u8 data[], u16 cnt) {
	u8 i, j;
	
	while (cnt--) { // 扫描cnt次
		for (i = 0; i < 8; ++i) { // row
			for (j = 0; j < 8; ++j) { // column(bit)
				if (data[i] & (1 << j)) GPIO_SetBits(GPIO_Port_SER, GPIO_Pin_SER);
				else GPIO_ResetBits(GPIO_Port_SER, GPIO_Pin_SER);
				
				GPIO_SetBits(GPIO_Port_SCK, GPIO_Pin_SCK);
				Delay(1);
				GPIO_ResetBits(GPIO_Port_SCK, GPIO_Pin_SCK);
			}
			
			if (i & 0x01) GPIO_SetBits(GPIO_Port_A, GPIO_Pin_A);
			else GPIO_ResetBits(GPIO_Port_A, GPIO_Pin_A);
			if (i & 0x02) GPIO_SetBits(GPIO_Port_B, GPIO_Pin_B);
			else GPIO_ResetBits(GPIO_Port_B, GPIO_Pin_B);
			if (i & 0x04) GPIO_SetBits(GPIO_Port_C, GPIO_Pin_C);
			else GPIO_ResetBits(GPIO_Port_C, GPIO_Pin_C);
			
			GPIO_SetBits(GPIO_Port_RCK, GPIO_Pin_RCK);
			Delay(1);
			GPIO_ResetBits(GPIO_Port_RCK, GPIO_Pin_RCK);
		}
	}
}
