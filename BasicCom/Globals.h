/*
 * Globals.h
 *
 * Created: 19.07.2017 08:08:22
 *  Author: Robert
 */ 
#ifndef GLOBALS_H_
#define GLOBALS_H_

#define F_CPU 8000000UL

// Execut bits
#define MN_EXEC_COUNT	16
#define EXECNR_LED		0
#define EXECNR_CHAR_RX	1
#define EXECNR_CMD		2

#define BV(x) (1<<x)

// 'Global Base Functions' -> really implemented and prototyped in other Module files! 'Pseudo Interfaces' to get module dependencies 'go away'. ;-)
#define uartPrintC(c) cm_uart_putc(c)
#define uartPrintS(s) cm_uart_puts(s)
// copy of implementing prototypes still needed here. but if you only use the defines this can 
// be replaced with other (module) implementation later ....
void cm_uart_putc(char c);
void cm_uart_puts(const char* s);



#endif /* GLOBALS_H_ */