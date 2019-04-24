#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "PSJF.h"
#define CHILD_CPU 1
#define PARENT_CPU 0

/* Running one unit time */
#define UNIT_T()				\
{						\
	volatile unsigned long i;		\
	for (i = 0; i < 1000000UL; i++);	\
}						\

//struct process {
//	char name[20];
//	int ready;
//	int exec;
//	pid_t pid;
//};

/* Last context switch time for RR scheduling */
static int t_last;

/* Current unit time */
static int ntime;

/* Index of running process. -1 if no process running */
static int running;

/* Number of finish Process */
static int finish_cnt;

int proc_assign_cpu(int pid, int core){

	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index error.");
		return -1;
	}
  //printf("Parent pid: %d\n", p_pid);
	//The cpu_set_t data structure represents a set of CPUs.
	//"mask" represents a set of CPUs
	cpu_set_t mask;
	// Clears mask, so that it contains no CPUs.
	CPU_ZERO(&mask);
	//Add CPU core to mask.
	CPU_SET(core, &mask);
	//printf("size of CPU set: %ld\n", sizeof(mask));	//128
	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		//sets the CPU affinity mask of the thread whose ID
     //  is p_pid to the value specified by mask. 
		perror("sched_setaffinity");
		exit(1);
	}
  return 0;
}

/* Sort processes by ready time */
int cmp(const void *a, const void *b) {
	return ((struct process *)a)->ready - ((struct process *)b)->ready;
}

