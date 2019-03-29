#include<linux/kernel.h>
#include<linux/linkage.h>
#include<linux/ktime.h>
#include<linux/syscalls.h>
SYSCALL_DEFINE2( my_syscall_0, unsigned long*, a, unsigned long*, b)
{
struct timespec t;
getnstimeofday(&t);
//maybe we should try to pass value from user space to kernel space
//pass the pid value to the kernel
//but we must pass the 
*a=t.tv_sec;
*b=t.tv_nsec;
//printk("%lu.%09lu\n", *a, *b); //184455567.184455567 WRONG
//printk("%lu.%09lu\n", t.tv_sec, t.tv_nsec);//1553781770.184455567 CORRECT
//printk("HAHAHAHAHAHA");
return 0;


}


SYSCALL_DEFINE1( my_syscall_1,char*, a)
{
//int size= sizeof(*a)/sizeof(char);// size==1 WRONG!
//printk("size= %d", size); //
printk("%s\n", a); //\xffffffb5\xffffffec\xffffffd6 ?????WHY
//int i;
//for(i=0 ; i< size; i++){
//printk("%c", a[i]);
//}

//printk("HAHA, %d\n", *a);
return 0;

}
