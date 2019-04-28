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
#include "RR.h"
#include "SJF.h"


int main(int argc, char *argv[])
{
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
    if (strcmp(policy, "PSJF") == 0)
    {    	
        PSJF(proc,n_proc);
    }
    else if (strcmp(policy, "RR") == 0)
    {
        RR_main(proc, n_proc);
    }
    else if(strcmp(policy, "SJF") == 0)
    {
				    
    SJF(proc, n_proc);
		
    }
 
    return 0;
}
