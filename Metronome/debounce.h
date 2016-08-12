#ifndef FSM_DEBOUNCE_H_
#define FSM_DEBOUNCE_H_

#include <msp430.h>

// Definitions for debounce Times for the P1.3 button (ms)
#define HIGH_THRESHOLD 10
#define LOW_THRESHOLD 20

//Variables for the 7 segment display
#define BLANK BIT4
#define SIN BIT7
#define SLCK BIT5
#define XLAT BIT0

//Operations for the 7 segment display setup
#define SET_BLANK_AS_OUTPUT		P1DIR |= BLANK
#define DISABLE_BLANK			P1OUT &= ~BLANK
#define SET_SLCK_AS_OUTPUT		P1DIR |= SLCK
#define SLCK_LOW				P1OUT &= ~SLCK
#define SET_XLAT_AS_OUTPUT		P2DIR |= XLAT
#define XLAT_LOW				P2OUT &= ~XLAT
#define SET_SIN_AS_OUTPUT		P1DIR |= SIN
#define SIN_LOW					P1OUT &= ~SIN

// Type Definitions
typedef enum {
	DbExpectHigh, DbValidateHigh, DbExpectLow, DbValidateLow
} DbState;

typedef enum {
	Low, High
} SwitchStatus;

typedef enum {
	AlowBlow, AhighBlow, AlowBhigh, AhighBhigh, Initialize
} EncoderState;

// This structure should encapsulate all of the information needed for
// a switch.  As such, You will NEED TO insert more definitions in this struct.
// For example, you should include values for your High/Low threshold delay times.
typedef struct {
	DbState CurrentState;			// Current state of the FSM
	char * SwitchPort;				// Input port associated with switch
	unsigned char SwitchPortBit;	// Port pin associated with switch
	unsigned int TimeOut;			// Time when button is pressed
	unsigned int LowThreshold;		// Low Threshold delay
	unsigned int HighThreshold;		// High Threshold delay
	SwitchStatus Status;			// Low (1) or High (0) input
} SwitchDefine;

//Structure for encoder information
typedef struct {
	SwitchDefine * SwitchA;
	SwitchDefine * SwitchB;
	SwitchDefine * Reset;
	EncoderState CurrState;
} EncoderDefine;

// Function Prototypes
void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit, int High, int Low);

void InitializeEncoder(EncoderDefine *Encoder, SwitchDefine *SwitchA, SwitchDefine *SwitchB, SwitchDefine *Reset);

//This function returns the instantaneous value of the selected switch
SwitchStatus GetSwitch(SwitchDefine *Switch);

//Function for the Rotary encoder FSM
EncoderState RotaryEncoder(EncoderDefine *myRotaryEncoder);

//This function debounces a switch input
SwitchStatus Debouncer(SwitchDefine *Switch);

#endif
