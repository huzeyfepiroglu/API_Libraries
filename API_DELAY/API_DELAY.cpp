/*
 * @file  			: API_DELAY.cpp
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
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "API_DELAY.h"

TIM_HandleTypeDef *htim;

void Init_Microsecond_Timer (TIM_HandleTypeDef *htim_num)
{
	  htim=htim_num;
	  HAL_TIM_Base_Start_IT(htim);
	  
}

void Delay_uS (uint16_t uS)
{
	uint32_t first_counter_value = __HAL_TIM_GET_COUNTER(htim);
	//__HAL_TIM_SET_COUNTER(htim,0);                 // set the counter value a 0
	while ((__HAL_TIM_GET_COUNTER(htim) - first_counter_value) < (uS-1));  // wait for the counter to reach the us input in the parameter
	
}

uint32_t millis()
{
  uint32_t value_mil = __HAL_TIM_GET_COUNTER(htim)/1000;
	return value_mil;
}

uint32_t micros()
{
	uint32_t value_mic = __HAL_TIM_GET_COUNTER(htim);
	return value_mic;
}

/*****END OF FILE****/
