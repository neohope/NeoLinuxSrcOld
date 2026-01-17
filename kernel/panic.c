/*
 * 本文件实现内核致命错误处理函数 panic()：
 * - 当内核遇到无法恢复的错误时被调用
 * - 打印错误信息，尝试同步磁盘（sys_sync），然后死循环停机
 */

#include <linux/kernel.h>
#include <linux/sched.h>

void sys_sync(void);	/* it's really int */

volatile void panic(const char * s)
{
	printk("Kernel panic: %s\n\r",s);
	if (current == task[0])
		printk("In swapper task - not syncing\n\r");
	else
		sys_sync();
	for(;;);
}
