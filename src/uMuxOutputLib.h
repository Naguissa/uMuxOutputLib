/**
 * Really tiny library to control multiplexed outputs.
 *
 * It cycles among multiplexed outputs using a timer to control all pins.
 *
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa@foroelectro.net
 * @version 1.0.0
 * @created 2019-08-08
 */
#ifndef _uMuxOutputLib_
	#define _uMuxOutputLib_

	#include "Arduino.h"
	#include "uTimerLib.h"

	class uMuxOutputLib {
		public:
			// Constructors
			uMuxOutputLib(uint8_t, uint8_t, int *, int *);
			uMuxOutputLib(uint8_t, uint8_t, int *, int *, unsigned int);

			// Set and get
			void setPinAbsolute(uint, bool);
			void setPinMuxPin(uint8_t, uint8_t, bool);
			bool getPinAbsolute(uint);
			bool getPinMuxPin(uint8_t, uint8_t);

			// To run main interrupt of this library; call it when you want to it start working.
			void attachInterrupt();

			// Used to set enabled/disabled values on MUXES
			bool activeMuxValue = false; // Se to FALSE to invert logic.

			// Aux methods
			static void interrupt(void);
			static uMuxOutputLib *_instance;

		private:
			void _interrupt(void);

			uint8_t _npins, _nmuxes;
			uint8_t  *_values = NULL;

			int *_muxes = NULL;
			int *_pins = NULL;

			int _freq = 70;

			unsigned char _currentMux = 0;
			unsigned char _lastMux = 0;
	};
#endif

