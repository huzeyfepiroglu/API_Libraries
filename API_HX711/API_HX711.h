/*
 * @file  			: API_HX711.h
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
#ifndef API_HX711_h
#define API_HX711_h

#include "stm32f0xx_hal.h"
#include "gpio.h"

#define LSBFIRST 0
#define MSBFIRST 1

class HX711
{
	private:
		uint8_t PD_SCK;		// Power Down and Serial Clock Input Pin
		uint8_t DOUT;			// Serial Data Output Pin
		uint8_t GAIN;			// amplification factor
		long OFFSET;		  // used for tare weight
		float SCALE;			// used to return weight in grams, kg, ounces, whatever

	public:

		HX711();

		virtual ~HX711();

		// Initialize library with data output pin, clock input pin and gain factor.
		// Channel selection is made by passing the appropriate gain:
		// - With a gain factor of 64 or 128, channel A is selected
		// - With a gain factor of 32, channel B is selected
		// The library default is "128" (Channel A).
		//void begin(int dout, int pd_sck, int gain = 128);
	  void begin(uint8_t gain = 128); //açiklama headerda

		// Check if HX711 is ready
		// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
		// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
		bool is_ready();

		// Wait for the HX711 to become ready
		void wait_ready(unsigned long delay_ms = 0);
		bool wait_ready_retry(uint8_t retries = 3, unsigned long delay_ms = 0);
		bool wait_ready_timeout(unsigned long timeout = 1000, unsigned long delay_ms = 0);

		// set the gain factor; takes effect only after a call to read()
		// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
		// depending on the parameter, the channel is also set to either A or B
		void set_gain(uint8_t gain = 128);

		// waits for the chip to be ready and returns a reading
		long read();

		// returns an average reading; times = how many times to read
		long read_average(uint8_t times = 10);

		// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
		double get_value(uint8_t times = 1);

		// returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
		// times = how many readings to do
		float get_units(uint8_t times = 1);

		// set the OFFSET value for tare weight; times = how many times to read the tare value
		void tare(uint8_t times = 10);

		// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
		void set_scale(float scale = 1.f);

		// get the current SCALE
		float get_scale();

		// set OFFSET, the value that's subtracted from the actual reading (tare weight)
		void set_offset(long offset = 0);

		// get the current OFFSET
		long get_offset();

		// puts the chip into power down mode
		void power_down();

		// wakes up the chip after power down mode
		void power_up();
};

#endif /* API_HX711_h */
