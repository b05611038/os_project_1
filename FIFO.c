#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PARENT_CPU 0
#define CHILD_CPU 1

//define the members of a process
struct process{
	char name[16];
	int ready;
	int exec;
	//pid_t pid;
};

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


int main(int argc, char *argv[]){

	if(argc!=2){
		printf("\ninput format : ./a.out [input file name]\neg: ./a.out testcase_1.txt\n\n");
		exit(0);
	}

	//read in test file
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
	

	//start processing
	if(strcmp(policy, "FIFO")!=0){
		printf("policy other than FIFO, or invalid policy\n");
		exit(0);
	}
	//sort the processes according to ready order
	qsort(proc, n_proc, sizeof(struct process), compare);

	//debug	
	for(int i=0; i<n_proc; i++){
		printf("%s %d %d\n", proc[i].name, proc[i].ready, proc[i].exec);
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

	while(running<n_proc){
		if((proc[running].ready-time_cnt)>0){
			int temp=time_cnt;
			for(int i=0; i < (proc[running].ready-temp); i++){
				time_unit();
				time_cnt++;
			}
		}
					
		int c_pid=fork();
		if(c_pid<0){
			fprintf(stderr, "fork failed\n");
			exit(1);
		}
		if(c_pid==0){
			printf("process %s started    time : %d   pid: %d\n", proc[running].name, time_cnt, getpid());

			for(int j=0; j < proc[running].exec; j++){
				time_unit();
				time_cnt++;
			}

			printf("process %s ended      time : %d\n", proc[running].name, time_cnt);
			
			exit(0);
		}else{
			
			time_cnt=time_cnt+proc[running].exec;

			//set CPU for child process
			cpu_set_t mask2;
			CPU_ZERO(&mask2);
			CPU_SET(CHILD_CPU, &mask2);
			if(sched_setaffinity(c_pid, sizeof(mask2), &mask2)<0){
				fprintf(stderr, "sched_setaffinity error\n");
				exit(1);
			}
		
			struct sched_param param2;
			param2.sched_priority=0;
			if(sched_setscheduler(c_pid, SCHED_OTHER, &param2)<0){
				fprintf(stderr, "sched_setscheduler error\n");
				exit(1);
			}

			waitpid(c_pid, NULL, 0);

		}
		
		running++;

	}


	
	return 0;
}
