// Display for Herman Hobitat2 on PIC18F1220 used 8MHz internal rezonator
// - Timer0 initialization on 500Hz used interupt
// - LED (2COM õ 7SEG = Common Anode)
// - UART recivie data used interrupt
//   recive frame 8 bytes, pause 25ms.
/** I N C L U D E S **********************************************************/
#include <p18f1220.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\cpucfg.h"			// IO configuration
#include "interrupt\interrupt.h"	// Interrupts
#include "user\user.h"				// User function

/** C O N F I G U R A T I O N   B I T S **************************************/ 
#pragma config OSC = INTIO2
#pragma config PWRT = ON
#pragma config BOR = ON, BORV = 42
#pragma config WDT = OFF, MCLRE=OFF
#pragma config LVP = OFF
#pragma config CP0 = OFF
#pragma config CP1 = OFF

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void Initialize(void);

/** V A R I A B L E S ********************************************************/
BYTE Flags;
byte DataReceive[MAX_NUM_RECEIVE];
byte ReceiveNum;

/******************************************************************************
 * Function:        void main(void)
 *****************************************************************************/
void main(void)
{
	// OSC - internal 8MHz, Reset ports - all inputs, delay 70ms 
	mInitOSC();
	mResetPorts();
	Delay1KTCYx(140);//70ms
		
	// Init CPU and Variable
	Initialize();
	
	// Main cycle
	while(1)
	{
		// Receive UART
		if(fReceiveSuccess)
		{
			mDisableInterrupt();
			//
			fReceiveSuccess = false;
   			mWriteUSART(chReceive);
   			mWriteUSART(chSpace);
			ServiceUART();			
			//
			mEnableInterrupt();
		}
		
		// Update LED
		if(fUpdateLED)
		{
			mDisableInterrupt();
			//
			fUpdateLED = false;
			ServiceFlash();
			UpdateLED();
			//
			mEnableInterrupt();
		}
	}//end while
}

/******************************************************************************
 * Function:        static void Initialize(void)
 *****************************************************************************/
void Initialize(void)
{
 // Interrupt -  See interrupt.h
    mPriorityInterrupt(false); // Used as PIC16CXXX
    // Flags
    Flags.bits = 0;
    // Display LED - See cpucfg.h
    mInitDisplay();
    // Timer0
    mInitTimer0();
    mLoadTimer0();//-> 2 ms
	mStartTimer0();
	// UART
   	mInitUART();
   	mWriteUSART(chStart);
   	mWriteUSART(chSpace);
	// Variable
    InitValue();
    // Enagle interrupt
    mEnableInterrupt();
}
