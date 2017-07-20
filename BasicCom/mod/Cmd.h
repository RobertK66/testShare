/*
 * Cmd.h
 *
 * Created: 20.07.2017 13:45:02
 *  Author: Robert
 */ 


#ifndef CMD_H_
#define CMD_H_

typedef struct {
	char cmd;
	int par1;
	int par2;
} cdCmd_t;

void cd_execute_command(cdCmd_t cmd);



#endif /* CMD_H_ */