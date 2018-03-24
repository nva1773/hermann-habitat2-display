/*********************************************************************
 * FileName:        user.h
 ********************************************************************/
#ifndef USER_H
#define USER_H

/** P U B L I C  P R O T O T Y P E S ********************************/
void InitValue(void);
void BCD(word data);
void ServiceFlash(void);
void ClearLCD(void);
void UpdateLCD(void);
byte BitReverse(byte b);
void ServiceUART(void);

/** D E F I N E **************************************************************/
// Flags LCD
#define fUpdateLCD	 	Flags.b0
#define fFlashLCD		Flags.b1
// Flags UART
#define fReceiveStart	Flags.b4
#define fReceiveNext	Flags.b5
#define fReceiveSuccess	Flags.b6
// Display Emmergas
// SymbolCom1
#define fSym_Power4	SymbolCom1.b0
#define fSym_Power3	SymbolCom1.b1
#define fSym_Heat 	SymbolCom1.b2
#define fSym_1D  	SymbolCom1.b3
#define fSym_Flame 	SymbolCom1.b4
#define fSym_Summer	SymbolCom1.b5
#define fSym_2D 	SymbolCom1.b6
#define fSym_Water 	SymbolCom1.b7
// SymbolCom2
#define fSym_Power2	SymbolCom2.b0
#define fSym_Power1	SymbolCom2.b1
#define fSym_1E 	SymbolCom2.b2
#define fSym_1G 	SymbolCom2.b3
#define fSym_1C 	SymbolCom2.b4
#define fSym_2C 	SymbolCom2.b5
#define fSym_2G 	SymbolCom2.b6
#define fSym_2E 	SymbolCom2.b7
// SymbolCom
#define fSym_Degree	SymbolCom3.b0
#define fSym_Winter	SymbolCom3.b1
#define fSym_1F 	SymbolCom3.b2
#define fSym_1A  	SymbolCom3.b3
#define fSym_1B 	SymbolCom3.b4
#define fSym_2B 	SymbolCom3.b5
#define fSym_2A		SymbolCom3.b6
#define fSym_2F 	SymbolCom3.b7
// UART
#define MAX_NUM_RECEIVE 8
// CHAR DISPLAY
#define DISPLAY_START 105
#define CHAR_START 5
#define DISPLAY_OFF 100
#define CHAR_OFF 10
#define DISPLAY_FLASH 101
#define CHAR_FLASH 11
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
