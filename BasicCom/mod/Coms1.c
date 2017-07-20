/*
 * Coms1.c
 *
 * Created: 19.07.2017 14:11:37
 *  Author: Robert
 */ 
#include <stdlib.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "Globals.h"
#include "main.h"
#include "mod\Cmd.h"
#include "mod\Coms1.h"

#define BAUD 9600

#define CM_TX_BUFFER_SIZE 64
#define CM_TX_BUFFER_MASK (CM_TX_BUFFER_SIZE-1)
#define CM_RX_BUFFER_SIZE 64
#define CM_RX_BUFFER_MASK (CM_RX_BUFFER_SIZE-1)

//#define CM_CMDQUEUE_SIZE 8
//#define CM_CMDQUEUE_MASK (CM_CMDQUEUE_SIZE - 1)
//

// Locals
// Ringbuffer with pointers for usage in TX and RX interrupts.
volatile char CmTxBuffer[CM_TX_BUFFER_SIZE];
volatile char CmRxBuffer[CM_RX_BUFFER_SIZE];
volatile uint8_t CmTxWrt;
volatile uint8_t CmTxRead;
volatile uint8_t CmRxWrt;
volatile uint8_t CmRxRead;
volatile bool CmRxBufferOverflow;

//// Command queue with parsed commands executed in 2nd Exec.
//cm_cmd CmCmdQueue[CM_CMDQUEUE_SIZE];
//uint8_t CmCmdWrt;
//uint8_t CmCmdRead;

void cm_exec_charrx();

void uart_puts(const char* s);
void uart_putc(char c);

typedef enum {
	AWAIT_COM_CHAR,
	AWAIT_PAR1,
	CHAR_PAR1,
	AWAIT_PAR2,
	CHAR_PAR2, 
	IGNORE_REST
	} cm_linestatus;

cm_linestatus cmStat; 
cdCmd_t cmCurrentCmd;
uint8_t cm_ParIx;
char cm_parBuffer[8];


/// EXEC APIs
void cm_module_init() {
	CmTxWrt = 0;
	CmTxRead = 0;
	CmRxWrt = 0;
	CmRxRead = 0;
	
	cmStat = AWAIT_COM_CHAR;
	
	UBRRH = (unsigned char)((((F_CPU)/(BAUD*16UL)-1)>>8)&0x80) ;
    UBRRL = (unsigned char) (((F_CPU)/(BAUD*16UL)-1)&0x00FF);
		
	UCSRB|= BV(TXEN)|BV(RXEN)|BV(RXCIE);                
	UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);   
	sei();
		
	//MND_ENTER_EXECUTE(EXECNR_CMD, &cm_exec_cmd);
	MND_ENTER_EXECUTE(EXECNR_CHAR_RX, &cm_exec_charrx);
	
	cm_uart_puts("hallo\n\r");
}
//
//void cm_exec_cmd() {
	//uint8_t ix;
	//cm_cmd cmd;
	//
	//if ( CmCmdWrt == CmCmdRead ) {
		//// no more commands available. Clear execute from main loop.
		//MND_CLREXEC(EXECNR_CMD);
		//return;
	//}
	//
	//ix = (CmCmdRead + 1) & CM_CMDQUEUE_MASK;
	//cmd = CmCmdQueue[ix];
	//CmCmdRead = ix;
	//
	//char buffer[8];      // buffer to put string
			//
	//cm_uart_puts("Cmd '");
	//cm_uart_putc(cmd.cmd);
	//cm_uart_puts("': p1=");
	//itoa(cmd.par1, buffer, 10);
	//cm_uart_puts(buffer);
	//cm_uart_puts(" p2=");
	//itoa(cmd.par2, buffer, 10);
	//cm_uart_puts(buffer);
	//cm_uart_puts("\n");
//
//}

uint16_t cm_parse_par() {
	cm_parBuffer[cm_ParIx & 0x07] = '\0';
	return atoi(cm_parBuffer);
}

