#include <msp430.h>
#include "debounce.h"
#include "timerA.h"
#include "LED.h"
#include "SPI.h"

// Global Variables
SwitchDefine gPushButton;
SwitchDefine rSwitchA;
SwitchDefine rSwitchB;
EncoderDefine rEncoder;

// Function Prototypes
void ConfigureClockModule();
void InitializeGlobalVariables(void);
void ManageSoftwareTimers(void); // This function manages software timers.
void ConfigureTimerA();

void InitializePushButtonPortPin (void)
{
	P1DIR &= ~BIT3; //Declare button as input
	P1OUT |= BIT3; //Enable pull-up / pull-down
	P1REN |= BIT3; //Declare resistor as a pullup
	P2DIR &= ~BIT3; //For rotary switch A
	P2DIR &= ~BIT4;	//For rotary switch B
}

void InitializeRotaryEncoder (void)
{
	SET_BLANK_AS_OUTPUT;
	DISABLE_BLANK;
	SET_SLCK_AS_OUTPUT;
	SLCK_LOW;
	SET_XLAT_AS_OUTPUT;
	XLAT_LOW;
	SET_SIN_AS_OUTPUT;
	SIN_LOW;
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    //Configure timers
    ConfigureClockModule();
    ConfigureTimerA();

    // Initialization of hardware.
    InitializePushButtonPortPin();
    InitializeRotaryEncoder();
	InitializeGlobalVariables();
	_BIS_SR(GIE);		// interrupts enabled

	// Loop forever
	while(1) {
		ManageSoftwareTimers();
		RotaryEncoder(&rEncoder);
		P2OUT |= XLAT;
		P2OUT &= ~XLAT;
	}
}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;
}

void InitializeGlobalVariables(void)
{
	g1mSTimeout = 0;
	g1msTimer = 0;
	gButtonPresses = 0;
	InitializeSwitch(&gPushButton,(char *) &P1IN,(unsigned char) BIT3, 10, 20); //Initialize push button P1.3
	InitializeSwitch(&rSwitchA, (char *) &P2IN, (unsigned char) BIT3, 5, 5); //Initialize switch A for the rotary encoder
	InitializeSwitch(&rSwitchB, (char *) &P2IN, (unsigned char) BIT4, 5, 5); //Initialize switch B for the rotary encoder
	InitializeEncoder(&rEncoder, &rSwitchA, &rSwitchB, &gPushButton); //Initialize Rotary encoder
	SET_LED1_AS_AN_OUTPUT;
	SET_LED2_AS_AN_OUTPUT;
	P1DIR |= BIT2;
	P1OUT &= ~BIT0;
	P1OUT &= ~BIT6;
}



