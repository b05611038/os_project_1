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
#include <string.h>
#include "PSJF.h"



void PSJF(struct process *proc,int n_proc)
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
                int ret = sched_setscheduler((proc+i)->pid, SCHED_FIFO, &block);
        		if (ret==-1)
                {
                    printf("block error\n");
                }
            }
        }
        //printf("test:%d",some_process_is_ready(proc,n_proc));   
        //chose one process to run
        if (some_process_is_ready(proc,n_proc))
        {   
            int index = find_min_index(proc,n_proc);
            //printf("%d",index);
            running=proc[index].pid;

                //printf("running:%d and pre_running:%d\n",running,pre_running);
            if(running!=pre_running)
            {
                if(pre_running!=-1)
                {
                    int ret2 = sched_setscheduler(pre_running, SCHED_FIFO, &high);
                    if (ret2==-1)
                    {
                        printf("pre_running error%d",pre_running);
                    }
                }
                int ret3 = sched_setscheduler(running, SCHED_FIFO, &block);
                if (ret3==-1)
                {
                    printf("running error:%d ",running);
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
                //printf("I am : %s \n",pros.name);
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
