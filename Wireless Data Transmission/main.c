/*
 * This code is a skeleton for a Manchester Phase Encoded Transmitter
 * It may transmit any number of bits up to 32. A separate start bit is sent,
 * with a rising edge at mid-bit. After this, normal transmission is sent.
 * This version defines a 1 as a rising edge at mid-bit. Transmission rate is 1 mS per bit.
 * There is an inter-transmission time of 5 mS, then repeat transmission.
 * A 500uS clock tick is interrupt is required.
 */

#include <msp430.h>
#include "configure.h"
#include "timerA.h"
#include "transmit.h"
#include "receive.h"

// Function Prototypes
void InitializeVariables(void);
void InitializeHardware(void);
void ConfigureClockModule(void);

//Global Variables Section
TransmitterData Xmit1 ;  //This declares an instance of the transmitter data structure.
ManchesterReceiver Rcv1 ; //Instance of the receiver data structure
Event_Queue Receiver_Events; //Receiver Global Variables
int one_count = 0; //The number of ones in the bit stream to be transferred
unsigned long parity = 0; //The sequence of bits used to change the first bit in the transferred bit stream
int parity_bit = 0; //1 or 0, depending on one_count
int data2 = 0; //Current bit in bit sequence to be transmitted
unsigned long last_receive; //Last bit stream received by Rcv1

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
    ConfigureClockModule();

    InitializeHardware();
	InitializeVariables();

	//Calculates the parity of the transmitted bit stream
	while (Xmit1.Bits_Remaining != 0) {
		data2 = (Xmit1.Transmit_Data >> Xmit1.Bits_Remaining - 1); //Grab individual bits starting with the second (first will be parity bit)
		data2 &= 0x01;
		Xmit1.Bits_Remaining -= 1; //Subtract from bits remaining

		if (data2) { //If the current bit is a 1, add one to one_count
			one_count+=1;
		}
	}

	//ODD PARITY
	if (one_count % 2 == 0) { //If one_count is even, PARITY 1
		parity = 0x80000000;
		parity_bit = 1; //Set parity bit to 1
		Xmit1.Transmit_Data_Buffer = parity | Xmit1.Transmit_Data_Buffer; //Set the first bit of the data buffer to 1
		Xmit1.Transmit_Data = parity | Xmit1.Transmit_Data; //Set the first bit of the bit stream to be transmitted to 1
	}
	else { //If one_count is odd, PARITY 0
		parity = 0x7FFFFFFF;
		parity_bit = 0; //Set parity bit to 0
		Xmit1.Transmit_Data_Buffer = parity & Xmit1.Transmit_Data_Buffer; //Set the first bit of the data buffer to 0
		Xmit1.Transmit_Data = parity & Xmit1.Transmit_Data; //Set the first bit of the bit stream to transmitted to 0
	}

	Xmit1.Bits_Remaining = BITS_IN_TRANSMISSION;


	_BIS_SR(GIE);		// interrupts enabled

	while (1) {
		if (TRANSMIT_RECEIVE_MODE == RECEIVE) {
			rcv();
			last_receive = Rcv1.LastValidReceived;
		}
	}
}

//Set up globals, including the transmitter structure.
//Comment Well
void InitializeVariables(void){
	//Here is an example. This is just sample data, the final application determines what is to be sent.
		Xmit1.Bits_Remaining = BITS_IN_TRANSMISSION ;
		Xmit1.Transmit_Data_Buffer = 0x4D3C2B1A;
		Xmit1.Transmit_Data = 0x4D3C2B1A   ;
		Xmit1.Transmit_Clock_Phase = Low ;
		Xmit1.Transmitter_State = StartBit ;
		Xmit1.InterwordTimeout = INTERWORD_DELAY ;
	//etc. .....
		Receiver_Events.Get_Index = 0 ;
		Receiver_Events.Put_index = 0 ;
		Receiver_Events.QueSize   = 0 ;

	//etc.........
		Rcv1.CurrentRcvState = Initial_Expect_Rising ;
		Rcv1.CurrentRecvdData =      0 ;
		Rcv1.FallingEdgeTimeStamp =  0 ;
		Rcv1.RisingEdgeTimeStamp =   0 ;
		Rcv1.MidBitTimeStamp =       0 ;
		Rcv1.PulseWidth =            0 ;
		Rcv1.CurrentRecvdData =      0 ;
		Rcv1.LastValidReceived =     0 ;

}

void InitializeHardware(void)
{
	ConfigureTimerA();
	InitializeTransmit();
	InitializeReceive();
	P2DIR |= BIT4;
	P2DIR |= BIT5;
	SET_TR_AS_AN_OUTPUT;
}
/*
 *  ======== BCSplus_init ========
 *  Initialize MSP430 Basic Clock System
 */
void ConfigureClockModule(void)
{
    /*
     * Basic Clock System Control 2
     *
     * SELM_0 -- DCOCLK
     * DIVM_0 -- Divide by 1
     * ~SELS -- DCOCLK
     * DIVS_0 -- Divide by 1
     * ~DCOR -- DCO uses internal resistor
     *
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;

    if (CALBC1_8MHZ != 0xFF) {
        /* Adjust this accordingly to your VCC rise time */
        __delay_cycles(100000);

        // Follow recommended flow. First, clear all DCOx and MODx bits. Then
        // apply new RSELx values. Finally, apply new DCOx and MODx bit values.
        DCOCTL = 0x00;
        BCSCTL1 = CALBC1_8MHZ;      /* Set DCO to 8MHz */
        DCOCTL = CALDCO_8MHZ;
    }

    /*
     * Basic Clock System Control 1
     *
     * XT2OFF -- Disable XT2CLK
     * ~XTS -- Low Frequency
     * DIVA_0 -- Divide by 1
     *
     * Note: ~XTS indicates that XTS has value zero
     */
    BCSCTL1 |= XT2OFF + DIVA_0;

    /*
     * Basic Clock System Control 3
     *
     * XT2S_0 -- 0.4 - 1 MHz
     * LFXT1S_2 -- If XTS = 0, XT1 = VLOCLK ; If XTS = 1, XT1 = 3 - 16-MHz crystal or resonator
     * XCAP_1 -- ~6 pF
     */
    BCSCTL3 = XT2S_0 + LFXT1S_2 + XCAP_1;
}

