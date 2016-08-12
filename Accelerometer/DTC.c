
/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430G2x33/G2x53 Demo - ADC10, DTC Sample A1-0 16x, AVcc, Repeat Seq, DCO
//
//  Description: Use DTC to sample A1/A0 repeat sequence 16x(32 total samples)
//  with reference to AVcc.  Software sets ADC10SC to trigger sample burst.
//  In Mainloop MSP430 waits in LPM0 to save power until ADC10 conversion
//  complete, ADC10_ISR will force exit from any LPMx in Mainloop on reti.
//  ADC10 internal oscillator times sample period (16x) and conversion (13x).
//  DTC transfers conversion code to RAM 200h - 240h.  ADC10(DTC) interrupt
//  will return system active. P1.0 set at start of conversion burst, reset
//  on completion.
//
//               MSP430G2x33/G2x53
//            -----------------
//        /|\|              XIN|-
//         | |                 |
//         --|RST          XOUT|-
//           |                 |
//       >---|P1.1/A1          |
//       >---|P1.0/A0          |
//
//  D. Dang
//  Texas Instruments Inc.
//  December 2010
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
//5TAU = CAPACITOR IS FULLY CHARGED -> 64 ADC CLOCK CYCLES
#include <msp430.h>
#include "DTC.h"
#include "init.h"
#include "Cordic.h"
unsigned int measurement[3];
unsigned int x[8];
unsigned int y[8];
unsigned int z[8];
unsigned int averageX = 0;
unsigned int averageY = 0;
unsigned int averageZ = 0;
int finalX = 0;
int finalY = 0;
int finalZ = 0;
unsigned int CumulativeSumX = 0;
unsigned int CumulativeSumY = 0;
unsigned int CumulativeSumZ = 0;
unsigned int i = 0;
int n = 0;
int check = 0;
int b = 0;
int first_val = 0;
measurements result;

unsigned int filter(unsigned int sum)
{
	sum = sum >> 3;
	return sum;
}

void DTC(void)
{
  ADC10CTL1 = INCH_2 + CONSEQ_3;            // A2/A1/A0, repeat multi channel
  ADC10CTL0 = ADC10SHT_3 + MSC + ADC10ON + ADC10CT;
  ADC10AE0 = (BIT0 + BIT1 + BIT2);          // P1.0,1, 2 Analog enable
  ADC10DTC1 = 0x03;                         // number of conversions= 24??

  if (cal) {
	  ADC10CTL0 &= ~ENC;
	  while (ADC10CTL1 & BUSY);               // Wait if ADC10 core is active
	  ADC10SA = (int) &(measurement);        // Data buffer start
	  ADC10CTL0 |= ENC + ADC10SC;  			// Sampling and conversion ready
  }

  else {
	  for (;;)
	  {
		  ADC10CTL0 &= ~ENC;
		  while (ADC10CTL1 & BUSY);               // Wait if ADC10 core is active
		  ADC10SA = (int) &(measurement);        // Data buffer start
		  ADC10CTL0 |= ENC + ADC10SC;  			// Sampling and conversion ready

		  CumulativeSumX -= x[i];
		  CumulativeSumY -= y[i];
		  CumulativeSumZ -= z[i];
		  x[i] = measurement[2];
		  y[i] = measurement[1];
		  z[i] = measurement[0];
		  CumulativeSumX += x[i];
		  CumulativeSumY += y[i];
		  CumulativeSumZ += z[i];
		  i = (i + 1) & 7;
		  if (i == 0) {
			  //CALL CORDIC HERE
			  averageX = filter(CumulativeSumX);
			  finalX = averageX - XCent;
			  averageY = filter(CumulativeSumY);
			  finalY = averageY - YCent;
			  averageZ = filter(CumulativeSumZ);
			  finalZ = averageZ - ZCent; //FLAT BETWEEN 50 to 60 AND -50 to -60
			  coordinates p;
			  p.x = finalX;
			  p.y = finalY;
			  p.z = finalZ;
			  result = getDisplaySetting(&p); //return angles
			 // toggleBrightness(4, 20);
			  //varyBrightness();

			  //if statements to determine which LEDs to turn on

			  if ((averageZ <= ZMax + 5) && (averageZ >= ZMax - 5)) {
				  InitializeLEDs();
				  toggleBrightness(0, 10);
				  toggleBrightness(1, 10);
				  toggleBrightness(2, 10);
				  toggleBrightness(3, 10);
				  toggleBrightness(4, 10);
				  toggleBrightness(5, 10);
				  toggleBrightness(6, 10);
				  toggleBrightness(7, 10);
				  varyBrightness();
			  }

			  //quadrant 1
			  else if ((result.angle) < 9000 && (result.angle >= 4500)) {
				  InitializeLEDs();
				  toggleBrightness(0, 5);
				  toggleBrightness(1, 20);
				  toggleBrightness(2, 5);
				  toggleBrightness(3, 1);
				  toggleBrightness(7, 1);
				  varyBrightness();
			  }

			  //quadrant 2
			  else if (result.angle < 31000 && result.angle >= 28000) {
				  InitializeLEDs();
				  toggleBrightness(0, 5);
				  toggleBrightness(1, 1);
				  toggleBrightness(6, 5);
				  toggleBrightness(5, 1);
				  toggleBrightness(7, 20);
				  varyBrightness();
			  }

			  //in between quadrant 1 and 2
			  else if (result.angle >= 31000 || result.angle <= 4500) {
				  InitializeLEDs();
				  toggleBrightness(0, 20);
				  toggleBrightness(1, 5);
				  toggleBrightness(7, 5);
				  toggleBrightness(2, 1);
				  toggleBrightness(6, 1);
				  varyBrightness();
			  }

			  //quadrant 3
			  else if (result.angle < 24000 && result.angle >= 21000) {
				  InitializeLEDs();
				  toggleBrightness(5, 20);
				  toggleBrightness(4, 5);
				  toggleBrightness(3, 1);
				  toggleBrightness(6, 5);
				  toggleBrightness(7, 1);
				  varyBrightness();
			  }
			  //quadrant 4
			  else if (result.angle < 18000 && result.angle >= 14000) {
				  InitializeLEDs();
				  toggleBrightness(3, 20);
				  toggleBrightness(4, 5);
				  toggleBrightness(2, 5);
				  toggleBrightness(1, 1);
				  toggleBrightness(5, 1);
				  varyBrightness();
			  }

			  //in between quadrant 2 and 3
			  else if (result.angle >= 24000 && result.angle < 28000) {
				  InitializeLEDs();
				  toggleBrightness(6, 20);
				  toggleBrightness(5, 5);
				  toggleBrightness(7, 5);
				  toggleBrightness(4, 1);
				  toggleBrightness(0, 1);
				  varyBrightness();
			  }

			  //in between quadrant 3 and 4
			  else if (result.angle >= 18000 && result.angle < 21000) {
				  InitializeLEDs();
				  toggleBrightness(4, 20);
				  toggleBrightness(3, 5);
				  toggleBrightness(5, 5);
				  toggleBrightness(2, 1);
				  toggleBrightness(6, 1);
				  varyBrightness();
			  }

			  //in between quadrant 1 and 4
			  else if (result.angle >= 9000 && result.angle < 14000) {
				  InitializeLEDs();
				  toggleBrightness(2, 20);
				  toggleBrightness(1, 5);
				  toggleBrightness(3, 5);
				  toggleBrightness(0, 1);
				  toggleBrightness(4, 1);
				  varyBrightness();
			  }
		  }
		  ADC10CTL0 &= ~ADC10SC;
	  }

  }
}

