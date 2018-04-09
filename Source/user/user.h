/*********************************************************************
 * FileName:        user.h
 ********************************************************************/
#ifndef USER_H
#define USER_H

/** P U B L I C  P R O T O T Y P E S ********************************/
void InitValue(void);
void ByteToDigital(word data);
void ServiceFlash(void);
void UpdateLED(void);
void ServiceUART(void);
void ClearData(void);

/** D E F I N E **************************************************************/
// Flags LCD
#define fUpdateLED	 	Flags.b0
#define fFlashLED		Flags.b1

// Flags UART
#define fReceiveStart	Flags.b4
#define fReceiveNext	Flags.b5
#define fReceiveSuccess	Flags.b6

// Display Symbols
#define fSym_Heat		Symbols.b0
#define fSym_Water		Symbols.b1
#define fSym_Error		Symbols.b2
#define fSym_Flame		Symbols.b3
#define fSym_Degree		Symbols.b4

// Display state machine
#define stBCD		0
#define stLED1		1
#define stLED2		2
#define Led1		0
#define Led2		1

// UART
#define MAX_NUM_RECEIVE 8

// CHAR DISPLAY
#define DISPLAY_START 105
#define CHAR_START 5
#define DISPLAY_OFF 100
#define CHAR_OFF 10
#define DISPLAY_FLASH 101
#define CHAR_FLASH 11
#define DISPLAY_ERROR 103
#define CHAR_E 12
#define CHAR_R 13

// FLASH
#define FLASH_NO		0x00
#define FLASH_DIGITAL	0x01
#define FLASH_WATER		0x20
#define FLASH_HEAT		0x80
#define FLASH_D_W		0x21
#define FLASH_D_H		0x81
#define FLASH_D_H_W		0xA1
//
#endif //USER_H
