#ifndef _RR_
#define _RR_

#include <sys/types.h>
#include "PSJF.h"

#define CHILD_CPU 1
#define PARENT_CPU 0

#define Unit_time()                   \
{                                     \
  volatile unsigned long t;           \
  for (t = 0; t < 1000000UL; t++);    \
}                                     \

// rr scheduler
int RR_next(struct process *proc, int num_process);

// init cpu
//int proc_assign_cpu(int pid, int core);

// execute process
int proc_exec(struct process proc); 

// set low piority to the process
int proc_block(int pid);

// set high priority to process
int proc_wakeup(int pid);

// main of RR
void RR_main(struct process *proc, int num_process);

#endif
