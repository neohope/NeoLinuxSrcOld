/*
 * 本文件封装 dup() 系统调用：
 * - 在用户空间提供复制文件描述符的接口
 */

#define __LIBRARY__
#include <unistd.h>

_syscall1(int,dup,int,fd)
