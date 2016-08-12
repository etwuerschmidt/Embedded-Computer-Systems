#ifndef FSM_DEBOUNCE_H_
#define FSM_DEBOUNCE_H_

#include <msp430.h>

// Definitions for debounce Times for the P1.3 button (ms)
#define HIGH_THRESHOLD 10
#define LOW_THRESHOLD 20

// Type Definitions
typedef enum {
	DbExpectHigh, DbValidateHigh, DbExpectLow, DbValidateLow
} DbState;

typedef enum {
	Low, High
} SwitchStatus;

extern int move;
extern int next;
extern int led[7];
extern unsigned int measurement[3];
extern unsigned int calibX[6];
extern unsigned int calibY[6];
extern unsigned int calibZ[6];

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

// Function Prototypes
void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit, int High, int Low);

//This function returns the instantaneous value of the selected switch
SwitchStatus GetSwitch(SwitchDefine *Switch);

//This function debounces a switch input
SwitchStatus Debouncer(SwitchDefine *Switch);

#endif
