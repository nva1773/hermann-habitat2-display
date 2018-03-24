/*********************************************************************
 * FileName:        cpucfg.h
 ********************************************************************/
#ifndef CPUCFG_H
#define CPUCFG_H

/** O S C ***********************************************************/
// OSC = 8 MHz
#define OSCILLATOR	8000000
#define INSTRUCTION_CYCLE (OSCILLATOR)/4

/** T I M E R 0 *****************************************************/
// Timer0 = 500 Hz (16 bits mode, Fosc/4, Not prescale) low interrupt
#define TIMER0_PRESCALE 1
#define TIMER0_FREQUENCY 500
#define TIMER0_TRANSMIT 312
#define TIMER0_CORRECTION 158
#define TMR0_VALUE 65535-(INSTRUCTION_CYCLE/(TIMER0_FREQUENCY*TIMER0_PRESCALE))+TIMER0_CORRECTION
#define TMR0_TRANSMIT 65535-(INSTRUCTION_CYCLE/(TIMER0_TRANSMIT*TIMER0_PRESCALE))
#define mInitTimer0() T0CON = 0b00001000; INTCON2bits.TMR0IP = 0; INTCONbits.TMR0IE = 1
#define mLoadTimer0() TMR0H = TMR0_VALUE >> 8; TMR0L = TMR0_VALUE & 0x00FF; INTCONbits.TMR0IF = 0
#define mTransmitTimer0() TMR0H = TMR0_TRANSMIT >> 8; TMR0L = TMR0_TRANSMIT & 0x00FF; INTCONbits.TMR0IF = 0
#define mStartTimer0() T0CONbits.TMR0ON = 1
#define mStopTimer0() T0CONbits.TMR0ON = 0
#define mInterruptTimer0 INTCONbits.TMR0IE && INTCONbits.TMR0IF

/** P O R T S ******************************************************/
#define mResetPorts() {ADCON1 = 0x07;		\
					   TRISA = 0xFF;		\
					   TRISB = 0xFF;		\
					   TRISC = 0xFF;		\
					   LATC = 0xFF;}

/** U A R T  ********************************************************/
// UART only receive, high interrupt
// USART-> UART_Async, 2400 bod, BRGH = LOW, SPBRG = 51
// async mode, transmit disable!!!, 8 bits receive
#define mInterruptReceive PIE1bits.RCIE && PIR1bits.RCIF
#define mFlagReceive PIR1bits.RCIF
#define mInitReceive() {TRISCbits.TRISC7 = 1;	\
					 	TRISCbits.TRISC6 = 1;	\
					 	TXSTAbits.BRGH = 0;		\
					 	SPBRG = 51;				\
					 	TXSTAbits.SYNC = 0;		\
					 	RCSTAbits.SPEN = 1;		\
					 	TXSTAbits.TX9  = 0;		\
					 	TXSTAbits.TXEN = 1;		\
					 	RCSTAbits.RX9  = 0;		\
					 	RCSTAbits.CREN = 1;		\
					 	IPR1bits.RCIP = 1;		\
					 	PIE1bits.RCIE = 1;}
#define mTransmit LATCbits.LATC2
#define mInitTransmit() TRISCbits.TRISC2 = 0; mTransmit = 0

/** L I N K  L E D *************************************************/
// PORTAbits.RA5
#define mLinkLED		LATAbits.LATA5
#define mInitLinkLED()	TRISAbits.TRISA5 = 0; mLinkLED = 0

/** L C D ***********************************************************/
// PORTC = COM...COM3; PORTB = SEG7...SEG1;
#define mStartCom1() TRISCbits.TRISC5 = 0
#define mStopCom1() TRISCbits.TRISC5 = 1
#define mCom1Low() LATCbits.LATC5 = 0
#define mCom1High() LATCbits.LATC5 = 1
#define mStartCom2() TRISCbits.TRISC4 = 0
#define mStopCom2() TRISCbits.TRISC4 = 1
#define mCom2Low() LATCbits.LATC4 = 0
#define mCom2High() LATCbits.LATC4 = 1
#define mStartCom3() TRISCbits.TRISC3 = 0
#define mStopCom3() TRISCbits.TRISC3 = 1
#define mCom3Low() LATCbits.LATC3 = 0
#define mCom3High() LATCbits.LATC3 = 1
#define mSegments LATB
#define mInitLCD() TRISC = TRISC & 0xC7; LATC = LATC & 0xC7; TRISB = 0x00; LATB = 0x00
//
#endif //CPUCFG_H