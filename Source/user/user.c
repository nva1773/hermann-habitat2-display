/*********************************************************************
 * FileName:        user.c
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18F1220.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\cpucfg.h"             // I/O pin mapping
#include "user\user.h"

/** C O N S T A N T S ********************************************************/
/*
const rom char DigitalBCD[]={
	         //HGFEDCBA - normal for common catode
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
			 0b01111001};//12=0x79 (E ) char "E"
			 0b01010000};//13=0x50 ( r) char "r"
*/

const rom char DigitalBCD[]={
	         //HGFEDCBA - mirror for common anode
			 0b11000000,//0=0xC0
			 0b11001111,//1=0xCF
			 0b10100100,//2=0xA4
			 0b10000110,//3=0x06
			 0b10001011,//4=0x8B
			 0b10010010,//5=0x92
			 0b10010000,//6=0x90
			 0b11000111,//7=0xC7
			 0b10000000,//8=0x80
			 0b10000010,//9=0x82
			 0b10111111,//10=0xBF (--) used for OFF
			 0b11111111,//11=0xFF (  ) used for FLASH
			 0b10110000,//12=0xB0 (E ) char "E"
			 0b10111101};//13=0xBD ( r) char "r"

const rom char CommonLED[]={0b10111111, 0b01111111};

/** V A R I A B L E S ********************************************************/
extern BYTE Flags;
extern byte DataReceive[MAX_NUM_RECEIVE];
extern byte ReceiveNum;
//
byte FlashLED;
BYTE Symbols;
unsigned char DigitalValue;
unsigned char DigitalLED[]= {0xFF,0xFF};

/******************************************************************************
 * Function:      unsigned char ReadKEY(void)
 *****************************************************************************/
void InitValue(void)
{
    Symbols.bits = 0;
    FlashLED = FLASH_NO;
    DigitalValue = DISPLAY_START;
    ClearData();
    //
    return;
}	

/******************************************************************************
 * Function:      void ByteToDigital(unsigned int data)
 *****************************************************************************/
