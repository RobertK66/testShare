/*
 * Leds.h
 *
 * Created: 18.07.2017 16:12:53
 *  Author: Robert
 */ 

#define BV(x) (1<<x)
#define RED_OFF (PORTC |= BV(5))
#define RED_ON  (PORTC &= ~(BV(5)))
#define YELLOW_OFF (PORTC |= BV(4))
#define YELLOW_ON  (PORTC &= ~(BV(4)))
#define GREEN_OFF (PORTC |= BV(3))
#define GREEN_ON  (PORTC &= ~(BV(3)))


// EXEC API
void ld_module_init(uint8_t execnr);
void ld_module_exec();

