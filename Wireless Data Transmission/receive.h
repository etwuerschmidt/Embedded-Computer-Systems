#ifndef RECEIVE_H_
#define RECEIVE_H_

#include <msp430.h>
#include "configure.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   RX-DATA
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   P2.0 (Timer1_A3.CCI0A)
 *   P2.1 (Timer1_A3.CCI1A)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define RX_DATA_BITS				(BIT0+BIT1)
#define RX_DATA_PORT				P2IN
#define RX_DATA_DIR					P2DIR
#define SET_RX_DATA_AS_AN_INPUT		RX_DATA_DIR &= ~RX_DATA_BITS
#define RX_DATA_P2SEL				P2SEL
#define RX_DATA_P2SEL2				P2SEL2
#define SELECT_TIMER1_A_CCISx		RX_DATA_P2SEL |= RX_DATA_BITS

//Receiver Definitions and declarations
enum Captured_Edge {Rising,Falling} ;  //these are the 2 types of edges in the received signal
typedef enum Captured_Edge EdgeType ;
typedef struct {
   EdgeType     Edge      ;    // Which type of edge was received
   unsigned int TimeStamp ;    // When we got it.
} Event_Que_Entry ;

typedef struct {
   Event_Que_Entry Events[SIZE_OF_RCV_QUE] ; //What each entry looks like
   unsigned int  QueSize ;                   //Current size of queue
   unsigned int  Get_Index ;                 //Where we get data from
   unsigned int  Put_index ;                 //Where we put new data
} Event_Queue ;

enum Que_Errors {No_Error,Que_Full,Que_Empty} ;
typedef enum Que_Errors QueReturnErrors       ;

//The following typedefs are for the receiver section
typedef enum Rcv_States {Initial_Expect_Rising,Initial_Expect_Falling,MidBit_Expect_Rising,MidBit_Expect_Falling} ReceiverStates ;
typedef struct {
   ReceiverStates  CurrentRcvState      ;   // State for state machine implementation
   unsigned int    RisingEdgeTimeStamp  ;   // Time stamp at leading edge of signal
   unsigned int    FallingEdgeTimeStamp ;   // TIme stamp for falling edge
   unsigned int    PulseWidth           ;   // Difference in time between edges
   unsigned int    MidBitTimeStamp      ;   // Time Stamp of last valid mid-bit transition
   unsigned int    LastEdgeTimeStamp    ;   // When the last edge occured, regardless of polarity
   unsigned long   CurrentRecvdData     ;   // Data that is being shifted in
   unsigned long   LastValidReceived    ;   // Last complete valid word.
   unsigned int    BitsLeftToGet        ;   // Number of bits to go in reception.
}ManchesterReceiver;

typedef enum PulseWidths {Invalid_Width,Valid_HalfBit,Valid_FullBit} PulseWidthStatus ;

extern ManchesterReceiver Rcv1;
extern Event_Queue Receiver_Events;

void InitializeReceive(void);
void rcv(void);

//Receiver Queue functions:
//This function places a new event in the event queue structure.
//The rising and falling edge detectors should call this handler with the appropriate info!
QueReturnErrors InsertEvent(EdgeType DetectedEdge, unsigned int CapturedTime);

//This is called from within the main loop to see if there are any events on the que, i.e. captured edges.
//Note that it disables interrupts to ensure that data is not overwritten by an interrupter
//If no event is pending, it returns a -1, otherwise it returns an index into the queue that corresponds to the oldest
//event.
int GetEvent(void);

//This functions tests a current pulse width and determines if it is a valid width
PulseWidthStatus TestWidth(unsigned int CurrentPulse);

#endif /* RECEIVE_H_ */
