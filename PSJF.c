#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include<sys/time.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <limits.h>
#include <time.h>

struct process{
    char name[20];
    int ready;
    int exec;
    int waiting;
    pid_t pid;
};
void unit_time();
int process_ready(struct process pros);
int proc_assign_cpu(int pid, int core);
int find_min_index(struct process *pross,int npc);
bool some_process_is_ready(struct process *pross,int npc);

int main(int argc, char *argv[])
{
    //initializing    
    int time = 0;
    int pre_running=-1;
    int running =-1;
    int finished_process=0;
    proc_assign_cpu(getpid(), 0);
    struct sched_param block, high;
    block.sched_priority=10;
    high.sched_priority=90;

    //reading  process parmeter
    char policy[16];
    int n_proc;
    struct process *proc;
    FILE *file;
    file=fopen(argv[1], "r");
    if (!file)
    {
        fprintf(stderr, "error: file open failure\n");
        exit(1);
    }
    fscanf(file, "%s", policy);
    fscanf(file, "%d", &n_proc);
    proc=(struct process *)malloc(n_proc*sizeof(struct process));
    for(int i=0; i<n_proc; i++)
    {
        fscanf(file, "%s %d %d", proc[i].name, &proc[i].ready, &proc[i].exec);
    }

    //check if prcoess is ready, fork it if it is ready.
    while(finished_process!=n_proc)
    {
        for(int i=0;i<n_proc;i++)
        {
    		if(time==proc[i].ready)
            {
                proc[i].pid =  process_ready(proc[i]);
                printf("%d\n",(proc+i)->pid);
                proc[i].waiting=1;
                int ret = sched_setscheduler(proc[i].pid, SCHED_FIFO, &block);
        		if (ret==-1)
                {
                    printf("error");
                }
            }
        }
            
        //chose one process to run
        if (some_process_is_ready(proc,n_proc))
        {
            int index = find_min_index(proc,n_proc);
            running=proc[index].pid;
            if (pre_running==-1)
            {
                pre_running=running;
            }
                //printf("running:%d and pre_running:%d\n",running,pre_running);
            if(running!=pre_running)
            {
        
                int ret2 = sched_setscheduler(running, SCHED_FIFO, &high);
                if (ret2==-1)
                {
                    printf("error");
                }
                int ret3 = sched_setscheduler(pre_running, SCHED_FIFO, &block);
                if (ret3==-1)
                {
                    printf("error");
                }
            }
            //update 
            pre_running=running;
            proc[index].exec--;
            if (proc[index].exec==0)
            {
                proc[index].waiting=0;
                finished_process++;
            }
        }
        unit_time();
        time++;    
    }
    return 0;
}



void unit_time()
    {
	   volatile unsigned long i; for(i=0;i<10000000UL;i++);
	}

int process_ready(struct process pros)
    {
        int pid = fork();
        if (pid == 0) 
        {
            char info[1000];
            struct timespec start,end;
            //syscall
            //syscall(333,&start);
            //syscall
            printf("I am a child process my name is: %s\n",pros.name);
            for (int i = 0; i < pros.exec; i++)
            {
                unit_time();
                //printf("I am : %c \n",name);
            }

            printf("process name:%s is finished\n",pros.name);
            //syscall
            //syscall(333,&end);
            //sprintf(info, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
            //syscall(334,info);
            //syscall
            exit(0);
        }
        proc_assign_cpu(pid, 1);
        return pid;
    }

int proc_assign_cpu(int pid, int core)
    {
        if (core > sizeof(cpu_set_t)) 
        {
            fprintf(stderr, "Core index error.");
            return -1;
        }
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(core, &mask);   
        if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) 
        {
            perror("sched_setaffinity");
            exit(1);
        }

        return 0;
    }

int find_min_index(struct process *pross,int npc)
    {
        int index=0;
        int min=INT_MAX;
        for (int i =0;i<npc;i++)
        {
            int value =  (pross+i)->exec;
            if (value<min && (pross+i)->waiting==1)
            {
                min=value;
                index=i;
            }    
        }
        return index;
    }


bool some_process_is_ready(struct process *pross,int npc)
    {
        for (int i =0;i<npc;i++)
        {
            if ((pross+i)->waiting==1)
            {
                return true;
            }
        }
        return false;
    }
