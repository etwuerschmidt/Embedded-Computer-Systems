#include "debounce.h"
#include "timerA.h"
#include "LED.h"
#include "SPI.h"

extern int toggle;
int const led[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71}; //Array of values to be displayed on the 7-segment displays
int Count = 0; //Count that is incremented or decremented in the Encoder FSM
int Prev; //Previous value of Count
int send = 0;
int reset = 0; //False if the pushbutton is not pushed, true if the pushbutton is pushed
int taps = -1; //Number of times pushbutton is tapped
int tap[5] = {0, 0, 0, 0, 0}; //Time tap occured
int starts[5] = {0, 0, 0, 0, 0}; //Time pushbutton was pushed
int avg = 0; //Avg time between button presses
int new_toggle = 0; //New interrupt time for Timer A
float new_bps = 0; //New bps
int new_bpm = 0; //New bpm
int new_i = 0;
int new_j = 0;

int i = 12; //led Array position for the right 7-segment display
int j = 3; //led Array position for the left 7-segment display
int bpm = 0; //controls bpm of metronome

void TapTimer();

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

void InitializeEncoder(EncoderDefine *Encoder, SwitchDefine *SwitchA, SwitchDefine *SwitchB, SwitchDefine *Reset)
{
	Encoder->SwitchA = SwitchA;
	Encoder->SwitchB = SwitchB;
	Encoder->Reset = Reset;
	Encoder->CurrState = Initialize;
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

	SwitchStatus CurrentSwitchReading = GetSwitch(Switch);
	Switch->Status = CurrentSwitchReading;
	// Next, based on the input values and the current state, determine the next state.
	switch (Switch->CurrentState) {
		case DbExpectHigh:
			//TURN_OFF_LED1; 0
			//TURN_OFF_LED2; 0
			if (CurrentSwitchReading == Low) {
				Switch->TimeOut = g1msTimer; //Get timestamp for comparison
				Switch->CurrentState = DbValidateHigh;
			}
		break;
		case DbValidateHigh:
			//TURN_OFF_LED1; 0
			//TURN_ON_LED2; 1
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
			//TURN_ON_LED1; 1
			//TURN_OFF_LED2; 0
			if (CurrentSwitchReading == High) {
				Switch->TimeOut = g1msTimer; //Get timestamp for comparison
				Switch->CurrentState = DbValidateLow;
			}
		break;
		case DbValidateLow:
			//TURN_ON_LED1; 1
			//TURN_ON_LED2; 1
			TimeDiff = g1msTimer - Switch->TimeOut;
			if (TimeDiff < 0) { //If the time difference is negative, flip the bits to obtain the correct positive amount
				TimeDiff = ~TimeDiff;
			}
			if (CurrentSwitchReading == Low) {
				Switch->CurrentState = DbExpectLow;

			}
			else if ((CurrentSwitchReading == High) & (TimeDiff >= Switch->LowThreshold)) { //If the time difference is within the threshold and the switch is high
				Switch->CurrentState = DbExpectHigh;
				taps += 1;
			}
		break;
		default: Switch->CurrentState = DbExpectHigh;
	}

	return CurrentSwitchReading;
}

