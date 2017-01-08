#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include <msp430.h>

// Used to select whether to configure as a transmitter or receiver
#define TRANSMIT 1
#define RECEIVE  0
#define TRANSMIT_RECEIVE_MODE TRANSMIT

// Define constants
#define BITS_IN_TRANSMISSION  32
#define INTERWORD_DELAY       100	// This is in units of 0.5mS
#define SIZE_OF_RCV_QUE        4    // Must be a power of 2!

// The following defines are for bit width definitions
// The underlying assumptions are:
//	- The capture timers are updated with a 1uS resolution
//	- A full bit time is 1 mS nominal
#define VALID_HALF_BIT_MIN    450
#define VALID_HALF_BIT_MAX    550
#define VALID_FULL_BIT_MIN    950
#define VALID_FULL_BIT_MAX   1050
#define MISSING_EDGE_TIMEOUT 1200

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   T/R (on schematic, CNTRL1 in TR1000 datasheet)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   GPIO      :  P1.6
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define TR_BIT				BIT6
#define TR_PORT				P1OUT
#define TR_DIR				P1DIR
#define SET_TR_AS_AN_OUTPUT	TR_DIR |= TR_BIT
#define SET_TR_HIGH			TR_PORT |= TR_BIT
#define SET_TR_LOW			TR_PORT &= ~TR_BIT

#endif /* CONFIGURE_H_ */
