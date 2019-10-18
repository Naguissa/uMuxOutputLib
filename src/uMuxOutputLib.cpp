/**
 * Really tiny library to control multiplexed outputs.
 *
 * It cycles among multiplexed outputs using a timer to control all pins.
 *
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa@foroelectro.net
 * @version 1.0.1
 * @created 2019-08-08
 */
#include <Arduino.h>
#include "uMuxOutputLib.h"

// Static variable assignment
uMuxOutputLib * uMuxOutputLib::_instance = NULL;


/**
 * Constructor
 *
 * @param uint8_t npins Number of pins
 * @param uint8_t nmuxes Number of muxes
 * @param int* array of pins for outputs
 * @param int* array of pins for muxes
 */
uMuxOutputLib::uMuxOutputLib(uint8_t npins, uint8_t nmuxes, int *pins, int *muxes) {
	_npins = npins;
	_nmuxes = nmuxes;
	uint8_t i;
	// Create arrays
	if (_pins != NULL) {
		free(_pins);
	}
	if (_muxes != NULL) {
		free(_muxes);
	}
	if (_values != NULL) {
		free(_values);
	}
	// We use uint8_t to store booleans at a position to don't waste extra memory
	_values = (uint8_t *) malloc(npins * nmuxes / 8 + 1); // Sometimes 1 extra byte, but prevents underallocating

	_pins = (int *) malloc(sizeof(int) * npins);
	_muxes = (int *) malloc(sizeof(int) * nmuxes);

	// Fill arrays
	for (i = 0; i < npins; i++) { // pins
		_pins[i] = pins[i];
		pinMode(pins[i], OUTPUT);
	}
	for (i = 0; i < nmuxes; i++) { // muxes
		_muxes[i] = muxes[i];
		pinMode(muxes[i], OUTPUT);
	}
	uint16_t max = npins * nmuxes / 8 + 1; // Stored values
	for (i = 0; i < max; i++) {
		_values[i] = 0;
	}
}


/**
 * Constructor
 *
 * @param uint8_t npins Number of pins
 * @param uint8_t nmuxes Number of muxes
 * @param int* array of pins for outputs
 * @param int* array of pins for muxes
 * @param unsigned int Refresh frequency (for all pins, will be multiplied by nmuxes to calculate end result)
 */
uMuxOutputLib::uMuxOutputLib(uint8_t npins, uint8_t nmuxes, int *pins, int *muxes, unsigned int freq) {
	_freq = freq;
	uMuxOutputLib(npins, nmuxes, pins, muxes);
}



/**
 * Sets a pin in linear method
 *
 * Absolute position is calculated as: mux * npins + pin
 *
 * @param uint16_t position Position of the bit on linear fashion
 * @param bool value Value to be set
 */
void uMuxOutputLib::setPinAbsolute(uint16_t position, bool value) {
	if (value) {
		_values[position / 8] |= (0b00000001 << (position % 8));
	} else {
		_values[position / 8] &= ~(0b00000001 << (position % 8));
	}
}



/**
 * Sets a pin by its pin and mux number.
 *
 * Note that positions starts at 0.
 *
 * @param uint8_t pin Pin position on passed pins array (starting at 0)
 * @param uint8_t mux Mux position on passed muxes array (starting at 0)
 * @param bool value Value to be set
 */
void uMuxOutputLib::setPinMuxPin(uint8_t pin, uint8_t mux, bool value) {
	setPinAbsolute(mux * _npins + pin, value);
}

/**
 * Gets stored status in linear method
 *
 * Absolute position is calculated as: mux * npins + pin
 *
 * @param uint16_t position Position of the bit on linear fashion
 * @return bool value Current value of that pin
 */
bool uMuxOutputLib::getPinAbsolute(uint16_t position) {
	return (bool) (_values[position / 8] & (0b00000001 << (position % 8)));
}


/**
 * Gets stored status by its pin and mux number.
 *
 * Note that positions starts at 0.
 *
 * @param uint8_t pin Pin position on passed pins array (starting at 0)
 * @param uint8_t mux Mux position on passed muxes array (starting at 0)
 * @return bool value Current value of that pin
 */
bool uMuxOutputLib::getPinMuxPin(uint8_t pin, uint8_t mux) {
	return getPinAbsolute(mux * _npins + pin);
}

/**
 * Attach Timer2 interrupt
 *
 * Needed for usual operation, but you can call loop manually instead
 */
void uMuxOutputLib::attachInterrupt() {
	extern uTimerLib TimerLib;
	if (_instance == NULL) {
		uMuxOutputLib::_instance = this;
		unsigned long int period_us = 1000000 / _freq / _nmuxes;
		TimerLib.setInterval_us(uMuxOutputLib::interrupt, period_us);
	}
}


/**
 * Main loop
 *
 * Refreshes all 8-segment digits
 */
void uMuxOutputLib::interrupt() {
	_instance->_interrupt();
}


void uMuxOutputLib::_interrupt(void) {

	// Off last mux
	digitalWrite(_muxes[_lastMux], !activeMuxValue);

	// Set new pins values
	uint16_t position = _currentMux * _npins; // Calculate base position and increment it in for loop to save instructions
	for (uint8_t i = 0; i < _npins; i++) {
		digitalWrite(_pins[i], (bool) (_values[position / 8] & (0b00000001 << (position % 8))));
		position++;
	}

	// On new mux
	digitalWrite(_muxes[_currentMux], activeMuxValue); // In case of muxes, off works inverse

	// Tidy
	_lastMux = _currentMux;
	_currentMux = (_currentMux + 1) % _nmuxes;
}
