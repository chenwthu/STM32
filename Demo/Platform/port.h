/**
  ******************************************************************************
  *
  ******************************************************************************
  */

#ifndef __PORT_H
#define __PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define CLOCKS_PER_SEC 1000000

/* Exported functions ------------------------------------------------------- */
void System_Init(void);
void Delay(__IO uint32_t nTime);
void Timing_Delay_Decrement(void);

#ifdef __cplusplus
}
#endif

#endif /* __PORT_H */
