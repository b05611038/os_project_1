#ifndef _RR_
#define _RR_

#include <sys/types.h>

#define CHILD_CPU 1
#define PARENT_CPU 0
#define FIFO    1
#define RR      2
#define SJF     3
#define PSJF    4

#define Unit_time()                   \
{                                     \
  volatile unsigned long t;           \
  for (t = 0; t < 1000000UL; t++);    \
}                                     \


struct Process {
  char name[8];
  int ready;
  int remain;
  pid_t pid;
};

// rr scheduler
int RR_next(struct Process *proc, int num_process);

// init cpu
int proc_assign_cpu(int pid, int core);

// execute process
int proc_exec(struct Process proc) 

// set low piority to the process
int proc_block(int pid)

// set high priority to process
int proc_wakeup(int pid)

// main of RR
void RR(struct Process *proc, int num_process);

#endif
