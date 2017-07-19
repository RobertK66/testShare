/*
 * Leds.c
 *
 * Created: 18.07.2017 16:12:20
 *  Author: Robert
 */ 
#include <avr/io.h>
#include "main.h"
#include "Leds.h"


#define RED_OFF (PORTC |= BV(5))
#define RED_ON  (PORTC &= ~(BV(5)))
#define YELLOW_OFF (PORTC |= BV(4))
#define YELLOW_ON  (PORTC &= ~(BV(4)))
#define GREEN_OFF (PORTC |= BV(3))
#define GREEN_ON  (PORTC &= ~(BV(3)))


// Locals
unsigned long counter;

void ld_module_init(uint8_t execnr) {
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	MND_ENTER_EXECUTE(execnr, &ld_module_exec);
	MND_SETEXEC(execnr);
}

void ld_module_exec() {
	counter++;
	if (counter> 2000)
		counter=0;
	if (counter > 1000) {
		GREEN_ON;
	} else {
		GREEN_OFF;
	}
}
