/**
  ******************************************************************************
  * 
	*	TM1637 4-Digit 7-Segment Display
	*
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "TM1637.h"
#include "port.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define GPIO_Port_CLK GPIOA
#define GPIO_Port_DIO GPIOC

#define GPIO_Pin_CLK GPIO_Pin_7
#define GPIO_Pin_DIO GPIO_Pin_5

#define TM1637_DIOIn() { GPIO_Port_DIO->CRL &= 0xFF0FFFFF; GPIO_Port_DIO->CRL |= 0x00800000; }
#define TM1637_DIOOut() { GPIO_Port_DIO->CRL &= 0xFF0FFFFF; GPIO_Port_DIO->CRL |= 0x00300000; }

/* Private variables ---------------------------------------------------------*/
u8 commonAnode[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xA0, 0x83, 0xA7, 0xA1, 0x84, 0x8E, 0xFF};
u8 commonCathode[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x00};

/* Private function prototypes -----------------------------------------------*/
void TM1637_Start(void);
void TM1637_Stop(void);
void TM1637_Ack(void);
void TM1637_WriteByte(u8);

/* Private functions ---------------------------------------------------------*/
void TM1637_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_CLK;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_Port_CLK, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_DIO;
	GPIO_Init(GPIO_Port_DIO, &GPIO_InitStruct);
}

void TM1637_Disp(u8 data[], u8 colonFlag) {
	TM1637_Start();
	TM1637_WriteByte(0x40); // 0x40: 地址自增；0x44：地址固定（数码管4位地址分别为0xC0, 0xC1, 0xC2, 0xC3）
	TM1637_Stop();
	
	TM1637_Start();
	TM1637_WriteByte(0xC0); // 起始地址
	TM1637_WriteByte(commonCathode[data[0]]);
	if (colonFlag) TM1637_WriteByte(commonCathode[data[1]] | 0x80);
	else TM1637_WriteByte(commonCathode[data[1]]);
	TM1637_WriteByte(commonCathode[data[2]]);
	TM1637_WriteByte(commonCathode[data[3]]);
	TM1637_Stop();
	
	TM1637_Start();
	TM1637_WriteByte(0x8F); // 开启显示，低四位控制亮度
	TM1637_Stop();
}

void TM1637_Start(void) {
	TM1637_DIOOut();
	GPIO_SetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
	GPIO_SetBits(GPIO_Port_DIO, GPIO_Pin_DIO);
	Delay(1);
	GPIO_ResetBits(GPIO_Port_DIO, GPIO_Pin_DIO);
}

void TM1637_Stop(void) {
	TM1637_DIOOut();
	GPIO_ResetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
	GPIO_ResetBits(GPIO_Port_DIO, GPIO_Pin_DIO);
	Delay(1);
	GPIO_SetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
	GPIO_SetBits(GPIO_Port_DIO, GPIO_Pin_DIO);
}

void TM1637_Ack(void) {
	TM1637_DIOIn();
	GPIO_ResetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
	while (GPIO_ReadInputDataBit(GPIO_Port_DIO, GPIO_Pin_DIO));
	GPIO_SetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
	Delay(1);
	GPIO_ResetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
}

void TM1637_WriteByte(u8 data) {
	u8 i;
	
	TM1637_DIOOut();
	for (i = 0; i < 8; ++i) {
		GPIO_ResetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
		if (data & 0x01) GPIO_SetBits(GPIO_Port_DIO, GPIO_Pin_DIO);
		else GPIO_ResetBits(GPIO_Port_DIO, GPIO_Pin_DIO);
		data >>= 1;
		GPIO_SetBits(GPIO_Port_CLK, GPIO_Pin_CLK);
		Delay(1);
	}
	TM1637_Ack();
}
