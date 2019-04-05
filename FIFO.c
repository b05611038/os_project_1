#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

//define the members of a process
struct process{
	char name[16];
	int ready;
	int exec;
	pid_t pid;
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
		exit(1);
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
	/*debug
	for(int i=0; i<n_proc; i++){
		printf("%s %d %d\n", proc[i].name, proc[i].ready, proc[i].exec);
	}
	printf("%s\n%d\n\n", policy, n_proc);
	*/

	//start processing
	if(strcmp(policy, "FIFO")!=0){
		printf("policy other than FIFO, or invalid policy\n");
		exit(1);
	}else{
		//sort the processes according to ready order
		qsort(proc, n_proc, sizeof(struct process), compare);
		//debub	
		/*
		for(int i=0; i<n_proc; i++){
			printf("%s %d %d\n", proc[i].name, proc[i].ready, proc[i].exec);
		}
		*/

		//assign to one cpu
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(0, &mask);
		if(sched_setaffinity(getpid(), sizeof(mask), &mask)<0){
			fprintf(stderr, "sched_setaffinity error\n");
			exit(1);
		}

		//set priority
		struct sched_param param;
		param.sched_priority=0;
		if(sched_setscheduler(getpid(), SCHED_OTHER, &param)<0){
			fprintf(stderr, "sched_setscheduler error\n");
		}

		//start counting time
		int time_cnt=0;
		//record current process's number
		int tmp=0;
		//start time
		int start_t=proc[0].ready;
		//end time
		int end_t=start_t+proc[0].exec;

		//run one unit of time for each loop
		while(1){

			if(time_cnt==start_t && time_cnt==end_t){
				//kill child process
				//printf("time : %d    %s killed\n", time_cnt, proc[tmp].name);   //debug
				// set new tmp
				tmp++;
				// set new end_t
				end_t=time_cnt+proc[tmp].exec;
				// set new start_t
				if(tmp+1>=n_proc){
						
				}else if(proc[tmp+1].ready>end_t){
					start_t=proc[tmp+1].ready;
				}else{
					start_t=end_t;
				}
				// fork a child
				//proc[tmp].pid=fork();
				//printf("time : %d    %s created\n", time_cnt, proc[tmp].name);  //debug
		
			}else if(time_cnt==start_t){
				// set new start_t
				if(tmp+1>=n_proc){
					
				}else if(proc[tmp+1].ready>end_t){
					start_t=proc[tmp+1].ready;
				}else{
					start_t=end_t;
				}
				// fork a child
				//proc[tmp].pid=fork();
				//printf("time : %d    %s created\n", time_cnt, proc[tmp].name);  //debug

			}else if(time_cnt==end_t){
				// kill child process
				//printf("time : %d    %s killed\n", time_cnt, proc[tmp].name);   //debug
				// set new tmp
				tmp++;
				// set new end_t
				end_t=start_t+proc[tmp].exec;

			}else if(time_cnt!=start_t && time_cnt!=end_t){

			}else{
				printf("logic error\n");
				exit(0);
			}

			time_unit();
			time_cnt++;
			if(tmp==n_proc) break;   //if the last process has finished its job, leave while loop and end main
		}
	}
	return 0;
}
