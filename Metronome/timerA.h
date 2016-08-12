#ifndef TIMERA_H_
#define TIMERA_H_

extern int bpm;
float bps;

unsigned int g1mSTimeout;		// This variable is incremented by the interrupt handler and
								// decremented by a software call in the main loop.
unsigned int g1mSTimeout2;
unsigned int g1msTimer;			// This variable is incremented in ManageSoftwareTimers
unsigned int g1msTimer2;
unsigned int gButtonPresses;	// Number of button presses from all switches
unsigned int g500msTimer;



#endif /* TIMERA_H_ */
