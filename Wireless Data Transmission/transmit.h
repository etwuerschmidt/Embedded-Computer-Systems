#ifndef TRANSMIT_H_
#define TRANSMIT_H_

#include <msp430.h>
#include "configure.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   TX-DATA (on schematic, TXMOD in TR1000 datasheet)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   GPIO      :  P1.5
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define TX_DATA_BIT					BIT5
#define TX_DATA_PORT				P1OUT
#define TX_DATA_DIR					P1DIR
#define SET_TX_DATA_AS_AN_OUTPUT	TX_DATA_DIR |= TX_DATA_BIT
#define SET_TX_DATA_HIGH			TX_DATA_PORT |= TX_DATA_BIT
#define SET_TX_DATA_LOW				TX_DATA_PORT &= ~TX_DATA_BIT

// Typedef Section
enum Transmit_States {StartBit,NormalXmit,InterWord} ;
enum XmitClockPhase  {High,Low} ;
typedef struct {
   unsigned long        Transmit_Data         ;  //This is the data to actually be transmitted
   unsigned long        Transmit_Data_Buffer  ;  //This should be reloaded any time we wish to change what is transmitted.
   unsigned int         Bits_Remaining        ;  //This is the number of bits remaining in the current transmission
   enum XmitClockPhase  Transmit_Clock_Phase  ;  //This gets updated once every 1/2 bit period (500 uS in this case.)
   unsigned int         InterwordTimeout      ;  //This represents a "dead" period between successive transmissions
   enum Transmit_States Transmitter_State     ;  //This is the current state machine state for the transmitter
} TransmitterData ;

void InitializeTransmit(void);
void Xmit(TransmitterData * TData) ; //This routine is called every 500 uS by an interrupt handler.

#endif /* TRANSMIT_H_ */
