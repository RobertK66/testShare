/*
 * BasicCom.c
 *
 * Created: 18.07.2017 16:00:07
 * Author : Robert
 */ 

#include <avr/io.h>

#include "main.h"
#include "Leds.h"


// Globlas
void (*ExecutePtrs[MN_EXEC_BYTES])(void);

// Locals
uint8_t execBits[MN_EXEC_BYTES];

void init_modules(uint8_t *execBits);

int main(void)
{
	
	init_modules(execBits);
	
    while (1) 
    {
		// For every set execute bit the corresponding function pointer gets called.
		for (uint8_t i = 0; i < MN_EXEC_COUNT; i++) {
			if ((execBits[i>>3] & BV(i & 0x07)) > 0) {
				ExecutePtrs[i]();
			}
		}		
    }
}


void init_modules(uint8_t *execBits) {
	// Clear all Execute Bits
	for (uint8_t i = 0; i < MN_EXEC_BYTES; i++) {
		execBits[i] = 0x00;
	}
	
	// TODO replace mit 'autocall'/reflection oder sowas ähnliches....
	ld_module_init(0);
	
}



// extern API
void mn_enter_execute(uint8_t execbit, void (* ptr)(void)){
	ExecutePtrs[execbit] = ptr;
}

void mn_setexec(uint8_t execnr){
	execBits[execnr>>3] = execBits[execnr>>3] | BV(execnr & 0x07);
}
