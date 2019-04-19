#ifndef _PSJF_
#define _PSJF_


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
void PSJF(struct process *proc,int n_proc);

#endif