void SJF(struct process *proc, int nproc)
{
	printf("SJF is successfully called!\n");
	//proc = (struct process *)malloc(nproc * sizeof(struct process));
  //sort processes by ready time, NOT execution time
	qsort(proc, nproc, sizeof(struct process), cmp);
	/* Initial pid = -1 imply not ready */
	for (int i = 0; i < nproc; i++)
	{ 
		proc[i].pid = -1;
	}
	/* Set single core prevent from preemption */
  int p_pid = getpid();	
	proc_assign_cpu(p_pid, PARENT_CPU);
	/* Set high priority to scheduler */
	struct sched_param param_1;
	/* SCHED_OTHER should set priority to 0 */
	param_1.sched_priority = 0;
  int p_pid2 =getpid(); //p_pid2 is the same as the p_pid
	//because they are all parent process
	int ret_1 = sched_setscheduler(p_pid2, SCHED_OTHER, &param_1);
	//printf("Parent pid_2: %d\n", p_pid2);
	if (ret_1 < 0) {
		perror("sched_setscheduler");
		//return -1;
	}
	/* Initial scheduler */
	ntime = 0;
	running = -1;
	finish_cnt = 0;
	
	while(1) {
		//fprintf(stderr, "Current time: %d\n", ntime);

		/* Check if running process finish */
		if (running != -1 && proc[running].exec == 0) {
		
#ifdef DEBUG
			fprintf(stderr, "%s finish at time %d.\n", proc[running].name, ntime);
#endif
			//kill(running, SIGKILL);
			//printf("Pid of running process: %d\n", proc[running].pid);//pid>0
			//The waitpid() system call suspends execution of the calling process until a child specified by pid argument has changed state.
			//printf("%s waiting for exit...\n", proc[running].name);
			waitpid(proc[running].pid, NULL, 0);
		  //printf("%s suspended after termination...\n", proc[running].name);	
			printf("%s %d\n", proc[running].name, proc[running].pid);
			running = -1;
			finish_cnt++;
			/* All process finish */
			if (finish_cnt == nproc)
				break; //break while(1)
		}
    //printf("value of running is %d\n", running);
		/* Check if process ready and execute */
		for (int i = 0; i < nproc; i++) {
			 if (proc[i].ready == ntime) {
       //if process i is ready, fork a child process to execute
	     int c_pid = fork();
			 //On success, the PID of the child process is returned in the parent,
       //and 0 is returned in the child.  On failure, -1 is returned in the
       //parent, no child process is created, and errno is set appropriately.
			 //printf("%s is forked...\n", proc[i].name);
	     //printf("Child process id is %d\n", c_pid); //24615, the pid of the child process
			 //printf("Get pid %d\n: ", getpid());//24614,the pid of the parent process
			 if (c_pid < 0) {
				//if pid<0, fork fails	
		    perror("fork fails! ");
		   // return -1;
	       }
    	 if (c_pid == 0) {//if pid==0, we successfully forked a child process
				//printf("%s  begins execution at time %d...(Child)\n", proc[i].name, ntime);
		   //deal with Kernel later
			 //unsigned long start_sec, start_nsec, end_sec, end_nsec;
		   //char to_dmesg[200];
		   //syscall(GET_TIME, &start_sec, &start_nsec);
			 //the process is executing for t_exec unit of time
		   for (int j = 0; j < proc[i].exec; j++) {
			       UNIT_T();
						//note 1 unit of time here does not equal to the 1 unit of time outside
						//1 unit of time in the child process(represented by "j") does NOT equal to 
						//1 unit of time in the parent process(represented by "ntime")
						//printf("%s is running for %d unit of time!\n",proc[i].name, j+1 ); 
#ifdef DEBUG
			  if (j % 100 == 0){
           fprintf(stderr, "%s is executing for: %d/%d(Child)\n", proc[i].name, j, proc[i].exec); 
				}
#endif
	     	}
		   //syscall(GET_TIME, &end_sec, &end_nsec);
		   //sprintf(to_dmesg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_sec, start_nsec, end_sec, end_nsec);
		   //syscall(PRINTK, to_dmesg);
			 //printf("%s finished execution and will be terminated(Child)\n", proc[i].name);
		   exit(0);
			// printf("%s terminated\n", proc[i].name); NOT executed after exit(0)
			 //cause normal process termination-->the process changed state
     	}
	/* Assign child to another core prevent from interupted by parant */
      proc_assign_cpu(c_pid, CHILD_CPU); 
			proc[i].pid = c_pid;
	    struct sched_param param_2;
	      /* SCHED_IDLE should set priority to 0 */
	      param_2.sched_priority = 0;
        // printf("Put %s to the background...(Parent)\n", proc[i].name);
	      int ret_2 = sched_setscheduler(proc[i].pid, SCHED_IDLE, &param_2);
	
	      if (ret_2 < 0) {
		    perror("sched_setscheduler");
		   // return -1;
       	}
#ifdef DEBUG
				fprintf(stderr, "%s ready at time %d.\n", proc[i].name, ntime);
#endif
			} //
    } //end of iterating all the processes
	// Select next running  process 
	int next;
	//printf("value of running is %d \n", running);	
      /* Return index of next process  */
	    /* Non-preemptive */
		//running is the index of the running process
	if (running != -1 )
	{
		next=running;
	}else{

	int ret = -1;
		for (int i = 0; i < nproc; i++) {
			//iterate through all the processes
			if (proc[i].pid == -1 || proc[i].exec == 0)
			{
				//if the process is NOT ready, continue
				continue;
			}
			if (ret == -1 || proc[i].exec < proc[ret].exec) 
			{ //if ret == -1, it's the start of the program, so select the first process
				//compare execution time; if the exec time of process A is less than that of process B, select A
				//in the for loop, we select process that has min execution time to execute
				ret = i;
			}
		}
	  
		next=ret;
	}
	//===========================================	
		//printf("value of running is %d \n", running); //success
		if (next != -1) {
			/* Context switch */
     // printf("value of next is %d\n", next);//
		 // printf("value of running is %d \n", running); //success
		 // most of the time, next will equal to running, because the child process is still executing
		 // there is no need for context switch
			if (running != next) {
			//	proc_wakeup(proc[next].pid);
    //wake up the next process
		 // printf("value of running is %d \n", running); //-1
				struct sched_param param_3;
	     /* SCHED_OTHER should set priority to 0 */
	      //I use SCHED_FIFO here
				param_3.sched_priority= 99;
//SCHED_OTHER   the standard round-robin time-sharing policy;
        //printf("Put %s to the foreground...\n", proc[next].name);
	      int ret_3 = sched_setscheduler(proc[next].pid, SCHED_FIFO, &param_3);
	      if (ret_3 < 0) {
		    perror("sched_setscheduler");
		   // return -1;
	      }
     //block the running process
			//	proc_block(proc[running].pid);
     //since this is a SJF, so we won't need to put process to background
				running = next;
				//Last context switch time for RR scheduling
				t_last = ntime;
			}
		}
    //printf("the value of running is...%d\n", running);
		/* Run an unit of time */
		UNIT_T();
		if (running != -1){
			proc[running].exec--;
		}  
		ntime++;
		  //if there are no running processes
			//a unit of time still passes
	}
	exit(0);//parent process terminates
}
