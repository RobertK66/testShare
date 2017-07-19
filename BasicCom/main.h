/*
 * main.h
 *
 * Created: 18.07.2017 16:03:48
 *  Author: Robert
 */ 
#include <avr/pgmspace.h>

#ifndef MAIN_H_
#define MAIN_H_

#define MN_EXEC_COUNT	16
#define MN_EXEC_BYTES	MN_EXEC_COUNT/8

extern void (*ExecutePtrs[])(void);

void mn_enter_execute(uint8_t execnr, void (*ptr)(void));

#endif /* MAIN_H_ */