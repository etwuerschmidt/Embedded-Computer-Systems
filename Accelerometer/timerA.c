#include <msp430.h>
#include "timerA.h"
#include "LED.h"
#include "debounce.h"
#include "init.h"

void ManageSoftwareTimers(void)
{
	if (g1mSTimeout != 0) { //Increment the 1 ms timer
		g1mSTimeout--; //Adjust so g1msTimeout
		g1msTimer++;
	}

}
void ConfigureTimerA(void)
{
	TA0CCR0 = 1000; //Interrupt thrown every millisecond
	TA0CTL = (TASSEL_2 | ID_0 | MC_1 | TACLR );
	TA0CCTL0 = (CCIE);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
	g1mSTimeout++; //Increment g1msTimeout
	//TACCTL0 &= ~(CCIFG);
}

