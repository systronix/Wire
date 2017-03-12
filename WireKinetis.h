/*
  TwoWire.h - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
*/

#ifndef TwoWireKinetis_h
#define TwoWireKinetis_h

#if defined(__arm__) && defined(TEENSYDUINO)

extern "C" void i2c0_isr(void);

#include <inttypes.h>
#include "Arduino.h"

#define BUFFER_LENGTH 32
#define WIRE_HAS_END 1

#if defined(__MKL26Z64__)
#define WIRE_HAS_STOP_INTERRUPT 1
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define WIRE_HAS_START_INTERRUPT 1
#define WIRE_HAS_STOP_INTERRUPT 1
#endif

class TwoWire : public Stream
{
public:
	TwoWire();
	void begin();
	void begin(uint8_t address);
	void begin(int address) {
		begin((uint8_t)address);
	}
	void end();
	void setClock(uint32_t frequency);
	void setSDA(uint8_t pin);
	void setSCL(uint8_t pin);
	void beginTransmission(uint8_t address) {
		txBuffer[0] = (address << 1);
		transmitting = 1;
		txBufferLength = 1;
	}
	void beginTransmission(int address) {
		beginTransmission((uint8_t)address);
	}
	uint8_t endTransmission(uint8_t sendStop);
	uint8_t endTransmission(void) {
		return endTransmission(1);
	}
	uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop);
	uint8_t requestFrom(uint8_t address, uint8_t quantity) {
		return requestFrom(address, quantity, (uint8_t)1);
	}
	uint8_t requestFrom(int address, int quantity, int sendStop) {
		return requestFrom((uint8_t)address, (uint8_t)quantity,
			(uint8_t)(sendStop ? 1 : 0));
	}
	uint8_t requestFrom(int address, int quantity) {
		return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)1);
	}
	virtual size_t write(uint8_t data);
	virtual size_t write(const uint8_t *data, size_t quantity);
	virtual int available(void) {
		return rxBufferLength - rxBufferIndex;
	}
	virtual int read(void) {
		if (rxBufferIndex >= rxBufferLength) return -1;
		return rxBuffer[rxBufferIndex++];
	}
	virtual int peek(void) {
		if (rxBufferIndex >= rxBufferLength) return -1;
		return rxBuffer[rxBufferIndex];
	}
	virtual void flush(void) {
	}
	void onReceive(void (*function)(int numBytes)) {
		user_onReceive = function;
	}
	void onRequest(void (*function)(void)) {
		user_onRequest = function;
	}
	// send() for compatibility with very old sketches and libraries
	void send(uint8_t b) {
		write(b);
	}
	void send(uint8_t *s, uint8_t n) {
		write(s, n);
	}
	void send(int n) {
		write((uint8_t)n);
	}
	void send(char *s) {
		write(s);
	}
	uint8_t receive(void) {
		int c = read();
		if (c < 0) return 0;
		return c;
	}
	size_t write(unsigned long n) {
		return write((uint8_t)n);
	}
	size_t write(long n) {
		return write((uint8_t)n);
	}
	size_t write(unsigned int n) {
		return write((uint8_t)n);
	}
	size_t write(int n) {
		return write((uint8_t)n);
	}
	using Print::write;
private:
	uint8_t rxBuffer[BUFFER_LENGTH];
	uint8_t rxBufferIndex;
	uint8_t rxBufferLength;
	uint8_t txAddress;
	uint8_t txBuffer[BUFFER_LENGTH+1];
	uint8_t txBufferIndex;
	uint8_t txBufferLength;
	uint8_t transmitting;
	uint8_t slave_mode;
	uint8_t irqcount;
	void onRequestService(void);
	void onReceiveService(uint8_t*, int);
	void (*user_onRequest)(void);
	void (*user_onReceive)(int);
	void sda_rising_isr(void);
	uint8_t sda_pin_num;
	uint8_t scl_pin_num;
	friend void i2c0_isr(void);
	friend void sda_rising_isr(void);
};

extern TwoWire Wire;

