/*
 * ProgMem.c
 *
 * Created: 20.07.2017 14:44:20
 *  Author: Robert
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Globals.h"
#include "mod/ProgMem.h"

const char * hex = "0123456789ABCDEF";

void pg_print_flash_page(uint16_t pageNr){
	uint16_t idx;
	uint8_t c;
	uint32_t flashStartAdr = pageNr * SPM_PAGESIZE;
	
	uartPrintS("Page #");
	uartPrintI(pageNr);
	uartPrintS(" (");
	uartPrintI(flashStartAdr);
	uartPrintS("):");
	
	
	for(idx = 0; idx < SPM_PAGESIZE; idx++){
			c = pgm_read_byte(flashStartAdr + idx);
			if (!(idx%8)){
				uartPrintC('\n');
			}
			uartPrintC(hex[(c>>4) & 0xF]);
			uartPrintC(hex[ c & 0x0F]);
			uartPrintC(' ');
	}
	uartPrintS("\n");
}
