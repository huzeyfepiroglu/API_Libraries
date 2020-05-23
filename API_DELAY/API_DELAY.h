/*
 * @file  			: API_DELAY.h
 * @author			: huzeyfe piroglu
 * @date  			: 23.05.2020
 * @Keil  			: ARM MDK V5.30
 * @MCU   			: STM32F051K6T6 
 * @CubeMX			: STM32CubeMX V5.3.0
 * @mail   			: a.huzeyfepiroglu@gmail.com
 * @github 			: https://github.com/huzeyfepiroglu
 *
 * @Description :
 *
 * It can be fed with timers in Stm32.
 * What needs to be done is to set the timer to 1 microsecond.
 * You can start the Init function by entering the timer information you have activated.
 * @Example     : Init_Delay_uS_Timer(&htim1);
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __API_DELAY_H
#define __API_DELAY_H

#include "tim.h"
void Init_Microsecond_Timer (TIM_HandleTypeDef *htim_num);
void Delay_uS (uint16_t uS);
uint32_t millis();
uint32_t micros();


#endif /* __API_DELAY_H */

/*****END OF FILE****/
