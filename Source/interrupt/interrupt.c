/******************************************************************************
* interrupt.c
******************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include "system\cpucfg.h"
#include "interrupt\interrupt.h"
#include "user\user.h"

/** V A R I A B L E S ********************************************************/
extern BYTE Flags;
extern byte DataReceive[MAX_NUM_RECEIVE];
extern byte ReceiveNum;
//
byte TimeOut;

/** I N T E R R U P T  V E C T O R S *****************************************/
#pragma code high_vector=0x08
void interrupt_high_vector(void)
{
    _asm goto isrHigh _endasm
}
#pragma code

/** D E C L A R A T I O N S **************************************************/
/******************************************************************************
 * Function:        void isrHigh(void)
 *****************************************************************************/
#pragma interrupt isrHigh
void isrHigh(void)
{
	unsigned char temp;
	
	// UART Receive Interrupt
	if(mInterruptReceive)
	{
		mFlagReceive = false;
		temp = mReadUSART();
		TimeOut = 0;
		// First byte
		if(!fReceiveStart)
		{
			fReceiveStart = true;
			// used function
		}
		// Next byte
		if(ReceiveNum < MAX_NUM_RECEIVE)
		{
			DataReceive[ReceiveNum] = temp;
			ReceiveNum++;
			fReceiveNext = true;
		}
	}//end receive
	
	// Timer0 - 2ms for LED
	if(mInterruptTimer0)
	{
		mLoadTimer0();
		// Receive data
		if(fReceiveStart && fReceiveNext)
		{
			TimeOut++; // == 14ms
			if(TimeOut == 7)
			{
				fReceiveNext = false;
				TimeOut = 0;
				if(ReceiveNum == MAX_NUM_RECEIVE)
					fReceiveSuccess = true;
				else
					ClearData();
				ReceiveNum = 0;
			}
		}
		// Update LED
		fUpdateLED = true;
	}// end Timer0
}
#pragma code

/** EOF interrupt.c **********************************************************/
