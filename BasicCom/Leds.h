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
// TODO: wie vergeben wir diese Bits am besten fehlerfrei?... -> nach main.h schieben, oder precompiler count++ !?
#define EXECNR_LED 0

void ld_module_init();
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
#define LD_FLASH_SLOW 40

bool ld_flash(uint8_t count, uint8_t time, ld_color color);
void ld_flash_force(uint8_t count, uint8_t time, ld_color color);

// Direct IO -> be aware this interferes with running flashes....
#define RED_OFF (PORTC |= BV(5));
#define RED_ON  (PORTC &= ~(BV(5)));
#define RED_TOGGLE  (PORTC ^= BV(5));

#define YELLOW_OFF (PORTC |= BV(4));
#define YELLOW_ON  (PORTC &= ~(BV(4)));
#define YELLOW_TOGGLE  (PORTC ^= BV(4));

#define GREEN_OFF (PORTC |= BV(3));
#define GREEN_ON  (PORTC &= ~(BV(3)));
#define GREEN_TOGGLE  (PORTC ^= BV(3));

#endif /* LEDS_H_ */