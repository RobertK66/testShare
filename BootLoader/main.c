/*
 * BootLoader.c
 *
 * Created: 09.07.2017 16:46:31
 * Author : Robert
 */ 
#include "bootloader.h"

#define BV(x) (1<<x)
#define RED_OFF (PORTC |= BV(5))
#define RED_ON  (PORTC &= ~(BV(5)))
#define YELLOW_OFF (PORTC |= BV(4))
#define YELLOW_ON  (PORTC &= ~(BV(4)))
#define GREEN_OFF (PORTC |= BV(3))
#define GREEN_ON  (PORTC &= ~(BV(3)))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uart.h"
 
#define BOOT_UART_BAUD_RATE     9600     /* Baudrate */
#define XON                     17       /* XON Zeichen */
#define XOFF                    19       /* XOFF Zeichen */

int main();

// Section .myapp liegt bei 0x0000 im prog mem.
// alles andere (.text leigt ab 0x1800 (=> word ptr: 0x0c00 als boot section start)
void mainApp() __attribute__((section(".myapp")));
void mainApp() {
	DDRC = 0xFF;
	PORTC = 0xFF;
		
	int i = 10;
	while (i-- > 0 ) {
		RED_ON;
		_delay_ms(1000);
		RED_OFF;
		_delay_ms(400);
	}	
}

#define C_MODE_EMPTY  0x00
#define C_MODE_READINT1 0x01
#define C_MODE_READINT2 0x02
#define C_MODE_TOOMUCH 0x03

char valueBuffer[17];
unsigned char bufIx;

int ConvertBuf() {
	valueBuffer[bufIx & 0x0F] = '\0';
	return atoi(valueBuffer);
}

unsigned char	cmdMode;
unsigned char   cmd;
int par1;
int par2;

void initLine(void ) {
	cmdMode = C_MODE_EMPTY;
	cmd = 0;
	par1 = 0;
	par2 = 0;
	uart_puts("Boot>");
}

bool ReadFlashPage(uint32_t flashStartAdr, uint8_t  *dataPage){
	unsigned int index;
	if(!(flashStartAdr & (SPM_PAGESIZE-1))){      // If input address is a page address
		for(index = 0; index < SPM_PAGESIZE; index++){
			dataPage[index] = pgm_read_byte(flashStartAdr + index);
		}
		return true;                            // Return TRUE if valid page address
	}
	else{
		return false;                           // Return FALSE if not valid page address
	}
}

void boot_program_page (uint32_t flashStartAdr, uint8_t *buf)
{
	uint16_t i;
	uint8_t sreg;
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


const char * hex = "0123456789ABCDEF";


unsigned char execute(unsigned char c, int par1, int par2) {
	
	unsigned char pageData[SPM_PAGESIZE];
	
	uart_putc(c);
	uart_putc(':');
	char buffer[7];      // buffer to put string
	itoa(par1, buffer, 10);    // make the string
	uart_puts("(");
	uart_puts(buffer);
	itoa(par2, buffer, 10);    // make the string
	uart_puts(",");
	uart_puts(buffer);
	uart_puts(")\n\r");

	switch (c) {
		case 'q':
			return 0;
		case 'r':
			{	// read a page from flash	par1 ... page #
				//
				if (ReadFlashPage(par1*SPM_PAGESIZE, pageData)) {
					uart_puts("ReadOk:");
					for (int i=0;i<SPM_PAGESIZE;i++){
						if (!(i%8)){
							uart_putc('\n');
						}
						//printf("0x%02x, ", pageData[i]);
						uart_putc(hex[(pageData[i]>>4)&0xF]);
						uart_putc(hex[(pageData[i])&0xF]);
						uart_putc(' ');
					}
				} else {
					uart_puts("Error in read\n\r");
				}
			}
			break;
		case 'w':
			{	// write some data to flash	par1 ... page #
				//
				for (int i=0;i<SPM_PAGESIZE;i++){
					pageData[i] = i & 0xFF;
				}				
				boot_program_page(par1*SPM_PAGESIZE, pageData);
				
			}
		default:
			break;		
	}
	
	
	return 1;
}


int main() 
{
	// Endlosschleife Bootloader:
	do {
	
		unsigned int 	c=0;            /* Empfangenes Zeichen + Statuscode */
		unsigned char	temp;           /* Variable */
		unsigned char   flag=1;         /* Flag zum steuern der Endlosschleife */
	//	void (*start)( void ) = 0x0000; /* Funktionspointer auf 0x0000 */
				
		/* Interrupt Vektoren verbiegen */
		char sregtemp = SREG;
		cli();
		temp = GICR ;
		GICR  = temp | (1<<IVCE);
		GICR  = temp | (1<<IVSEL);
		SREG = sregtemp;
 
		/* Einstellen der Baudrate und aktivieren der Interrupts */
		uart_init( UART_BAUD_SELECT(BOOT_UART_BAUD_RATE,F_CPU) ); 
		sei();
 
		initLine();
 
		do
		{	// "Command interpreter Format "x 123 456" wobei "abcdx 123 456" das selbe gint (command char 'x'!) 
			// 'q' verlässt die command loop.
			c = uart_getc();
			if( !(c & UART_NO_DATA) )
			{
				switch((unsigned char)c)
				{
					case '\n':
					case '\r':
						if (cmdMode == C_MODE_READINT1) {
							par1 = ConvertBuf();
						} else if (cmdMode == C_MODE_READINT2) {
							par2 = ConvertBuf();
						}
						flag = execute(cmd, par1, par2) ;
						initLine();
						break;
					case ' ':
						if (cmdMode == C_MODE_EMPTY) {
							cmdMode = C_MODE_READINT1;
							bufIx = 0;
						} else if (cmdMode == C_MODE_READINT1) {
						  	par1 = ConvertBuf();
							cmdMode = C_MODE_READINT2;
							bufIx = 0;  
						} else if (cmdMode == C_MODE_READINT2) {
							par2 = ConvertBuf();
							cmdMode = C_MODE_TOOMUCH;
						}
						break;
					default:
						if (cmdMode == C_MODE_EMPTY) {
							cmd = c;
						} else {
							valueBuffer[(bufIx++) & 0x0F] = c;
						}
 						break;
				}
			}
		}
		while(flag);
 
		uart_puts("Springe zur Adresse 0x0000!\n\r");
		_delay_ms(1000);

		/* vor Rücksprung eventuell benutzte Hardware deaktivieren
		   und Interrupts global deaktivieren, da kein "echter" Reset erfolgt */

		/* Interrupt Vektoren wieder gerade biegen */
		cli();
		temp = GICR ;
		GICR  = temp | (1<<IVCE);
		GICR  = temp & ~(1<<IVSEL);

		/* Rücksprung zur Adresse 0x0000 */
		//!?EIND=0 wenn win 2byte jmp nicht ausreicht, weil Flash uu groß 
		//start(); 
		
		mainApp();
		// Wenn Hauptprogramm ein return macht, kommen wir wieder in die Bootloader Schleife .... 
	} while (1);
	return (0);
}




