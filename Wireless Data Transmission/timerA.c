#include "timerA.h"

extern TransmitterData Xmit1;

void ConfigureTimerA(void)
{
	Timer0_A3_initial();
	Timer1_A3_initial();

	// Used for Timer1 A capture of rising/falling edges of received data
	SELECT_TIMER1_A_CCISx;
}

// Interrupt Handlers
#pragma vector=TIMER0_A0_VECTOR
__interrupt void periodicTimerA0Interrupt(void)
{
   /* Capture Compare Register 0 ISR Hook Function Name */
   if (TRANSMIT_RECEIVE_MODE) {
	   SET_TR_LOW;
	   Xmit(&Xmit1);
   }
   else {
	   SET_TR_HIGH;
   }
   /* No change in operating mode on exit */
}


/*
 *  ======== Timer1_A3 Interrupt Service Routine ========
 */
#pragma vector=TIMER1_A0_VECTOR
__interrupt void timerCaptureRisingInterrupt(void)
{
	/* Capture Compare Register 0 ISR Hook Function Name */
	InsertEvent(Rising, TA1CCR0) ;   //Insert this event into event Queue

	/* No change in operating mode on exit */
}

/*
 *  ======== Timer1_A3 Interrupt Service Routine ========
 */
#pragma vector=TIMER1_A1_VECTOR
__interrupt void timerCaptureFallingInterrupt(void)
{
    switch (__even_in_range(TA1IV, TA1IV_TAIFG))    // Efficient switch-implementation
    {
        case TA1IV_TACCR1:
            /* Capture Compare Register 1 ISR Hook Function Name */
        InsertEvent(Falling, TA1CCR1) ; //Insert this event into event Queue.
            /* No change in operating mode on exit */
            break;
        case TA1IV_TACCR2:
            break;
        case TA1IV_TAIFG:
            break;
    }
}

/*
 *  ======== Timer0_A3_init ========
 *  Initialize MSP430 Timer0_A3 timer
 */
void Timer0_A3_initial(void)
{
    /*
        * TA0CCTL0, Capture/Compare Control Register 0
        *
        * CM_0 -- No Capture
        * CCIS_0 -- CCIxA
        * ~SCS -- Asynchronous Capture
        * ~SCCI -- Latched capture signal (read)
        * ~CAP -- Compare mode
        * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
        *
        * Note: ~<BIT> indicates that <BIT> has value zero
        */
       TA0CCTL0 = CM_0 + CCIS_0 + OUTMOD_0 + CCIE;

       /* TA0CCR0, Timer_A Capture/Compare Register 0 */
       TA0CCR0 = TACCR0_VALUE ;

       /*
        * TA0CTL, Timer_A3 Control Register
        *
        * TASSEL_2 -- SMCLK
        * ID_3 -- Divider - /8
        * MC_1 -- Up Mode
        */
       TA0CTL = TASSEL_2 + ID_3 + MC_1;
}

/*
 *  ======== Timer1_A3_init ========
 *  Initialize MSP430 Timer1_A3 timer
 */
void Timer1_A3_initial(void)
{
   /*
        * TA1CCTL0, Capture/Compare Control Register 0
        *
        * CM_1 -- Rising Edge
        * CCIS_0 -- CCIxA
        * SCS -- Sychronous Capture
        * ~SCCI -- Latched capture signal (read)
        * CAP -- Capture mode
        * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
        *
        * Note: ~SCCI indicates that SCCI has value zero
        */
       TA1CCTL0 = CM_1 + CCIS_0 + SCS + CAP + OUTMOD_0 + CCIE;

       /*
        * TA1CCTL1, Capture/Compare Control Register 1
        *
        * CM_2 -- Falling Edge
        * CCIS_0 -- CCIxA
        * SCS -- Sychronous Capture
        * ~SCCI -- Latched capture signal (read)
        * CAP -- Capture mode
        * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
        *
        * Note: ~SCCI indicates that SCCI has value zero
        */
       TA1CCTL1 = CM_2 + CCIS_0 + SCS + CAP + OUTMOD_0 + CCIE;

       /*
        * TA1CCTL2, Capture/Compare Control Register 2
        *
        * CM_2 -- Falling Edge
        * CCIS_0 -- CCIxA
        * SCS -- Sychronous Capture
        * ~SCCI -- Latched capture signal (read)
        * CAP -- Capture mode
        * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
        *
        * Note: ~SCCI indicates that SCCI has value zero
        */
       TA1CCTL2 = CM_2 + CCIS_0 + SCS + CAP + OUTMOD_0;

       /*
        * TA1CTL, Timer_A3 Control Register
        *
        * TASSEL_2 -- SMCLK
        * ID_3 -- Divider - /8
        * MC_2 -- Continuous Mode
        */
       TA1CTL = TASSEL_2 + ID_3 + MC_2;
}

