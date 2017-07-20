/*
 * Coms1.h
 *
 * Created: 19.07.2017 14:11:53
 *  Author: Robert
 */ 


#ifndef COMS1_H_
#define COMS1_H_

// EXEC API
void cm_module_init();

// Global Module API
void cm_uart_putc(char c);
void cm_uart_puts(const char* s);

//typedef struct {
	//char cmd;
	//int par1;
	//int par2;
//} cm_cmd;

#endif /* COMS1_H_ */