#ifndef SD_CMD_H_
#define SD_CMD_H_

#include <stdint.h>

// Protocol for commands.
#define CMD_END        0
#define CMD_RAISE_PEN  1
#define CMD_DROP_PEN   2
#define CMD_TURN_RIGHT 3
#define CMD_TURN_LEFT  4
#define CMD_FORWARD    5
#define CMD_BACKWARD   6
typedef struct command{
	uint8_t type;
	int num;
} command;
void init_fs();
void get_cmd(command* cmd);

#endif
