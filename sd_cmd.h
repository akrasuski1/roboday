#ifndef SD_CMD_H_
#define SD_CMD_H_

#include <stdint.h>

#define CMD_
struct command{
	uint8_t type;
	int num;
};
void init_fs();

#endif
