/*
 * 本文件封装 setsid() 系统调用：
 * - 创建新会话并使当前进程成为会话首进程
 */

#define __LIBRARY__
#include <unistd.h>

_syscall0(pid_t,setsid)
