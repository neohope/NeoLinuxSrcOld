/*
 * 本文件封装 execve() 系统调用：
 * - 在当前进程地址空间内加载并执行新程序
 */

#define __LIBRARY__
#include <unistd.h>

_syscall3(int,execve,const char *,file,char **,argv,char **,envp)
