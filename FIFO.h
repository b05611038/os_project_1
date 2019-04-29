#ifndef _FIFO_
#define _FIFO_

#include "PSJF.h"

#define PARENT_CPU 0
#define CHILD_CPU 1

//members of proc
/*
struct process{
	char name[16];
	int ready;
	int exec;
	//pid_t pid;
};
*/
//define one unit of time
void time_unit();
//function called by qsort
int compare(const void *p1, const void *p2);
//main function
void FIFO(struct process *proc, int n_proc);


#endif
