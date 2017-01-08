#include "transmit.h"

int interword = 0;
int data = 0;

void InitializeTransmit(void)
{
	SET_TR_AS_AN_OUTPUT;
	SET_TX_DATA_AS_AN_OUTPUT;
}

//This routine manages the actual transmitter and is called every 500uS by a periodic interrupt.
void Xmit(TransmitterData* TData)
{
	enum XmitClockPhase Phase;

	//Toggles the clock every cycle
	Phase = TData->Transmit_Clock_Phase;
	if (TData->Transmit_Clock_Phase == Low) {
		TData->Transmit_Clock_Phase = High;
	}
	else {
		TData->Transmit_Clock_Phase = Low;
	}
	Phase = TData->Transmit_Clock_Phase;
	P2OUT ^= BIT4; //Toggles clock on MSP to be seen on the oscilloscope

	// Now, begin the transmitter state machine.
	switch(TData->Transmitter_State) {

		case StartBit :
			//Send a 1 followed by a 0, using Manchester Option A
			TData->Bits_Remaining = BITS_IN_TRANSMISSION;
			switch(Phase) {
				case Low :
					P2OUT &= ~BIT5;
					SET_TX_DATA_HIGH; //If low, set TX_DATA high
					TData->Transmitter_State = NormalXmit; //Switch to normal transmission
				break ;
				case High :
					P2OUT |= BIT5;
					SET_TX_DATA_LOW; //If high, set TX_DATA low
				break ;
			}
		break ;

		case NormalXmit :
			if (TData->Bits_Remaining == 0) { //If there are no more bits to send, begin Interword Delay
				TData->Transmitter_State = InterWord;
			}
			switch(Phase) {
			//Data is transmitted on the low edge of the clock
				case Low :
					//Use XOR of clock and current bit
					data = (TData->Transmit_Data >> TData->Bits_Remaining); //Get the current bit to be transmitted
					data &= 0x01;
					TData->Bits_Remaining -= 1;
					if (data) { // 0 ^ 1 = 1
						P2OUT |= BIT5;
						SET_TX_DATA_HIGH;
					}
					else { // 0 ^ 0 = 0
						P2OUT &= ~BIT5;
						SET_TX_DATA_LOW;
					}
				break ;
			//Transitions can still occur here
				case High :
					//Use negation of current bit
					data = (TData->Transmit_Data >> TData->Bits_Remaining); //Get the current bit to be transmitted
					data &= 0x01;
					if (data) { // ~1 = 0
						P2OUT |= BIT5;
						SET_TX_DATA_LOW;
					}
					else { // ~0 = 1
						SET_TX_DATA_HIGH;
						P2OUT &= ~BIT5;
					}
				break ;
			}

		break ;
		case InterWord :
			P2OUT &= ~BIT5;
			SET_TX_DATA_LOW; //Set TX_DATA low for easier scope viewing
			if (interword == INTERWORD_DELAY) { //When counter = 100, 50ms have ellapsed
				TData->Transmitter_State = StartBit; //Begin transmission again
				interword = 0; //Reset counter
			}
			else { //500us * 100 = 50ms
				interword += 1; //Increment the interword counter
			}

		break ;
		default :
			TData->Transmitter_State = StartBit ;
		break ;

	}
}
