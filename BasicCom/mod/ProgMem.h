/*
 * ProgMem.h
 *
 * Created: 20.07.2017 14:44:33
 *  Author: Robert
 */ 

//
// Custom section '.boot' must be specified in linker options to be in upper part of flash ('boot flash section').
// in ATMEL Studio this can be done by entering the line '.boot=0xc00' in the section
// ToolChain - Linker - Memory Settings - FLASHSections
//

#ifndef PROGMEM_H_
#define PROGMEM_H_


void pg_print_flash_page(uint16_t pageNr);
void pg_program_page(uint16_t pageNr, uint8_t* data)  __attribute__((section(".boot")));


#endif /* PROGMEM_H_ */