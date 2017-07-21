/*
 * Cmd.c
 *
 * Created: 20.07.2017 13:44:48
 *  Author: Robert
 */ 
#include <stdio.h>
#include "Globals.h"
#include "main.h"
#include "mod/Cmd.h"
#include "mod/Leds.h"
#include "mod/ProgMem.h"

#define CD_CMDQUEUE_SIZE 8
#define CD_CMDQUEUE_MASK (CD_CMDQUEUE_SIZE - 1)

// Command queue with parsed commands executed in 2nd Exec.
cdCmd_t CdCmdQueue[CD_CMDQUEUE_SIZE];
uint8_t CdCmdWrt;
uint8_t CdCmdRead;

/// EXEC APIs
void cd_exec_cmd() {
	uint8_t ix;
	cdCmd_t cmd;
	
	if ( CdCmdWrt == CdCmdRead ) {
		// no more commands to process. Clear execute from main loop.
		MND_CLREXEC(EXECNR_CMD);
		return;
	}
	
	ix = (CdCmdRead + 1) & CD_CMDQUEUE_MASK;
	cmd = CdCmdQueue[ix];
	CdCmdRead = ix;
	
	switch (cmd.cmd) {
		case 'r':
			pg_print_flash_page(cmd.par1);
			break;
		case 'w':	
			{	// Writes (dummy) data to a flash page.
				uint8_t pageData[64];
				for (int i=0;i<64;i++){
					pageData[i] = i & 0xFF;
				}
				pg_program_page(cmd.par1, pageData);
			}
			break;

		case 'l':
			// Flashes the red led par1 times (with par2 timimg) 
			ld_flash_force(cmd.par1, ((cmd.par2>0)?cmd.par2:LD_FLASH_SLOW) , RED);
			break;
			
		
		default:
			{
				// Just write as output ....
				uartPrintS("Cmd '");
				uartPrintC(cmd.cmd);
				uartPrintS("': p1=");
				uartPrintI(cmd.par1);
				uartPrintS(" p2=");
				uartPrintI(cmd.par2);
				uartPrintS("\n");
			}
			break;	
	}

}


void cd_module_init() {
	// Enter the execute routine under its bitnummber
	MND_ENTER_EXECUTE(EXECNR_CMD, &cd_exec_cmd);
}


// The command is put in queue and execute bit to process this is set.
void cd_execute_command(cdCmd_t cmd) {
	uint8_t ix;
	
	ix = (CdCmdWrt + 1) & CD_CMDQUEUE_MASK;
	if (ix == CdCmdRead) {
		// queue is full!!! What to do?
		
	} else {
		CdCmdWrt = ix;
		CdCmdQueue[ix] = cmd;
		MND_SETEXEC(EXECNR_CMD);
	}
}

