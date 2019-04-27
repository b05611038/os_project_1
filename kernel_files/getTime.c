#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage int sys_getTime(struct timespec *t){
    getnstimeofday(t);
    return 0;
}

asmlinkage int sys_printk(char* input){
    printk("%s\n", input);
    return 0;
}

