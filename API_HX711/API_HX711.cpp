/*
 * @file  			: API_HX711.cpp
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
 * In embedded systems, the use of the C programming language is often greater.
 * However, the number of C++ libraries has exceeded the number of c libraries with the use of Arduino.
 * While C libraries are running in software written in C++, C++ libraries do not work in c based software.
 * At this point, I think it is more advantageous to write c ++ based software to develop the project faster.
 * My motto is this.
 */
#include <API_HX711.h>
#include "API_Delay_uS.h"

uint8_t shiftInSlow(GPIO_TypeDef *dataPin_port, uint8_t dataPin, GPIO_TypeDef *clockPin_port, uint8_t clockPin, uint8_t bitOrder)
{
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i)
	{
		HAL_GPIO_WritePin(clockPin_port, clockPin, GPIO_PIN_SET);
		Delay_uS(1);
		if (bitOrder == LSBFIRST)
			value |= HAL_GPIO_ReadPin(dataPin_port, dataPin) << i;
		else
			value |= HAL_GPIO_ReadPin(dataPin_port, dataPin) << (7 - i);
		HAL_GPIO_WritePin(clockPin_port, clockPin, GPIO_PIN_RESET);
		Delay_uS(1);
	}
	return value;
}

#define SHIFTIN_WITH_SPEED_SUPPORT(dataPin_port, dataPin, clockPin_port, clockPin, order) shiftInSlow(dataPin_port, dataPin, clockPin_port, clockPin, order)

HX711::HX711()
{
}

HX711::~HX711()
{
}

/*
void HX711::begin(int dout, int pd_sck, int gain) {
	PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	set_gain(gain);
}

Bu kisimda pin tanimlamasi yapiliyor. Zaten Cubemxte pinleri tanimladigimiz i�in
PD_SCK 'yi digital output ve DOUT 'u ise digital input olarak tanimlamak yetecektir.
set_gain fonksiyomnu ise basta cagirilmali. bu yuzden fonksiyon bu sekilde degistirildi.
*/
void HX711::begin(uint8_t gain)
{
	set_gain(gain);
}

bool HX711::is_ready()
{
	return HAL_GPIO_ReadPin(DOUT_GPIO_Port, DOUT_Pin) == false;
}

void HX711::set_gain(uint8_t gain)
{
	switch (gain)
	{
	case 128: // channel A, gain factor 128
		GAIN = 1;
		break;
	case 64: // channel A, gain factor 64
		GAIN = 3;
		break;
	case 32: // channel B, gain factor 32
		GAIN = 2;
		break;
	}
	HAL_GPIO_WritePin(PD_SCK_GPIO_Port, PD_SCK_Pin, GPIO_PIN_RESET);

	read();
}

long HX711::read()
{
	// Wait for the chip to become ready.
	wait_ready();

	// Define structures for reading data into.
	unsigned long value = 0;
	uint8_t data[3] = {0};
	uint8_t filler = 0x00;

	// Protect the read sequence from system interrupts.  If an interrupt occurs during
	// the time the PD_SCK signal is high it will stretch the length of the clock pulse.
	// If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
	// power down mode during the middle of the read sequence.  While the device will
	// wake up when PD_SCK goes low again, the reset starts a new conversion cycle which
	// forces DOUT high until that cycle is completed.

	// Disable interrupts.
	__disable_irq();

	// Pulse the clock pin 24 times to read the data.
	data[2] = SHIFTIN_WITH_SPEED_SUPPORT(DOUT_GPIO_Port, DOUT_Pin, PD_SCK_GPIO_Port, PD_SCK_Pin, MSBFIRST);
	data[1] = SHIFTIN_WITH_SPEED_SUPPORT(DOUT_GPIO_Port, DOUT_Pin, PD_SCK_GPIO_Port, PD_SCK_Pin, MSBFIRST);
	data[0] = SHIFTIN_WITH_SPEED_SUPPORT(DOUT_GPIO_Port, DOUT_Pin, PD_SCK_GPIO_Port, PD_SCK_Pin, MSBFIRST);

	// Set the channel and the gain factor for the next reading using the clock pin.
	for (uint8_t i = 0; i < GAIN; i++)
	{
		HAL_GPIO_WritePin(PD_SCK_GPIO_Port, PD_SCK_Pin, GPIO_PIN_SET);
		Delay_uS(1);
		HAL_GPIO_WritePin(PD_SCK_GPIO_Port, PD_SCK_Pin, GPIO_PIN_RESET);
		Delay_uS(1);
	}

	// Enable interrupts again.
	__enable_irq();

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80)
	{
		filler = 0xFF;
	}
	else
	{
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = (static_cast<unsigned long>(filler) << 24 | static_cast<unsigned long>(data[2]) << 16 | static_cast<unsigned long>(data[1]) << 8 | static_cast<unsigned long>(data[0]));

	return static_cast<long>(value);
}

void HX711::wait_ready(unsigned long delay_ms)
{
	while (!is_ready())
	{
		HAL_Delay(delay_ms);
	}
}

bool HX711::wait_ready_retry(uint8_t retries, unsigned long delay_ms)
{
	int count = 0;
	while (count < retries)
	{
		if (is_ready())
		{
			return true;
		}
		HAL_Delay(delay_ms);
		count++;
	}
	return false;
}

bool HX711::wait_ready_timeout(unsigned long timeout, unsigned long delay_ms)
{
	unsigned long millisStarted = millis();
	while (millis() - millisStarted < timeout)
	{
		if (is_ready())
		{
			return true;
		}
		HAL_Delay(delay_ms);
	}
	return false;
}

long HX711::read_average(uint8_t times)
{
	long sum = 0;
	for (int i = 0; i < times; i++)
	{
		sum += read();
		HAL_Delay(0);
	}
	return sum / times;
}

double HX711::get_value(uint8_t times)
{
	return read_average(times) - OFFSET;
}

float HX711::get_units(uint8_t times)
{
	return get_value(times) / SCALE;
}

void HX711::tare(uint8_t times)
{
	double sum = read_average(times);
	set_offset(sum);
}

void HX711::set_scale(float scale)
{
	SCALE = scale;
}

float HX711::get_scale()
{
	return SCALE;
}

void HX711::set_offset(long offset)
{
	OFFSET = offset;
}

long HX711::get_offset()
{
	return OFFSET;
}

void HX711::power_down()
{
	HAL_GPIO_WritePin(PD_SCK_GPIO_Port, PD_SCK_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PD_SCK_GPIO_Port, PD_SCK_Pin, GPIO_PIN_SET);
}

void HX711::power_up()
{
	HAL_GPIO_WritePin(PD_SCK_GPIO_Port, PD_SCK_Pin, GPIO_PIN_RESET);
}
