/*
 * Leds.h
 *
 * Created: 18.07.2017 16:12:53
 *  Author: Robert
 */ 
#ifndef LEDS_H_
#define LEDS_H_
#include <stdbool.h>

// EXEC API
#define EXECNR_LED 0

void ld_module_init(uint8_t execnr);
void ld_module_exec();

// Modul API
typedef enum
{
	RED,
	YELLOW,
	GREEN
} ld_color;

// the values here are ON/OFF times in 100ms
#define LD_FLASH_FAST 3
#define LD_FLASH_SLOW 10

bool ld_flash(uint8_t count, uint8_t time, ld_color color);
void ld_flash_force(uint8_t count, uint8_t time, ld_color color);







#endif /* LEDS_H_ */