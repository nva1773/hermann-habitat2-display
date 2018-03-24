// Display for Hermann Hobitat2 on PIC18F242 used 8MHz external rezonator
// - Timer0 initialization on 500Hz used interupt low
// - LCD with IMMERGAS EOLO NIKE (3COM õ 8SEG = 24 symbol)
// - UART recivie data used interrupt high
//	 recivie first byte - responce Log1 duration 3.21ms,
//   recive frame 8 bytes, pause 25ms.
/** I N C L U D E S **********************************************************/
#include <p18f242.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\cpucfg.h"			// IO configuration
#include "interrupt\interrupt.h"	// Interrupts
#include "user\user.h"				// User function

/** C O N F I G U R A T I O N   B I T S **************************************/ 
#pragma config OSC = HS, OSCS = OFF
#pragma config PWRT = ON
#pragma config BOR = ON, BORV = 42
#pragma config WDT = OFF
#pragma config CCP2MUX = ON
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
	// Reset ports - all inputs, delay 70ms 
	mResetPorts();
	mInitTransmit();
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
			ServiceUART();			
			//
			mEnableInterrupt();
		}
		
		// Update LCD
		if(fUpdateLCD)
		{
			fUpdateLCD = false;
			ServiceFlash();
			UpdateLCD();
		}
	}//end while
}

/******************************************************************************
 * Function:        static void Initialize(void)
 *****************************************************************************/
void Initialize(void)
{
 // Interrupt -  See interrupt.h
    mPriorityInterrupt(TRUE);		
    // Flags
    Flags.bits = 0;
    // Ports - See cpucfg.h
    mInitLCD();
    mInitLinkLED();
    // Timer0
    mInitTimer0();
    mLoadTimer0();//-> 2 ms
	mStartTimer0();
	// UART
   	mInitReceive();
	// Variable
    InitValue();
    // Enagle interrupt
    mEnableInterrupt();
}