EncoderState RotaryEncoder(EncoderDefine *myRotaryEncoder) //Implementation of the Rotary Encoder FSM
{
	bpm = (i + 16*j); //Modify bpm based on the value of right and left 7 segment displays
	EncoderState myEncoderState = myRotaryEncoder->CurrState;
	if (taps > 4) { //If taps overflow, reset taps
		taps = -1;
	}
	if (reset) {	//If the reset button is pressed, measure taps
		TapTimer(); //Separate software timer used to measure time between button presses
		starts[taps] = g1msTimer2; //Record time in "starts" array

		if (taps % 4 == 0  && taps != 0) { //If the number of button presses equals 5 (starts at -1), reset values and calculate avg bpm from taps
			taps = -1;
			reset = 0;
			Count = 0;
			Prev = 0;
			int i = 0;
			for (i = 0; i < 5; i++) { //Take the time difference between button presses
				tap[i] = starts[i+1] - starts[i];
			}
			for (i = 0; i < 3; i++) { //Average the time difference
				avg += tap[i];
			}
			avg = avg / 3;
			new_toggle = avg;
			new_bps = new_toggle / 1000.0; //Find the bps from avg time difference
			new_bpm = 60.0 / new_bps; //Find bpm from bps
			new_i = new_bpm % 16; //Light the appropriate LEDs
			new_j = (new_bpm / 16) % 16;
			j = new_j;
			i = new_i;
			SPISendByte(led[i]); //Send the appropriate bytes from the led Array
			SPISendByte(led[j]);
			toggle = new_toggle; //Timer interrupt value equals new time difference between presses

			P1OUT ^= BIT6; //Toggle green LED to let user know that new bpm has been calculated
			__delay_cycles(25000);
			P1OUT ^= BIT6;
			g1msTimer = 0; //Reset master software timer
		}
		avg = 0;
	}

	else if (Count % 4 == 0 & Prev != Count) { //If a correct FSM sequence has been completed (Count % 4 == 0) and the Previous Count doesn't equal the current Count
		if (Prev > Count) { //Encoder turned clockwise
			i++;
		}
		if (Prev < Count) { //Encoder turned counterclockwise
			i--;
		}
		if (i > 15 & j != 15) { //Values rolling over from xF to (x+1)0
			i = 0;
			j++;
		}
		else if (i < 0 & j != 0) { //Values rolling back from x0 to (x-1)F
			i = 15;
			j--;
		}
		else if (i > 15 & j >= 15) { //7 segment display at FF
			i = 15;
			j = 15;
		}
		else if (i < 0 & j <= 0) { //7 segment display at 00
			i = 0;
			j = 0;
		}
		SPISendByte(led[i]); //Send the appropriate bytes from the led Array
		SPISendByte(led[j]);
		Prev = Count; //Set the previous count as the current count for the next iteration
		g1msTimer = 0;
	}

	switch (myEncoderState) {
	case Initialize:
		Count = 0;
		if (Debouncer(myRotaryEncoder->Reset) == Low) {
			reset = 1;
			break;
		}
		if (Debouncer(myRotaryEncoder->SwitchA) == Low & Debouncer(myRotaryEncoder->SwitchB) == Low) {
			myRotaryEncoder->CurrState = AlowBlow;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == Low) {
			myRotaryEncoder->CurrState = AhighBlow;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == Low & Debouncer(myRotaryEncoder->SwitchA) == High) {
			myRotaryEncoder->CurrState = AlowBhigh;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchA) == High) {
			myRotaryEncoder->CurrState = AhighBhigh;
		}
		break;

	case AlowBlow:
		if (Debouncer(myRotaryEncoder->Reset) == Low) {
			reset = 1;
			break;
		}
		if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == High) {
			myRotaryEncoder->CurrState = AlowBlow;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == Low) {
			myRotaryEncoder->CurrState = AhighBlow;
			Count++;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == Low & Debouncer(myRotaryEncoder->SwitchB) == High){
			myRotaryEncoder->CurrState = AlowBhigh;
			Count--;
		}
		else {
			myRotaryEncoder->CurrState = AlowBlow;
		}
		break;

	case AlowBhigh:
		if (Debouncer(myRotaryEncoder->Reset) == Low) {
			reset = 1;
			break;
		}
		if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == High) {
			myRotaryEncoder->CurrState = AhighBhigh;
			Count--;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == Low) {
			myRotaryEncoder->CurrState = AlowBhigh;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == Low & Debouncer(myRotaryEncoder->SwitchB) == High){
			myRotaryEncoder->CurrState = AlowBhigh;
		}
		else {
			myRotaryEncoder->CurrState = AlowBlow;
			Count++;
		}
		break;

	case AhighBlow:
		if (Debouncer(myRotaryEncoder->Reset) == Low) {
			reset = 1;
			break;
		}
		if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == High) {
			myRotaryEncoder->CurrState = AhighBhigh;
			Count++;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == Low) {
			myRotaryEncoder->CurrState = AhighBlow;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == Low & Debouncer(myRotaryEncoder->SwitchB) == High){
			myRotaryEncoder->CurrState = AhighBlow;
		}
		else {
			myRotaryEncoder->CurrState = AlowBlow;
			Count--;
		}
		break;

	case AhighBhigh:
		if (Debouncer(myRotaryEncoder->Reset) == Low) {
			reset = 1;
			break;
		}
		if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == High) {
			myRotaryEncoder->CurrState = AhighBhigh;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == High & Debouncer(myRotaryEncoder->SwitchB) == Low) {
			myRotaryEncoder->CurrState = AhighBlow;
			Count--;
		}
		else if (Debouncer(myRotaryEncoder->SwitchA) == Low & Debouncer(myRotaryEncoder->SwitchB) == High){
			myRotaryEncoder->CurrState = AlowBhigh;
			Count++;
		}
		else {
			myRotaryEncoder->CurrState = AhighBhigh;
		}
		break;

	default:
		myRotaryEncoder->CurrState = Initialize;
	}

	return myRotaryEncoder->CurrState;
}




