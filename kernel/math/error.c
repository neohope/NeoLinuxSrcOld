/*
 * linux/kernel/math/error.c
 *
 * (C) 1991 Linus Torvalds
 */

#include <signal.h>

#include <linux/sched.h>

/*
 * 本文件处理 80387 浮点错误：
 * - math_error()：清除协处理器状态，并向最近使用浮点的进程发送 SIGFPE
 */

void math_error(void)
{
	__asm__("fnclex");
	if (last_task_used_math)
		last_task_used_math->signal |= 1<<(SIGFPE-1);
}
