#ifndef INIT_H_
#define INIT_H_

#include <msp430.h>

//Variables for the CAT4008
#define BLANK BIT4
#define SIN BIT7
#define SCLK BIT5
#define XLAT BIT0
#define MOSI BIT7


extern unsigned int g1msTimeout;
extern unsigned int g1mSTimer;

void ConfigureLEDs(void);
void varyBrightness();
void toggleBrightness(unsigned char LEDNumber, int intensity);
void InitializeLEDs();
void LEDOn(unsigned char LEDNumber);
void setDisplay(unsigned char byte_value);
void ConfigureLEDDisplay(void);
void Flat(void);

//Variables for the accelerometer
#define XOUT BIT0
#define YOUT BIT1
#define ZOUT BIT2

//Operations for the CAT4008 setup
#define SET_BLANK_AS_OUTPUT		P1DIR |= BLANK
#define DISABLE_BLANK			P1OUT &= ~BLANK
#define SET_SCLK_AS_OUTPUT		P1DIR |= SCLK
#define SCLK_LOW				P1OUT &= ~SCLK
#define SET_XLAT_AS_OUTPUT		P2DIR |= XLAT
#define XLAT_LOW				P2OUT &= ~XLAT
#define SET_SIN_AS_OUTPUT		P1DIR |= SIN
#define SIN_LOW					P1OUT &= ~SIN

//Variables for the accelerometer setup
#define SET_XOUT_AS_OUTPUT		P1DIR |= XOUT
#define ENABLE_XOUT				P1OUT |= XOUT
#define SET_YOUT_AS_OUTPUT		P1DIR |= YOUT
#define ENABLE_YOUT				P1OUT |= YOUT
#define SET_ZOUT_AS_OUTPUT		P1DIR |= ZOUT
#define ENABLE_ZOUT				P1OUT |= ZOUT

#endif /* INIT_H_ */