class TWBRemulation
{
public:
	inline TWBRemulation & operator = (int val) __attribute__((always_inline)) {
		if (val == 12 || val == ((F_CPU / 400000) - 16) / 2) { // 22, 52, 112
			I2C0_C1 = 0;
			#if F_BUS == 120000000
			I2C0_F = I2C_F_DIV288; // 416 kHz
			#elif F_BUS == 108000000
			I2C0_F = I2C_F_DIV256; // 422 kHz
			#elif F_BUS == 96000000
			I2C0_F = I2C_F_DIV240; // 400 kHz
			#elif F_BUS == 90000000
			I2C0_F = I2C_F_DIV224; // 402 kHz
			#elif F_BUS == 80000000
			I2C0_F = I2C_F_DIV192; // 416 kHz
			#elif F_BUS == 72000000
			I2C0_F = I2C_F_DIV192; // 375 kHz
			#elif F_BUS == 64000000
			I2C0_F = I2C_F_DIV160; // 400 kHz
			#elif F_BUS == 60000000
			I2C0_F = I2C_F_DIV144; // 416 kHz
			#elif F_BUS == 56000000
			I2C0_F = I2C_F_DIV144; // 389 kHz
			#elif F_BUS == 54000000
			I2C0_F = I2C_F_DIV128; // 422 kHz
			#elif F_BUS == 48000000
			I2C0_F = I2C_F_DIV112; // 400 kHz
			#elif F_BUS == 40000000
			I2C0_F = I2C_F_DIV96;  // 416 kHz
			#elif F_BUS == 36000000
			I2C0_F = I2C_F_DIV96;  // 375 kHz
			#elif F_BUS == 24000000
			I2C0_F = I2C_F_DIV64;  // 375 kHz
			#elif F_BUS == 16000000
			I2C0_F = I2C_F_DIV40;  // 400 kHz
			#elif F_BUS == 8000000
			I2C0_F = I2C_F_DIV20;  // 400 kHz
			#elif F_BUS == 4000000
			I2C0_F = I2C_F_DIV20;  // 200 kHz
			#elif F_BUS == 2000000
			I2C0_F = I2C_F_DIV20;  // 100 kHz
			#endif
			I2C0_C1 = I2C_C1_IICEN;
		} else if (val == 72 || val == ((F_CPU / 100000) - 16) / 2) { // 112, 232, 472
			I2C0_C1 = 0;
			#if F_BUS == 120000000
			I2C0_F = I2C_F_DIV1152; // 104 kHz
			#elif F_BUS == 108000000
			I2C0_F = I2C_F_DIV1024; // 105 kHz
			#elif F_BUS == 96000000
			I2C0_F = I2C_F_DIV960; // 100 kHz
			#elif F_BUS == 90000000
			I2C0_F = I2C_F_DIV896; // 100 kHz
			#elif F_BUS == 80000000
			I2C0_F = I2C_F_DIV768; // 104 kHz
			#elif F_BUS == 72000000
			I2C0_F = I2C_F_DIV640; // 112 kHz
			#elif F_BUS == 64000000
			I2C0_F = I2C_F_DIV640; // 100 kHz
			#elif F_BUS == 60000000
			I2C0_F = I2C_F_DIV576; // 104 kHz
			#elif F_BUS == 56000000
			I2C0_F = I2C_F_DIV512; // 109 kHz
			#elif F_BUS == 54000000
			I2C0_F = I2C_F_DIV512; // 105 kHz
			#elif F_BUS == 48000000
			I2C0_F = I2C_F_DIV480; // 100 kHz
			#elif F_BUS == 40000000
			I2C0_F = I2C_F_DIV384; // 104 kHz
			#elif F_BUS == 36000000
			I2C0_F = I2C_F_DIV320; // 113 kHz
			#elif F_BUS == 24000000
			I2C0_F = I2C_F_DIV240; // 100 kHz
			#elif F_BUS == 16000000
			I2C0_F = I2C_F_DIV160; // 100 kHz
			#elif F_BUS == 8000000
			I2C0_F = I2C_F_DIV80; // 100 kHz
			#elif F_BUS == 4000000
			I2C0_F = I2C_F_DIV40; // 100 kHz
			#elif F_BUS == 2000000
			I2C0_F = I2C_F_DIV20; // 100 kHz
			#endif
			I2C0_C1 = I2C_C1_IICEN;
		}
		return *this;
	}
	inline operator int () const __attribute__((always_inline)) {
		#if F_BUS == 120000000
		if (I2C0_F == I2C_F_DIV288) return 12;
		#elif F_BUS == 108000000
		if (I2C0_F == I2C_F_DIV256) return 12;
		#elif F_BUS == 96000000
		if (I2C0_F == I2C_F_DIV240) return 12;
		#elif F_BUS == 90000000
		if (I2C0_F == I2C_F_DIV224) return 12;
		#elif F_BUS == 80000000
		if (I2C0_F == I2C_F_DIV192) return 12;
		#elif F_BUS == 72000000
		if (I2C0_F == I2C_F_DIV192) return 12;
		#elif F_BUS == 64000000
		if (I2C0_F == I2C_F_DIV160) return 12;
		#elif F_BUS == 60000000
		if (I2C0_F == I2C_F_DIV144) return 12;
		#elif F_BUS == 56000000
		if (I2C0_F == I2C_F_DIV144) return 12;
		#elif F_BUS == 54000000
		if (I2C0_F == I2C_F_DIV128) return 12;
		#elif F_BUS == 48000000
		if (I2C0_F == I2C_F_DIV112) return 12;
		#elif F_BUS == 40000000
		if (I2C0_F == I2C_F_DIV96) return 12;
		#elif F_BUS == 36000000
		if (I2C0_F == I2C_F_DIV96) return 12;
		#elif F_BUS == 24000000
		if (I2C0_F == I2C_F_DIV64) return 12;
		#elif F_BUS == 16000000
		if (I2C0_F == I2C_F_DIV40) return 12;
		#elif F_BUS == 8000000
		if (I2C0_F == I2C_F_DIV20) return 12;
		#elif F_BUS == 4000000
		if (I2C0_F == I2C_F_DIV20) return 12;
		#endif
		return 72;
	}
};
extern TWBRemulation TWBR;

#endif
#endif
