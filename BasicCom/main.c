/*
 * BasicCom.c
 *
 * Created: 18.07.2017 16:00:07
 * Author : Robert
 */ 

#include <avr/io.h>

#include "main.h"
#include "Leds.h"

// Globals
//uint8_t test __attribute__ ((section (".data.MySram0")));
void (*ExecutePtrs[MN_EXEC_COUNT])(void) ;	//TODO: vieleicht besser diese ptr list vom Compiler im Flash erzeugen .....
uint8_t ExecBits[MN_EXEC_BYTES];

// Locals
void init_modules();

int main(void)
{
	init_modules();
    while (1) 
    {
		// For every set execute bit the corresponding function pointer gets called.
		for (uint8_t i = 0; i < MN_EXEC_COUNT; i++) {
			if ((ExecBits[i>>3] & BV(i & 0x07)) > 0) {
				ExecutePtrs[i]();
			}
		}		
    }
}

void init_modules() {
	// Clear all Execute Bits
	for (uint8_t i = 0; i < MN_EXEC_BYTES; i++) {
		ExecBits[i] = 0x00;
	}
	
	// TODO replace mit 'autocall'/reflection oder sowas ähnliches....
	ld_module_init(EXECNR_LED);
}

