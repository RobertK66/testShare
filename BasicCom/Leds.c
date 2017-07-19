/*
 * Leds.c
 *
 * Created: 18.07.2017 16:12:20
 *  Author: Robert
 */ 
#include <avr/io.h>
#include "main.h"
#include "Leds.h"


// Locals
unsigned long counter;

void ld_module_init(uint8_t execnr) {
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	mn_enter_execute(execnr, &ld_module_exec);
}

void ld_module_exec() {
	counter++;
	if (counter > 0x7FFFFFFF) {
		YELLOW_ON;
	} else {
		YELLOW_OFF;
	}
}
