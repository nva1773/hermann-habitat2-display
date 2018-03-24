/*********************************************************************
 * FileName:        user.c
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\cpucfg.h"             // I/O pin mapping
#include "user\user.h"

/** C O N S T A N T S ********************************************************/
const rom char DigitalBCD[]={
	         //HGFEDCBA
			 0b00111111,//0=0x3F
			 0b00000110,//1=0x06
			 0b01011011,//2=0x5B
			 0b01001111,//3=0x4F
			 0b01100110,//4=0x66
			 0b01101101,//5=0x6B
			 0b01111101,//6=0x7D
			 0b00000111,//7=0x07
			 0b01111111,//8=0x7F
			 0b01101111,//9=0x6F
			 0b01000000,//10=0x40 (--) used for OFF
			 0b00000000};//11=0x00 (  ) used for FLASH

/** V A R I A B L E S ********************************************************/
extern BYTE Flags;
extern byte DataReceive[MAX_NUM_RECEIVE];
extern byte ReceiveNum;
//
byte StateLCD, FlashLCD;
BYTE SymbolCom1,SymbolCom2,SymbolCom3;
unsigned char DigitalValue;
unsigned char DigitalLCD[]= {0x00,0x00};
byte SegmentsCom1, SegmentsCom2, SegmentsCom3, CountSymbol;
//
enum stLCD{stBCD,stCom1L,stCom1H,stCom2L,stCom2H,stCom3L,stCom3H,stPause};

/******************************************************************************
 * Function:      unsigned char ReadKEY(void)
 *****************************************************************************/
void InitValue(void)
{
    SymbolCom1.bits = 0;
    SymbolCom2.bits = 0;
    SymbolCom3.bits = 0;
    StateLCD = stBCD;
	FlashLCD = FLASH_NO;
    CountSymbol = 0;
    SegmentsCom1 = 0;
    SegmentsCom2 = 0;
    SegmentsCom3 = 0;
	DigitalValue = DISPLAY_START;
    //
    return;
}	

/******************************************************************************
 * Function:      void BCD(unsigned int data)
 *****************************************************************************/
void BCD(unsigned int data)
{
 	unsigned char temp;
	// Start (-5), 0 - 0x05 = -5
 	if(data == DISPLAY_START)
 	{
	 	DigitalLCD[1]=DigitalBCD[CHAR_START];	//LOW = 5
	 	DigitalLCD[0]=DigitalBCD[CHAR_OFF];		//HIGH = -
	 	return;
 	}
 	// Off (--)
 	if(data == DISPLAY_OFF)
 	{
	 	DigitalLCD[1]=DigitalBCD[CHAR_OFF];
	 	DigitalLCD[0]=DigitalBCD[CHAR_OFF];
	 	return;
 	}
	// Flashing (  )
	if(data == DISPLAY_FLASH)
	{
		DigitalLCD[1]=DigitalBCD[CHAR_FLASH];
	 	DigitalLCD[0]=DigitalBCD[CHAR_FLASH];
		return;
	}
 	// Number 0...99
	if(data > 99)
	{
		DigitalLCD[1]=DigitalBCD[0];	//LOW = 0
	 	DigitalLCD[0]=DigitalBCD[0];	//HIGH = 0
	}
 	else
 	{
		temp = data%10;
		DigitalLCD[1]=DigitalBCD[temp];	//LOW
		temp = data/10;
	 	DigitalLCD[0]=DigitalBCD[temp];	//HIGH
 	}
 	//
 	return;
}

/******************************************************************************
 * Function:      void ServiceFlash(void)
 *****************************************************************************/
void ServiceFlash(void)
{
	static byte flash = 0;
	// Tick = 500 ms
	flash++;
	if(flash >= 250)
	{
		flash = 0;
		fFlashLCD = !fFlashLCD;
	}
	//Flash
	if(fFlashLCD)
	{
		switch(FlashLCD)
		{
			case FLASH_NO: break;
			case FLASH_DIGITAL: DigitalValue = DISPLAY_FLASH; fSym_Degree = false; break;
			case FLASH_WATER: fSym_Water = false; break;
			case FLASH_HEAT: fSym_Heat = false; break;
			case FLASH_D_W: DigitalValue = DISPLAY_FLASH; fSym_Water = false; fSym_Degree = false; break;
			case FLASH_D_H: DigitalValue = DISPLAY_FLASH; fSym_Heat = false; fSym_Degree = false; break;
			case FLASH_D_H_W: DigitalValue = DISPLAY_FLASH; fSym_Water = false; fSym_Heat = false; fSym_Degree = false; break;
			default: break;
		}
	}
	//
	return;
}

