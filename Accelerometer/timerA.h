#ifndef TIMERA_H_
#define TIMERA_H_

unsigned int g1mSTimeout;		// This variable is incremented by the interrupt handler and
								// decremented by a software call in the main loop.
unsigned int g1msTimer;			// This variable is incremented in ManageSoftwareTimers



#endif /* TIMERA_H_ */
