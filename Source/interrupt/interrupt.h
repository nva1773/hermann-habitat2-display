/******************************************************************************
* interrupt.h
******************************************************************************/
#ifndef INTERRUPT_H
#define INTERRUPT_H

/** D E F I N I T I O N S ****************************************************/
#define mPriorityInterrupt(a)	(RCONbits.IPEN = a)
#define mEnableInterrupt()		INTCONbits.GIEL = 1; INTCONbits.GIEH = 1;
#define mDisableInterrupt()		INTCONbits.GIEH = 0; INTCONbits.GIEL = 0;

/** P R O T O T Y P E S ******************************************************/
void isrHigh(void);

#endif //INTERRUPT_H