/******************************************************************************
 * Function:      void ClearLCD(void)
 *****************************************************************************/
void ClearLCD(void)
{
	SymbolCom1.bits = 0;
    SymbolCom2.bits = 0;
    SymbolCom3.bits = 0;
    DigitalValue = DISPLAY_OFF;
	//
	return;
}

/******************************************************************************
 * Function:      void UpdateLCD(void)
 *****************************************************************************/
void UpdateLCD(void)
{
	static byte Segments = 0;
	//
	switch(StateLCD)
	{
		// hex to bcd convert 
		case stBCD:
		{
			StateLCD++;
			// Convert BCD
			BCD(DigitalValue);
			// Symbol Com1,Com2,Com3
			SegmentsCom1 = SymbolCom1.bits;
    		SegmentsCom2 = SymbolCom2.bits;
    		SegmentsCom3 = SymbolCom3.bits;
			// Digital Com1
			SegmentsCom1 |= (DigitalLCD[0]&0b00001000);//1D
			SegmentsCom1 |= (DigitalLCD[1]&0b00001000)<<3;//2D
			// Digital Com2
			SegmentsCom2 |= (DigitalLCD[0]&0b00010000)>>2;//1E
			SegmentsCom2 |= (DigitalLCD[0]&0b01000000)>>3;//1G
			SegmentsCom2 |= (DigitalLCD[0]&0b00000100)<<2;//1C
			SegmentsCom2 |= (DigitalLCD[1]&0b00000100)<<3;//2C
			SegmentsCom2 |= (DigitalLCD[1]&0b01000000);//2G
			SegmentsCom2 |= (DigitalLCD[1]&0b00010000)<<3;//2E
			// Digital Com3
			SegmentsCom3 |= (DigitalLCD[0]&0b00100000)>>3;//1F
			SegmentsCom3 |= (DigitalLCD[0]&0b00000001)<<3;//1A
			SegmentsCom3 |= (DigitalLCD[0]&0b00000010)<<3;//1B
			SegmentsCom3 |= (DigitalLCD[1]&0b00000010)<<4;//2B
			SegmentsCom3 |= (DigitalLCD[1]&0b00000001)<<6;//2A
			SegmentsCom3 |= (DigitalLCD[1]&0b00100000)<<2;//2F
			// Bit reverse - used if reverse connect segments LCD 
			// RB0 - SEG7, ... , RB7 - SEG0
			SegmentsCom1 = BitReverse(SegmentsCom1);
			SegmentsCom2 = BitReverse(SegmentsCom2);
			SegmentsCom3 = BitReverse(SegmentsCom3);
			break;
		}
		// COM1 is LOW
		case stCom1L:
		{
			StateLCD++;
			Segments = SegmentsCom1;
			//
			mSegments = Segments;
			mCom1Low();
			mStartCom1();
			mCom1Low();
			break;
		}
		// COM1 is HIGH
		case stCom1H:
		{
			StateLCD++;
			Segments ^= 0xFF;
			//
			mSegments = Segments;
			mCom1High();
			break;
		}
		// COM2 is LOW
		case stCom2L:
		{
			StateLCD++;
			Segments = SegmentsCom2;
			//
			mStopCom1();
			mSegments = Segments;
			mCom2Low();
			mStartCom2();
			mCom2Low();
			break;
		}
		// COM2 is HIGH
		case stCom2H:
		{
			StateLCD++;
			Segments ^= 0xFF;
			//
			mSegments = Segments;
			mCom2High();
			break;
		}
		// COM3 is LOW
		case stCom3L:
		{
			StateLCD++;
			Segments = SegmentsCom3;
			//
			mStopCom2();
			mSegments = Segments;
			mCom3Low();
			mStartCom3();
			mCom3Low();
			break;
		}
		// COM3 is HIGH
		case stCom3H:
		{
			StateLCD++;
			Segments ^= 0xFF;
			//
			mSegments = Segments;
			mCom3High();
			break;
		}
		// Pause
		case stPause:
		{
			mStopCom3();
			StateLCD = stBCD;
			break;
		}
		default:
		{
			if(StateLCD > stPause)StateLCD = stBCD;
			break;
		}
    }//end switch
	//
	return;
}

