/*
 * Coms1.h
 *
 * Created: 19.07.2017 14:11:53
 *  Author: Robert
 */ 


#ifndef COMS1_H_
#define COMS1_H_

// EXEC API
#define EXECNR_CHAR_RX 1
#define EXECNR_CMD 2

void cm_module_init();
void uart_putc(char c);
void uart_puts(const char* s);

typedef struct {
	char cmd;
	int par1;
	int par2;
} cm_cmd;

#endif /* COMS1_H_ */