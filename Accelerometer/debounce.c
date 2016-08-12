#include "debounce.h"
#include "timerA.h"
#include "LED.h"
#include "SPI.h"
#include "DTC.h"

void InitializeSwitch(SwitchDefine *Switch,char *SwitchPort,unsigned char SwitchBit, int HighThresh, int LowThresh)
{
	Switch->CurrentState = DbExpectHigh;
	Switch->SwitchPort = SwitchPort;
	Switch->SwitchPortBit = SwitchBit;
	Switch->TimeOut = 0;
	Switch->HighThreshold = HighThresh;
	Switch->LowThreshold = LowThresh;
	Switch->Status = Low;
}

SwitchStatus GetSwitch(SwitchDefine *Switch)
{
	if (*Switch->SwitchPort & Switch->SwitchPortBit) { // Returns high if a switch is pushed
		return High;
	}
	else {
		return Low;
	}
}

SwitchStatus Debouncer(SwitchDefine *Switch) //Implementation of the debouncer FSM
{
	int TimeDiff;
	//SPISendByte(led[next]);

	SwitchStatus CurrentSwitchReading = GetSwitch(Switch);
	Switch->Status = CurrentSwitchReading;
	// Next, based on the input values and the current state, determine the next state.
	switch (Switch->CurrentState) {
		case DbExpectHigh:
			if (CurrentSwitchReading == Low) {
				Switch->TimeOut = g1msTimer; //Get timestamp for comparison
				Switch->CurrentState = DbValidateHigh;
			}
		break;
		case DbValidateHigh:
			TimeDiff = g1msTimer - Switch->TimeOut;
			if (TimeDiff < 0) { // If the time difference is negative, flip the bits to obtain the correct positive amount
				TimeDiff = ~TimeDiff;
			}

			if (CurrentSwitchReading == High) {
				Switch->CurrentState = DbExpectHigh;
			}
			else if ((CurrentSwitchReading == Low) & (TimeDiff >= Switch->HighThreshold)) { //If the time difference is within the threshold and the switch is low
				Switch->CurrentState = DbExpectLow;
			}
		break;
		case DbExpectLow:
			if (CurrentSwitchReading == High) {
				Switch->TimeOut = g1msTimer; //Get timestamp for comparison
				Switch->CurrentState = DbValidateLow;
			}
		break;
		case DbValidateLow:
			TimeDiff = g1msTimer - Switch->TimeOut;
			if (TimeDiff < 0) { //If the time difference is negative, flip the bits to obtain the correct positive amount
				TimeDiff = ~TimeDiff;
			}
			if (CurrentSwitchReading == Low) {
				Switch->CurrentState = DbExpectLow;

			}
			else if ((CurrentSwitchReading == High) & (TimeDiff >= Switch->LowThreshold)) { //If the time difference is within the threshold and the switch is high
				Switch->CurrentState = DbExpectHigh;
				DTC();
				move = 1;
			}
		break;
		default: Switch->CurrentState = DbExpectHigh;
	}

	return CurrentSwitchReading;
}

