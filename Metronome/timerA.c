#include <msp430.h>
#include "timerA.h"
#include "LED.h"
#include "debounce.h"

int toggle;

void delay_ms(unsigned int ms )
{
    unsigned int i;
    for (i = 0; i<= ms; i++)
       __delay_cycles(500); //Built-in function that suspends the execution for 500 cicles
}

void delay_us(unsigned int us )
{
    unsigned int i;
    for (i = 0; i<= us/2; i++)
       __delay_cycles(1);
}

//This function generates the square wave that makes the piezo speaker sound at a determinated frequency.
void beep(unsigned int note, unsigned int duration)
{
    int i;
    long delay = (long)(10000/note);  //This is the semiperiod of each note.
    long time = (long)((duration*100)/(delay*2));  //This is how much time we need to spend on the note.
    for (i=0;i<time;i++)
    {
        P1OUT |= BIT2;     //Set P1.2...
        delay_us(delay);   //...for a semiperiod...
        P1OUT &= ~BIT2;    //...then reset it...
        delay_us(delay);   //...for the other semiperiod.
    }
    delay_ms(20); //Add a little delay to separate the single notes
}

void ManageSoftwareTimers(void)
{
	bps = 60.0 / (float) bpm;
	toggle = 1000 * bps;
	if (g1mSTimeout != 0) { //Increment the 1 ms timer
		g1mSTimeout--; //Adjust so g1msTimeout
		g1msTimer++;
	}

	if (g1msTimer == toggle) {
		P1OUT ^= BIT0;
		beep(440, 50);
		__delay_cycles(10000);
		P1OUT ^= BIT0;
		g1msTimer = 0;
	}

}

void TapTimer(void) {
	if (g1mSTimeout2 != 0) { //Increment the 1 ms timer
		g1mSTimeout2--; //Adjust so g1msTimeout
		g1msTimer2++;
	}
}

void ConfigureTimerA(void)
{
	TA1CCR0 = 1000; //Interrupt thrown every millisecond
	TA1CTL = (TASSEL_2 | ID_0 | MC_1 | TACLR );
	TA1CCTL0 = (CCIE);

	TA0CCR0 = 1000;
	TA0CTL = (TASSEL_2 | ID_0 | MC_1 | TACLR );
	TA0CCTL0 = (CCIE);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
	g1mSTimeout++; //Increment g1msTimeout
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
	g1mSTimeout2++; //Increment g1msTimeout
}





