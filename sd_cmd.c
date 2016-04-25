#include "sd_cmd.h"

#include "sd/ff.h"
#include <stdlib.h>

void die(); // Defined in other file.

FATFS fatfs;
FIL   file;
void init_fs(){
	if(f_mount(&fatfs,"",0)!=FR_OK){
		die();
	}
	if(f_open(&file, "data.txt", FA_READ|FA_OPEN_EXISTING)!=FR_OK){
		die();
	}
}

static char get_char(){
	char c;
	unsigned int n;
	if(f_read(&file, &c, 1, &n)!=FR_OK){die();}
	return c;
}

uint8_t is_digit(char c){
	return c<='9' && c>='0';
}

static int get_int(){
	char buff[20];
	while(1){ // Skip bytes until a digit.
		buff[0]=get_char();
		if(is_digit(buff[0])){
			break;
		}
	}
	for(uint8_t i=1;;i++){
		buff[i]=get_char();
		if(!is_digit(buff[i])){
			buff[i]=0;
			break;
		}
	}
	return atoi(buff);
}
void get_cmd(command* res){
	res->type=get_int();
	switch(res->type){
	case CMD_TURN_LEFT:
	case CMD_TURN_RIGHT:
	case CMD_FORWARD:
	case CMD_BACKWARD:
		res->num=get_int();
		break;
	default:
		break;
	}
}
