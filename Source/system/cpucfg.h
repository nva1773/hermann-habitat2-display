/*********************************************************************
 * FileName:        cpucfg.h
 ********************************************************************/
#ifndef CPUCFG_H
#define CPUCFG_H

/** O S C ***********************************************************/
// OSC = 8 MHz internal RC
#define OSCILLATOR	8000000
#define INSTRUCTION_CYCLE (OSCILLATOR)/4
#define mInitOSC() OSCCON = 0b01110011

/** T I M E R 0 *****************************************************/
// Timer0 = 500 Hz (16 bits mode, Fosc/4, Not prescale) low interrupt
#define TIMER0_PRESCALE 1
#define TIMER0_FREQUENCY 500
#define TIMER0_CORRECTION 158
#define TMR0_VALUE 65535-(INSTRUCTION_CYCLE/(TIMER0_FREQUENCY*TIMER0_PRESCALE))+TIMER0_CORRECTION
#define mInitTimer0() T0CON = 0b00001000; INTCON2bits.TMR0IP = 0; INTCONbits.TMR0IE = 1
#define mLoadTimer0() TMR0H = TMR0_VALUE >> 8; TMR0L = TMR0_VALUE & 0x00FF; INTCONbits.TMR0IF = 0
#define mTransmitTimer0() TMR0H = TMR0_TRANSMIT >> 8; TMR0L = TMR0_TRANSMIT & 0x00FF; INTCONbits.TMR0IF = 0
#define mStartTimer0() T0CONbits.TMR0ON = 1
#define mStopTimer0() T0CONbits.TMR0ON = 0
#define mInterruptTimer0 INTCONbits.TMR0IE && INTCONbits.TMR0IF

/** P O R T S ******************************************************/
#define mResetPorts() {ADCON1 = 0x7F;		\
					   TRISA = 0xFF;		\
					   TRISB = 0xFF;}

/** U A R T  ********************************************************/
// UART only receive, high interrupt
// USART-> UART_Async, 2400 bod, BRGH = LOW, 8bit BRG (K=64),
// SPBRG = (Fosc/Baud Rate/K)-1 = 8000000/2400/64 - 1 = 51,
// async mode, transmit enable!!!, 8 bits receive.
#define mInitUART() {TRISBbits.TRISB4 = 1;	\
					 TRISBbits.TRISB1 = 0;	\
					 TXSTAbits.BRGH = 0;	\
					 BAUDCTL = 0x00;		\
					 SPBRG = 51;			\
					 TXSTAbits.SYNC = 0;	\
					 TXSTAbits.TX9  = 0;	\
					 TXSTAbits.TXEN = 1;	\
					 RCSTAbits.RX9  = 0;	\
					 RCSTAbits.CREN = 1;	\
					 RCSTAbits.SPEN = 1;	\
					 IPR1bits.RCIP = 0;		\
					 PIE1bits.RCIE = 1;}
#define mStartTransmit() TXSTAbits.TXEN = 1
#define mStopTransmit() TXSTAbits.TXEN = 0
#define mFlagTransmit PIR1bits.TXIF
#define mFlagReceive PIR1bits.RCIF
#define mInterruptTransmit PIE1bits.TXIE&&PIR1bits.TXIF
#define mInterruptReceive PIE1bits.RCIE && PIR1bits.RCIF
//
#define mBusyUSART() !TXSTAbits.TRMT
#define mWriteUSART(a) (TXREG = a)
#define mReadUSART() RCREG
//
#define chEnter 0x0D
#define chSpace 0x20
#define chStart 'S'
#define chReceive 'R'

/** L E D ***********************************************************/
// PORTA = LED1, LED2, MCLR, NC, SEG_D, SEG_C, SEG_B, SEG_A;
// PORTB = PGD, PGC, TxD, RxD, SEG_DP, SEG_G, SEG_F, SEG_E;
#define mInitDisplay() TRISA &= 0x30; LATA |= 0xCF; TRISB &= 0xD0; LATB |= 0x2F
#define mDisplayOFF() LATA |= 0xCF; LATB |= 0x0F
#define mLoadCOM(a) {LATA &= a;}
#define mLoadSEG(a) {LATA &= (a&0x0F)|0xF0; LATB &= ((a>>4)&0x0F)|0xF0;}
#define mSegmentF() LATBbits.LATB5
#define SHOW 0
#define HIDE 1
//
#endif //CPUCFG_H