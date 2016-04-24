#include "sd_cmd.h"

#include "sd/ff.h"

void die(); // Defined in other file.
void led_on(); // TODO delete me

FATFS fatfs;
FIL   file;
void init_fs(){
	if(f_mount(&fatfs,"",0)!=FR_OK){
		die();
	}
	if(f_open(&file, "data.txt", FA_READ|FA_OPEN_EXISTING)!=FR_OK){
		die();
	}
	char buff[20];
	unsigned int n;
	if(f_read(&file, buff, 20, &n)!=FR_OK){
		die();
	}
	if(buff[0]=='3'){
		led_on();
	}
}
