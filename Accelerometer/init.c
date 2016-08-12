#include <msp430.h>
#include "init.h"
#include "LED.h"

unsigned char duty[8];
unsigned char level;

void ConfigureLEDs(void);
void varyBrightness();
void toggleBrightness(unsigned char LED, int intensity);
void InitializeLEDs();
void LEDOn(unsigned char LED);
void setDisplay(unsigned char byte_value);
void ConfigureLEDDisplay(void);

void ConfigureLEDDisplay(void) {

	P1DIR |= BLANK;
	P1OUT &= ~BLANK;
	P1DIR |= SCLK;
	P1DIR |= MOSI;
	P2DIR |= XLAT;

	int i;

	for (i = 0; i < 8; i++) {
		toggleBrightness(i, 0);
	}
}

void Flat(void) {
	int i;
	for (i = 0; i < 8; i++) {
		toggleBrightness(i, 10);
	}
}

void setDisplay(unsigned char byte_value) {

	P2OUT &= ~XLAT;
	P1OUT &= ~MOSI;

	int i;

	for (i = 0; i < 8; i++) {
		P1OUT &= ~SCLK;

		if (byte_value & 0x80){
			P1OUT |= MOSI;
		} else {
			P1OUT &= ~MOSI;
		}
		byte_value <<= 1;

		P1OUT |= SCLK;
	}

	P2OUT |= XLAT;
}

void ConfigureLEDs(void) {
	ConfigureLEDDisplay();
	setDisplay(0x00);
}


void InitializeLEDs() {
	int i;
	for ( i = 0; i < 8; i++) {
		duty[i] = 0;
	}
}

void LEDOn(unsigned char LED) {
	InitializeLEDs();
	duty[LED] = 20;
}

void toggleBrightness(unsigned char LED, int intensity) {
	if (intensity < 0) {
		duty[LED] = 0;
		return;
	}
	if (intensity > 20) {
		duty[LED] = 20;
		return;
	}

	duty[LED] = intensity;
}

void varyBrightness() {
	unsigned char data = 0x00;

	unsigned char i;
	for (i = 0; i < 8; i++) {
		if (level < duty[i]) {
			data |= 0x1 << (7-i);
		}
	}

	level++;
	if (level == 20) {
		level = 0;
	}

	setDisplay(data);
}





