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
int proc_assign_cpu(int pid, int core);
int proc_wakeup(int pid);
int proc_exec(struct process proc);
int proc_block(int pid);
int next_proc(struct process *proc, int n_proc);
//main function
void FIFO(struct process *proc, int n_proc);


#endif
