/*
 * Cmd.c
 *
 * Created: 20.07.2017 13:44:48
 *  Author: Robert
 */ 
#include <stdio.h>
#include "Globals.h"
#include "main.h"
#include "mod\Coms1.h"
#include "mod\Cmd.h"


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
		// no more commands available. Clear execute from main loop.
		MND_CLREXEC(EXECNR_CMD);
		return;
	}
	
	ix = (CdCmdRead + 1) & CD_CMDQUEUE_MASK;
	cmd = CdCmdQueue[ix];
	CdCmdRead = ix;
	
	// Just write as output ....	
	char buffer[8];      
	cm_uart_puts("Cmd '");
	cm_uart_putc(cmd.cmd);
	cm_uart_puts("': p1=");
	itoa(cmd.par1, buffer, 10);
	cm_uart_puts(buffer);
	cm_uart_puts(" p2=");
	itoa(cmd.par2, buffer, 10);
	cm_uart_puts(buffer);
	cm_uart_puts("\n");

}


void cd_module_init() {
	// Enter the execute routine under its bitnummber
	MND_ENTER_EXECUTE(EXECNR_CMD, &cd_exec_cmd);
}




void cd_execute_command(cdCmd_t cmd) {
	uint8_t ix;
	
	ix = (CdCmdWrt + 1) & CD_CMDQUEUE_MASK;
	CdCmdQueue[ix] = cmd;
	CdCmdWrt = ix;
	MND_SETEXEC(EXECNR_CMD);
}

