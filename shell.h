#ifndef SHELL_H
#define SHELL_H
#include <stdlib.h>

//Using nil for tree relationship
#define nil NULL
#define true 1
#define false 0

/* Process structure

*/

typedef struct Process Process;

//pid and name characteristics to struct
struct Process {
	char* name;
	int pid;
};
#endif
