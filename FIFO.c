#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "PSJF.h"
#include "FIFO.h"



//define one unit of time
void time_unit()
{
	volatile unsigned long i;
	for(i=0; i<1000000UL; i++);
}

//function called by qsort
int compare(const void *p1, const void *p2){
	if(((struct process *)p1)->ready < ((struct process *)p2)->ready) return -1;
	if(((struct process *)p1)->ready == ((struct process *)p2)->ready) return 0;
	if(((struct process *)p1)->ready > ((struct process *)p2)->ready) return 1;
}


void FIFO(struct process *proc, int n_proc){
	//sort the processes according to ready order
	qsort(proc, n_proc, sizeof(struct process), compare);

	//debug	
	for(int i=0; i<n_proc; i++){
		printf("%s %d %d\n", proc[i].name, proc[i].ready, proc[i].exec);
	}
	
	for(int i=0; i<n_proc; i++){
		proc[i].pid=-1;
	}

	//assign to one cpu
	pid_t p_pid=getpid();   //get the parent pid
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(PARENT_CPU, &mask);
	if(sched_setaffinity(p_pid, sizeof(mask), &mask)<0){
		fprintf(stderr, "sched_setaffinity error\n");
		exit(1);
	}

	//set priority
	struct sched_param param;
	param.sched_priority=0;
	if(sched_setscheduler(p_pid, SCHED_OTHER, &param)<0){
		fprintf(stderr, "sched_setscheduler error\n");
		exit(1);
	}

	int running=0;
	int time_cnt=0;
	
	while(1){
		if(proc[running].exec==0){
			waitpid(proc[running].pid, NULL, 0);
			printf("process %s ended     time : %d\n", proc[running].name, time_cnt);
			running++;
			if(running==n_proc) break;
		}

		for(int i=0; i<n_proc; i++){
			if(proc[i].ready==time_cnt){
				proc[i].pid=fork();
				if(proc[i].pid<0){
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				if(proc[i].pid==0){
					printf("process %s forked    time : %d   pid: %d\n", proc[i].name, time_cnt, getpid());
					for(int j=0; j < proc[running].exec; j++){
						time_unit();
						//time_cnt++;
					}
					printf("process %s exited     time : %d\n", proc[i].name, time_cnt);
					exit(0);
				}
				//set CPU for child process
				cpu_set_t mask2;
				CPU_ZERO(&mask2);
				CPU_SET(CHILD_CPU, &mask2);
				if(sched_setaffinity(proc[i].pid, sizeof(mask2), &mask2)<0){
					fprintf(stderr, "sched_setaffinity error\n");
					exit(1);
				}
		
				struct sched_param param2;
				param2.sched_priority=0;
				if(sched_setscheduler(proc[i].pid, SCHED_IDLE, &param2)<0){
					fprintf(stderr, "sched_setscheduler2 error\n");
					exit(1);
				}	
			}
		}
		if(proc[running+1].pid!=-1 && proc[running].exec==0){
			running++;
			struct sched_param param3;
			param3.sched_priority=0;
			if(sched_setscheduler(proc[running].pid, SCHED_OTHER, &param3)<0){
				fprintf(stderr, "sched_setscheduler3 error\n");
				exit(1);
			}
		
		}
		time_unit();
		time_cnt++;
		if(proc[running].pid!=-1){
			proc[running].exec--;
		}
	}
	return 0;
}