// This Exec gets set if there is data aveilable in the Rx ringbuffer. We do processs only one char
// per mainloop here. The detected command line ist then entered in queue and executed in seperate exec.
void cm_exec_charrx(){
	uint8_t ix;
	char c;
	if ( CmRxWrt == CmRxRead ) {
		// no more char available. Clear execute from main loop.
		MND_CLREXEC(EXECNR_CHAR_RX);
		return;
	}
	if (CmRxBufferOverflow) {
		// We are to slow ???
		// lets reset out comms. Reinit Uart !? 
		cli();
		CmRxBufferOverflow = false;
		CmTxWrt = 0;
		CmTxRead = 0;
		CmRxWrt = 0;
		CmRxRead = 0;
		cmStat = AWAIT_COM_CHAR;
		MND_CLREXEC(EXECNR_CHAR_RX);
		sei();
		return;
	}
	
	ix = (CmRxRead + 1) & CM_RX_BUFFER_MASK;
	c =  CmRxBuffer[ix];
	CmRxRead = ix;
	
	if ((c == '\n') || (c == '\r')) {
		if (cmStat == CHAR_PAR1) {
			cmCurrentCmd.par1 = cm_parse_par();
		}
		if (cmStat == CHAR_PAR2) {
			cmCurrentCmd.par2 = cm_parse_par();
		}
		if (cmCurrentCmd.cmd != ' ') {
			cd_execute_command(cmCurrentCmd);
			//ix = (CmCmdWrt + 1) & CM_CMDQUEUE_MASK;
			//CmCmdQueue[ix] = cmCurrentCmd;
			//CmCmdWrt = ix;
			//MND_SETEXEC(EXECNR_CMD);
		}
		cmCurrentCmd.cmd = ' ';
		cmCurrentCmd.par1 = 0;
		cmCurrentCmd.par2 = 0;
		cmStat = AWAIT_COM_CHAR;
		
		return;
	}
	
	switch(cmStat) {
		case AWAIT_COM_CHAR:
			if (c == ' ') {
				cmStat = AWAIT_PAR1;
			} else {
				cmCurrentCmd.cmd = CmRxBuffer[ix];
			}
			break;
		case AWAIT_PAR1:
		case AWAIT_PAR2:
			cm_ParIx = 0;
			cm_parBuffer[cm_ParIx++ & 0x07] = c;
			if (cmStat == AWAIT_PAR1) cmStat = CHAR_PAR1;
			if (cmStat == AWAIT_PAR2) cmStat = CHAR_PAR2;
			break;
		case CHAR_PAR1:
			if (c == ' ') {
				cmCurrentCmd.par1 = cm_parse_par();
				cmStat = AWAIT_PAR2;			
			} else {
				cm_parBuffer[cm_ParIx++ & 0x07] = c;
			}
			break;
		case CHAR_PAR2:
			if (c == ' ') {
				cmCurrentCmd.par2 = cm_parse_par();
				cmStat = IGNORE_REST;
			} else {
				cm_parBuffer[cm_ParIx++ & 0x07] = c;
			}
			break;
		default:
			break;
	 }
}

ISR (USART_RXC_vect)
{
	uint8_t ix;
	// error = UCSRA & (BV(FE)|BV(DOR)|BV(PE));
	
	ix = ( CmRxWrt + 1) & CM_RX_BUFFER_MASK;
	if ( ix == CmRxRead ) {
		CmRxBufferOverflow = true;
	} else {
		CmRxWrt = ix;
		CmRxBuffer[ix] = UDR;
		// Echo 
		//UDR = CmRxBuffer[ix];
	}
	// inform main loop, that there is something to process.
	MND_SETEXEC(EXECNR_CHAR_RX);
}


ISR (USART_UDRE_vect)
{
	uint8_t ix;
	
	if ( CmTxWrt != CmTxRead) {
		ix = (CmTxRead + 1) & CM_TX_BUFFER_MASK;
		CmTxRead = ix;
		UDR = CmTxBuffer[ix];  /* send byte */
	} else {
		/* disable send interrupt */
		UCSRB &= ~BV(UDRIE);
	}
}

void cm_uart_putc(char c)
{
	uint8_t ix;
	ix  = (CmTxWrt + 1) & CM_TX_BUFFER_MASK;
	
	while ( ix == CmTxRead );	// wait until free place -> ??? timeout ???? -> error 
	CmTxBuffer[ix] = c;
	CmTxWrt = ix;

	/* enable send interrupt */
	UCSRB |= BV(UDRIE);
}

void cm_uart_puts(const char *s )
{
	while (*s) cm_uart_putc(*s++);
}

void cm_uart_puti(int i) {
	char buffer[8];
	itoa(i, buffer, 10);
	cm_uart_puts(buffer);
}
