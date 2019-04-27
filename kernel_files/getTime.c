#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage int sys_getTime(struct timespec *t){
    getnstimeofday(t);
//    printk("time = %lu.%lu\n", (*t).tv_sec, (*t).tv_nsec);
    return 0;
}

asmlinkage int sys_printk(char* a){
    printk("%s\n", a);
    return 0;
}