void ByteToDigital(unsigned int data)
{
 	unsigned char temp;
	// Start (-5), 0 - 0x05 = -5
 	if(data == DISPLAY_START)
 	{
	 	DigitalLED[1]=DigitalBCD[CHAR_START];	//LOW = 5
	 	DigitalLED[0]=DigitalBCD[CHAR_OFF];		//HIGH = -
	 	return;
 	}
 	// Off (--)
 	if(data == DISPLAY_OFF)
 	{
	 	DigitalLED[1]=DigitalBCD[CHAR_OFF];
	 	DigitalLED[0]=DigitalBCD[CHAR_OFF];
	 	return;
 	}
	// Flashing (  )
	if(data == DISPLAY_FLASH)
	{
		DigitalLED[1]=DigitalBCD[CHAR_FLASH];
	 	DigitalLED[0]=DigitalBCD[CHAR_FLASH];
		return;
	}
	// Error (Er)
	if(data == DISPLAY_ERROR)
	{
		DigitalLED[1]=DigitalBCD[CHAR_R];
	 	DigitalLED[0]=DigitalBCD[CHAR_E];
		return;
	}
 	// Number 0...99
	if(data > 99)
	{
		DigitalLED[1]=DigitalBCD[0];	//LOW = 0
	 	DigitalLED[0]=DigitalBCD[0];	//HIGH = 0
	}
 	else
 	{
		temp = data%10;
		DigitalLED[1]=DigitalBCD[temp];	//LOW
		temp = data/10;
	 	DigitalLED[0]=DigitalBCD[temp];	//HIGH
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
    
    // for Test -------------
	//DigitalValue = 10;
	//FlashLED = FLASH_DIGITAL;
	//fSym_Error = true;
	// ----------------------
	
	// Tick = 2ms * 250 = 500ms
	flash++;
	if(flash >= 250)
	{
		flash = 0;
		fFlashLED = !fFlashLED;
	}
	//Flash
	if(fFlashLED)
	{
		switch(FlashLED)
		{
			case FLASH_NO: break;
			case FLASH_DIGITAL: if(fSym_Error) DigitalValue = DISPLAY_ERROR; else DigitalValue = DISPLAY_FLASH; break;
			case FLASH_WATER: fSym_Water = false; break;
			case FLASH_HEAT: fSym_Heat = false; break;
			case FLASH_D_W: DigitalValue = DISPLAY_FLASH; fSym_Water = false; break;
			case FLASH_D_H: DigitalValue = DISPLAY_FLASH; fSym_Heat = false; break;
			case FLASH_D_H_W: DigitalValue = DISPLAY_FLASH; fSym_Water = false; fSym_Heat = false; break;
			default: break;
		}
	}
	//
	return;
}

/******************************************************************************
 * Function:      void UpdateLCD(void)
 *****************************************************************************/
void UpdateLED(void)
{
	static byte StateLED = 0;
	byte Common, Segment;
	// for Test --------------------------------
	//static byte delay = 0;
	//if(++delay > 250)
	//{
	//	delay = 0;
	//	DigitalValue++;
	//	if(DigitalValue > 99) DigitalValue = 0;
	//}
	// -----------------------------------------
	//
	switch(StateLED)
	{
		case stBCD:
		{
			ByteToDigital(DigitalValue);
			mDisplayOFF();
			Segment = 0xFF;
			StateLED++;
			break;
		}
		case stLED1:
		{
			mDisplayOFF();
			Common = CommonLED[Led1];
			Segment = DigitalLED[Led1];
			if(fSym_Water) Segment &= 0x7F;
			mLoadCOM(Common);
			mLoadSEG(Segment);
			StateLED++;
			break;
		}
		case stLED2:
		{
			mDisplayOFF();
			Common = CommonLED[Led2];
			Segment = DigitalLED[Led2];
			if(fSym_Heat) Segment &= 0x7F;
			mLoadCOM(Common);
			mLoadSEG(Segment);
			StateLED = stBCD;
			break;
		}
		default:
		{
			if(StateLED > 2)StateLED = stBCD;
			break;
		}
    }//end switch
    
    // Update Segment F
    if(Segment & 0b00100000)
		mSegmentF() = HIDE;
	else
		mSegmentF() = SHOW;
	//
	return;
}

/******************************************************************************
 * Function:     ServiceUART(void);
 * Frame 8 bytes;
 * Start frame: Byte0 = 0xAA, Byte1 = 0x55;
 * --- VALUE: Byte2, 3, 4, 5
 * Value setting water temp: Byte2 - 0x05; Example 0x32 - 0x05 = 0x2D (45);
 * Value setting heat temp: Byte3 - 0x05; Example 0x2D - 0x05 = 0x28 (40);
 * Value current temperature: Byte4 - 0x05; Example 0x1E - 0x05 = 0x19 (25@ NTC10kOm);
 * Value error number: Byte5; Example 0x0A (10);
 * --- OFF (--): Byte6 and Byte7 = 0x00;
 * --- SHOW VALUE: Byte6
 * Show current temp: Byte6 = 0x02;
 * Show setting water: Byte6 = 0x80; DigitalValue Flashing;
 * Show setting heat: Byte6 = 0x81; DigitalValue Flashing;
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
 *****************************************************************************/
void ServiceUART(void)
{
	Symbols.bits = 0;
    FlashLED = FLASH_NO;
	//
	if((DataReceive[0]==0xAA)&&(DataReceive[1]==0x55))
	{
		// OFF
		if((DataReceive[6]==0x00)&&(DataReceive[7]==0x00))
		{
			DigitalValue = DISPLAY_OFF;
			return;
		}
		// Curretn Temperature Value
		if(DataReceive[6]==0x02)
		{
			DigitalValue = DataReceive[4] - 0x05;
		}
		// Setting Water Temperature
		if(DataReceive[6]==0x80)
		{
			DigitalValue = DataReceive[2] - 0x05;;
			FlashLED = FLASH_DIGITAL;
		}
		// Setting Heat Temperature
		if(DataReceive[6]==0x81)
		{
			DigitalValue = DataReceive[3] - 0x05;;
			FlashLED = FLASH_DIGITAL;
		}
		// Error
		if(DataReceive[6]==0x83)
		{
			DigitalValue = DataReceive[5];
			FlashLED = FLASH_DIGITAL;
			fSym_Error = true;
			return;
		}
		// Symbols
		if(DataReceive[7]&0b00000001) fSym_Flame = true;
		if(DataReceive[7]&0b00000010) fSym_Water = true;
		if(DataReceive[7]&0b00000100) fSym_Degree = true;
		if(DataReceive[7]&0b00001000) fSym_Heat = true;
		// Flashing
		if(DataReceive[7]&0b00100000) FlashLED += FLASH_WATER; 
		if(DataReceive[7]&0b10000000) FlashLED += FLASH_HEAT;
	}
	//
	return;
}

/******************************************************************************
 * Function:      void ClearData(void)
 *****************************************************************************/
void ClearData(void)
{
	byte i;
	for(i=0; i<MAX_NUM_RECEIVE; i++)
	{
		DataReceive[i] = 0;
	}
}