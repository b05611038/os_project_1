#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static int interval = 300;

int time_now;

int time_last;

int running;

int finish_cnt;

int compare(const void *a, const void *b) {
  return ((struct Process *)a) -> ready - ((struct Process *)b) -> ready;
}

int RR_next(struct Process *proc, int num_process) {
  int ret = -1;

  if (running == -1) {
    for (int i = 0; i < num_process; i++) {
      if (proc[i].pid != -1 && proc[i].remain > 0){
        ret = i;
        break;
      }
    }
  } else if ((time_now - time_last) % interval == 0) {
      ret = (running + 1) % num_process;
      while (proc[ret].pid == -1 || proc[ret].remain == 0)
        
        ret = (ret + 1) % num_process;
    } else {
      ret = running;
  }

  return ret;
}

int proc_assign_cpu(int pid, int core) {
  if (core > sizeof(cpu_set_t)) {
    fprintf(stderr, "Core index error.");
    return -1;
  }

  /* init CPU */
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(core, &mask);

  if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
    perror("sched_setaffinity");
    exit(1);
  }

  return 0;
}

int proc_exec(struct Process proc) {
  int pid = fork();

  if (pid < 0) {
    perror("fork");
    return -1;
  }

  if (pid == 0) {
    for (int i = 0; i < proc.remain; i++) {
      Unit_time();
      exit(0);
    }
  }

  proc_assign_cpu(pid, CHILD_CPU);

  return pid;
}

int proc_block(int pid) {
  struct sched_param param;

  param.sched_priority = 0;

  int ret = sched_setscheduler(pid, SCHED_IDLE, &param);

  if (ret < 0) {
    perror("sched_setscheduler");
    return -1;
  }

  return ret;
}

int proc_wakeup(int pid) {
  struct sched_param param;

  param.sched_priority = 0;

  int ret = sched_setscheduler(pid, SCHED_OTHER, &param);

  if (ret < 0) {
    perror("sched_setscheduler");
    return -1;
  }

  return ret;
}

//int main(int argc, char **argv) {
void RR(struct Process *proc, int num_process)
  /*
  if (argc != 2) {
    printf("Usage: ./RR.out [input file name]\n");
    exit(1);
  }

  // read file
  char sched_policy[8];
  int policy;
  int num_process;
  struct Process *proc;

  FILE *file;
  file = fopen(argv[1], "r");
  if (!file) {
    fprintf(stderr, "STDError: Can not open file.\n");
    exit(1);
  }

  fscanf(file, "%s", sched_policy);
  fscanf(file, "%d", &num_process);
  proc = (struct Process *)malloc(num_process * sizeof(struct Process));
  for (int i = 0; i < num_process; i++) {
    fscanf(file, "%s %d %d", proc[i].name, &proc[i].ready, &proc[i].remain);
  }

  // check valid policy for RR scheduling
  if (strcmp(sched_policy, "RR") == 0) {
    policy = RR;
  } else {
    fprintf(stderr, "Invalid policy: %s.\n", sched_policy);
    exit(1);
  }
  */

  //sort process by ready time
  qsort(proc, num_process, sizeof(struct Process), compare);

  //initial PID
  for (int i = 0; i < num_process; i++) {
    proc[i].pid = -1;
  }

  int p_pid = getpid();
  proc_assign_cpu(p_pid, PARENT_CPU);


  //set priority
  struct sched_param para;
  para.sched_priority = 0;
  if (sched_setscheduler(p_pid, SCHED_OTHER, &para) < 0) {
    fprintf(stderr, "sched_scheduler error.\n");
    exit(1);
  }

  /* initial time */
  time_now = 0;
  time_last = 0;
  running = -1;
  finish_cnt = 0;
  int wait = 0;

  while(1) {
    /* check the finish of the loop */
    if (running != -1 && proc[running].remain == 0) {
      waitpid(proc[running].pid, NULL, 0);
      printf("Finish %s %d %d\n", proc[running].name, proc[running].pid, time_now);
      /*
      syscall block (finish time)
      */

      wait = wait + time_now - proc[running].ready - proc[running].remain;
      running = -1;
      finish_cnt++;

      if (finish_cnt == num_process)
        break;
    }

    /* check process ready and excute */
    for (int i = 0; i < num_process; i++) {
      if (proc[i].ready == time_now) {
        proc[i].pid = proc_exec(proc[i]);
        proc_block(proc[i].pid);

	printf("%s ready at time %d.\n", proc[i].name, time_now);
        /*
        syscall for pid can start
        */
      }
    }

    /* RR logic */
    int next = RR_next(proc, num_process);

    if (next != -1) {
      if (running != next) {
        proc_wakeup(proc[next].pid);
        proc_block(proc[running].pid);
        running = next;
        time_last = time_now;
      }
    }

    /* run actual time */
    Unit_time();
    if (running != -1) {
      proc[running].remain--;
    }
    time_now++;

  }

  float avg_wait = (float)wait / (float)num_process;
  printf("Average waiting time: %.2f\n", avg_wait);
  //return 0;
}


