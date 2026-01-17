/*
 * 本文件封装 wait()/waitpid()：
 * - 通过 _syscall3 调用内核 sys_waitpid
 * - wait() 基于 waitpid(-1, ...) 实现，等待任意子进程结束
 */

#define __LIBRARY__
#include <unistd.h>
#include <sys/wait.h>

_syscall3(pid_t,waitpid,pid_t,pid,int *,wait_stat,int,options)

pid_t wait(int * wait_stat)
{
	return waitpid(-1,wait_stat,0);
}
