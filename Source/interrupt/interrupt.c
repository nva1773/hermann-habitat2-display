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

#pragma code low_vector=0x18
void interrupt_low_vector(void)
{
    _asm goto isrLow _endasm
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
		temp = RCREG;
		TimeOut = 0;
		mLinkLED = TRUE;
		//
		if(!fReceiveStart)
		{
				fReceiveStart = true;
				mTransmit = TRUE;
				mStopTimer0();
				mTransmitTimer0();//-> 3.21 ms
				mStartTimer0();
		}
		else if(ReceiveNum < MAX_NUM_RECEIVE)
		{
			DataReceive[ReceiveNum] = temp;
			ReceiveNum++;
			fReceiveNext = true;
		}
	}//end receive
}
#pragma code

/******************************************************************************
 * Function:        void isrLow(void)
 *****************************************************************************/
#pragma interruptlow isrLow
void isrLow(void)
{
	// Timer0 - 3,21ms for UART or 2ms for LCD and Key
	if(mInterruptTimer0)
	{
		mLoadTimer0();
		// Receive data
		mDisableInterrupt();
		if(fReceiveStart)
		{
			if(mTransmit)
			{
				mTransmit = FALSE;
				mLinkLED = FALSE;
				ReceiveNum = 0;
				ClearLCD();
			}
			else if(fReceiveNext)
			{
				TimeOut++;
				if(TimeOut == 7)
				{
					fReceiveNext = false;
					if(ReceiveNum == MAX_NUM_RECEIVE) fReceiveSuccess = true;
					ReceiveNum = 0;
					TimeOut = 0;
					mLinkLED = FALSE;
				}
			}
		}
		mEnableInterrupt();
		// Update LCD
		fUpdateLCD = true;
	}// end Timer0
}
#pragma code

/** EOF interrupt.c **********************************************************/
