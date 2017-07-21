/*
 * ProgMem.c
 *
 * Created: 20.07.2017 14:44:20
 *  Author: Robert
 */ 

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
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



// This function must run in the boot sector area!!! -> custom section needed!
void pg_program_page (uint16_t pageNr, uint8_t *buf) 
{
	uint32_t flashStartAdr;
	uint16_t i;
	uint8_t sreg;
	
	flashStartAdr = pageNr * SPM_PAGESIZE;
	
	// Disable interrupts.
	sreg = SREG;
	cli();
	eeprom_busy_wait ();
	boot_page_erase (flashStartAdr);
	boot_spm_busy_wait ();      // Wait until the memory is erased.
	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		boot_page_fill (flashStartAdr + i, w);
	}
	boot_page_write (flashStartAdr);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.
	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable ();
	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;
}

