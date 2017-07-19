/*
 * main.h
 *
 * Created: 18.07.2017 16:03:48
 *  Author: Robert
 */ 
#ifndef MAIN_H_
#define MAIN_H_

#include "Globals.h"

#define MN_EXEC_COUNT	16
#define MN_EXEC_BYTES	MN_EXEC_COUNT/8

// Globals 
extern void (*ExecutePtrs[MN_EXEC_COUNT])(void);
extern uint8_t execBits[MN_EXEC_BYTES];

// API
#define MND_ENTER_EXECUTE(nr, ptr) ExecutePtrs[nr] = ptr; 
#define MND_SETEXEC(nr) execBits[nr>>3] = execBits[nr>>3] | BV(nr & 0x07);
#define MND_CLREXEC(nr) execBits[nr>>3] = execBits[nr>>3] &  ~BV(nr & 0x07);

//
//void mn_enter_execute(uint8_t execnr, void (*ptr)(void));
//void mn_setexec(uint8_t execnr);

#endif /* MAIN_H_ */