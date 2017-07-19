/*
 * Leds.c
 *
 * Created: 18.07.2017 16:12:20
 *  Author: Robert
 */ 
#include <avr/io.h>
#include <stdbool.h>

#include "main.h"
#include "Leds.h"


#define RED_OFF (PORTC |= BV(5))
#define RED_ON  (PORTC &= ~(BV(5)))
#define YELLOW_OFF (PORTC |= BV(4))
#define YELLOW_ON  (PORTC &= ~(BV(4)))
#define GREEN_OFF (PORTC |= BV(3))
#define GREEN_ON  (PORTC &= ~(BV(3)))


// Locals
uint8_t cntRed;
uint8_t cntYellow;
uint8_t cntGreen;
int timRed;
int timYellow;
int timGreen;
uint8_t red100ms;
uint8_t yellow100ms;
uint8_t green100ms;

bool switch_led(ld_color col, uint8_t* cnt);



void ld_module_init(uint8_t execnr) {
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	MND_ENTER_EXECUTE(execnr, &ld_module_exec);
	// self test
	ld_flash_force(20, LD_FLASH_FAST, GREEN);
	ld_flash_force(10, LD_FLASH_SLOW, RED);
	ld_flash_force(100, LD_FLASH_FAST, YELLOW);
}

void ld_module_exec() {
	// TODO make real timers - not mainloop counter....
	if (timRed > 0) {
		timRed--;
		if (timRed == 0){
			if (switch_led(RED, &cntRed)) {
				timRed = 100 * red100ms;
			}
		}
	}
	if (timYellow > 0) {
		timYellow--;
		if (timYellow == 0){
			if (switch_led(YELLOW, &cntYellow)) {
				timYellow = 100 * yellow100ms;
			}
		}
	}
	if (timGreen > 0) {
		timGreen--;
		if (timGreen == 0){
			if (switch_led(GREEN, &cntGreen)) {
				timGreen = 100 * green100ms;
			}
		}
	}
	// If no timer running any more - no need to keep exec bit set.
	if ((timGreen == 0)	&& (timRed == 0) && (timYellow == 0)) {
		MND_CLREXEC(EXECNR_LED);
	}
}

bool switch_led(ld_color col, uint8_t* cnt){
	uint8_t bitnr;
	switch(col){
		case RED:
			bitnr = 5;
			break;
		case YELLOW:
			bitnr = 4;
			break;
		case GREEN:
		default:
			bitnr = 3;
			break;
	}
	if (*cnt & 0x01) {
		PORTC |= BV(bitnr);
	} else {
		PORTC &= ~BV(bitnr);
	}
	(*cnt)--;
	
	if (*cnt == 0) {
		// lets stop flashing now.
		return false;
	}
	return true;
}



bool ld_flash(uint8_t count, uint8_t time, ld_color color){
	// TODO: do not override flashing if something runs on that color.....
	return false;
}

void ld_flash_force(uint8_t count, uint8_t time, ld_color color){
	switch(color){
		case RED:
			cntRed = count * 2;
			red100ms = time;
			timRed = 100 * red100ms;
			break;
		case YELLOW:
			cntYellow = count * 2;
			yellow100ms = time;
			timYellow = 100 * yellow100ms;
			break;
		case GREEN:
			cntGreen = count * 2;
			green100ms = time;
			timGreen = 100 * green100ms;
			break;
		default:
			break;
		}
		MND_SETEXEC(EXECNR_LED);
}

