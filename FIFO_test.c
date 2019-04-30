#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include<sys/time.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#define PARENT_CPU 0
#define CHILD_CPU 1

//members of proc
struct process{
	char name[16];
	int ready;
	int exec;
	pid_t pid;
};

int running;
int finish;
int time_cnt;

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

int proc_assign_cpu(int pid, int core){
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
	if(sched_setaffinity(pid, sizeof(mask), &mask)<0){
		fprintf(stderr, "sched_setaffinity error\n");
		exit(1);
	}
	return 0;
}

int proc_wakeup(int pid){
	struct sched_param param;
	param.sched_priority=0;
	int ret=sched_setscheduler(pid, SCHED_OTHER, &param);
	if(ret<0){
		fprintf(stderr, "------wake up error------\n");
		return -1;
	}
	return ret;
}

int proc_exec(struct process proc){
	int pid=fork();
	if(pid<0){
		fprintf(stderr, "fork failed\n");
		return -1;
	}	
	if(pid==0){
		printf("process %s forked    time : %d   pid: %d\n", proc.name, time_cnt, getpid());
		for(int i=0; i < proc.exec; i++){
			time_unit();
		}
		printf("process %s exited     time : %d\n", proc.name, time_cnt);

		exit(0);
	}
	proc_assign_cpu(pid, CHILD_CPU);
	
	return pid;
}

int proc_block(int pid){
	struct sched_param param;
	param.sched_priority=0;
	int ret=sched_setscheduler(pid, SCHED_IDLE, &param);
	if(ret<0) {
		fprintf(stderr, "sched idle error\n");
		return -1;
	}
	return ret;
}

int next_proc(struct process *proc, int n_proc){
	if(running != -1){
		return running;
	}
	int ret=-1;
	for(int i=0; i<n_proc; i++){
		if(proc[i].pid==-1 || proc[i].exec==0) continue;
		if(ret==-1 || proc[i].ready < proc[ret].ready) ret=i;
	}
	return ret;
}

int main(int argc, char *argv[])
{
	//reading  process parmeter
	char policy[16];
	int n_proc;
	struct process *proc;
	FILE *file;
	file=fopen(argv[1], "r");
	if (!file){
	fprintf(stderr, "error: file open failure\n");
		exit(1);
	}
	fscanf(file, "%s", policy);
	fscanf(file, "%d", &n_proc);
	proc=(struct process *)malloc(n_proc*sizeof(struct process));
	for(int i=0; i<n_proc; i++){
		fscanf(file, "%s %d %d", proc[i].name, &proc[i].ready, &proc[i].exec);
 	}

	//sort the processes according to ready order
	qsort(proc, n_proc, sizeof(struct process), compare);
	for(int i=0; i<n_proc; i++){
		printf("%s %d %d\n", proc[i].name, proc[i].ready, proc[i].exec);
	}	
	
	for(int i=0; i<n_proc; i++){
		proc[i].pid=-1;
	}

	proc_assign_cpu(getpid(), PARENT_CPU);
	proc_wakeup(getpid());
	printf("parent CPU assigned\n");

	running=-1;
	time_cnt=0;
	finish=0;

	while(1){
		if(running!=-1 && proc[running].exec==0){
			waitpid(proc[running].pid, NULL, 0);
			printf("process %s ended     time : %d\n", proc[running].name, time_cnt);
			running=-1;
			finish++;
			if(finish==n_proc) break;
		}

		for(int i=0; i<n_proc; i++){
			if(proc[i].ready==time_cnt){
				proc[i].pid=proc_exec(proc[i]);
				proc_block(proc[i].pid);					
			}
		}

		int next=next_proc(proc, n_proc);
		if(next!=-1){
			if (running!=next){
				printf("!!!!!!context switch!!!!!!\n");
				proc_wakeup(proc[next].pid);
				proc_block(proc[running].pid);

				running=next;
			}
		}

		time_unit();
		time_cnt++;

		if(running!=-1){
			proc[running].exec--;
		}
	}

	return 0;
}
