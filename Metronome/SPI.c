#include "SPI.h"
#include "debounce.h"
void SPISendByte(unsigned char byte_value)
{
	short k;

	// Shift the bits, beginning with the MSB
	for (k = 7; k >= 0; k--){

		// Right-shift byte_value k places so that the LSB
		// contains the kth bit. Then, mask off the LSB
		// and assign it to the MOSI port pin.
		if ((byte_value >> k) & 0x01) {
			P1OUT |= BIT7;
		}
		else {
			P1OUT &= ~BIT7;
		}

		// Toggle SPI Clock: (HIGH XOR 1) -> LOW, and (LOW XOR 1) -> HIGH
		P1OUT |= SLCK; P1OUT &= ~SLCK;
	}

}

