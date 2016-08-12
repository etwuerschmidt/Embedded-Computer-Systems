#include <msp430.h>
#include "init.h"
#include "timerA.h"
#include "LED.h"
#include "SPI.h"
#include "debounce.h"
#include "DTC.h"


SwitchDefine gPushButton;
int next = 0;
int cal = 1;
int move = 0;

int led[7] = {0x80, 0x08, 0x02, 0x20, 0xAA, 0x55, 0xFF};
unsigned int calibX[6];
unsigned int calibY[6];
unsigned int calibZ[6];
extern unsigned int measurement[3];
unsigned int XMax = 0;
unsigned int XMin = 0;
unsigned XCent = 0;
unsigned int YMax = 0;
unsigned int YMin = 0;
unsigned YCent = 0;
unsigned int ZMax = 0;
unsigned int ZMin = 0;
unsigned ZCent = 0;
//bounds[6] = {xmin, xmax, ymin, ymax, zmin, zmax}

// Function Prototypes
void ConfigureClockModule();
void InitializeGlobalVariables(void);
void ConfigureTimerA();
void Calibrate();
void DTC();
void ManageSoftwareTimers(void); // This function manages software timers.

void InitializePushButtonPortPin (void)
{
	P1DIR &= ~BIT3; //Declare button as input
	P1OUT |= BIT3; //Enable pull-up / pull-down
	P1REN |= BIT3; //Declare resistor as a pullup
}

void Calibrate(void) {
	while (next < 6) {
		ManageSoftwareTimers();
		Debouncer(&gPushButton);
		SPISendByte(led[next]);
		if (move) {
			calibX[next] = measurement[2];
			calibY[next] = measurement[1];
			calibZ[next] = measurement[0];
			next += 1;
			move = 0;
		}
		P2OUT |= XLAT;
		P2OUT &= ~XLAT;
	}

}

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    //Configure timers
    ConfigureClockModule();
    ConfigureTimerA();

    // Initialization of hardware.
	InitializeGlobalVariables();
	InitializePushButtonPortPin();
    //InitializeLEDPortPins();
    //ConfigureLEDs();
	_BIS_SR(GIE);		// interrupts enabled
	Calibrate();
	XMax = calibX[2];
	XMin = calibX[1];
	XCent = (XMax + XMin) / 2;
	YMax = calibY[4];
	YMin = calibY[3];
	YCent = (YMax + YMin) / 2;
	ZMax = calibZ[4];
	ZMin = calibZ[5];
	ZCent = (ZMax + ZMin) / 2;

	cal = 0;
	// Loop forever
    InitializeLEDPortPins();
    ConfigureLEDs();
	while(1) {
		//SPISendByte(led[next]);
		//ManageSoftwareTimers();
		//0x02 = WEST LED
		//0x08 = SOUTH LED
		//0x20 = EAST LED
		//0x80 = NORTH LED
		//0x01 = NORTHWEST LED
		//0x04 = SOUTHWEST LED
		//0x10 = SOUTHEAST LED
		//0x40 = NORTHEAST LED
		DTC();
	}

}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}

void InitializeGlobalVariables(void)
{
	g1mSTimeout = 0;
	g1msTimer = 0;
	InitializeSwitch(&gPushButton,(char *) &P1IN,(unsigned char) BIT3, 10, 20); //Initialize push button P1.3
	SET_BLANK_AS_OUTPUT;
	DISABLE_BLANK;
	SET_SCLK_AS_OUTPUT;
	SCLK_LOW;
	SET_XLAT_AS_OUTPUT;
	XLAT_LOW;
	SET_SIN_AS_OUTPUT;
	SIN_LOW;
}