/******************************************************************************
 * Function:      unsigend char BitReverse(unisgend char b)
 * Instruction Cylce = 62 (for 8Mhz - 31mks)
 *****************************************************************************/
byte BitReverse(byte b)
{
  b = ((b & 0x55) << 1) | ((b >> 1) & 0x55);
  b = ((b & 0x33) << 2) | ((b >> 2) & 0x33);
  b = ((b & 0x0F) << 4) | (b >> 4);
  //
  return b;
}

/******************************************************************************
 * Function:     ServiceUART(void);
 * Frame 8 bytes;
 * Start frame: Byte0 = 0xAA, Byte1 = 0x55;
 * --- VALUE: Byte2, 3, 4, 5
 * Value setting water temp: Byte2 - 0x05; Example 0x32 - 0x05 = 0x2D (45);
 * Value setting heat temp: Byte3 - 0x05; Example 0x53 - 0x05 = 0x4E (78);
 * Value current temperature: Byte4 - 0x05; Example 0x1E - 0x05 = 0x19 (25@ NTC10kOm);
 * Value error number: Byte5; Example 0x0A (10);
 * --- OFF (--): Byte6 and Byte7 = 0x00;
 * --- SHOW VALUE: Byte6
 * Show current temp: Byte6 = 0x02;
 * Show setting water: Byte6 = 0x80; DigitalValue Flashing;
 * Show setting heat: Byte6 = 0x??; DigitalValue Flashing;
 * Show Error: Byte6 = 0x83; DigitalValue Flashing;
 * --- SHOW SYMBOLS: Byte7 LSB
 * Show symbol Flame: Byte7.bit0 = 1;
 * Show symbol Water: Byte7.bit1 = 1;
 * Show symbol Degree: Byte7.bit2 = 1;
 * Show symbol Heat: Byte7.bit3 = 1;
 * --- FLASH SYMBOLS: Byte7 MSB
 * Flahs symbol Water: Byte7.bit5 = 1;
 * Flahs symbol Heat: Byte7.bit7 = 1;
 * --- Example:
 * 0xAA,0x55,0xFF, 0x00, 0x1E, 0x00, 0x00, 0x00 - выводим -- 
 * 0xAA,0x55,0x00, 0x00, 0x1E, 0x00, 0x02, 0x04 - выводим 25 и символ *С
 * 0xAA,0x55,0x32, 0x00, 0x1E, 0x00, 0x02, 0x06 - выводим 25*С, символ *С и символ "water"
 * 0xAA,0x55,0x32, 0x00, 0x1E, 0x00, 0x80, 0x22 - выводим 45 и символ "water", при этом символ мигают
 * 0xAA,0x55,0x32, 0x53, 0x1E, 0x00, 0x81, 0x88 - выводим 78 и символ "heat", при этом символ мигают
 *****************************************************************************/
void ServiceUART(void)
{
	SymbolCom1.bits = 0;
    SymbolCom2.bits = 0;
    SymbolCom3.bits = 0;
	FlashLCD = FLASH_NO;
	//
	if((DataReceive[0]==0xAA)&&(DataReceive[1]==0x55))
	{
		// OFF
		if((DataReceive[6]==0x00)&&(DataReceive[7]==0x00))
		{
			DigitalValue = DISPLAY_OFF;
			return;
		}
		// Error
		if(DataReceive[6]==0x83)
		{
			DigitalValue = DataReceive[5];
			FlashLCD = FLASH_DIGITAL;
			return;
		}
		// Curretn Temperature Value
		if(DataReceive[6]<=0x02)
		{
			DigitalValue = DataReceive[4] - 0x05;
		}
		// Setting Heat Temperature
		if(DataReceive[6]==0x81)
		{
			DigitalValue = DataReceive[3] - 0x05;;
			FlashLCD = FLASH_DIGITAL;
		}
		// Setting Water Temperature
		if(DataReceive[6]==0x80)
		{
			DigitalValue = DataReceive[2] - 0x05;;
			FlashLCD = FLASH_DIGITAL;
		}
		// Symbols
		if(DataReceive[7]&0b00000010) fSym_Water = true;
		if(DataReceive[7]&0b00000100) fSym_Degree = true;
		if(DataReceive[7]&0b00001000) fSym_Heat = true;
		// Flashing
		if(DataReceive[7]&0b00100000) FlashLCD += FLASH_WATER; 
		if(DataReceive[7]&0b10000000) FlashLCD += FLASH_HEAT;
	}
	//
	return;
}
