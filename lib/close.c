/*
 * 本文件封装 close() 系统调用：
 * - 关闭给定文件描述符
 */

#define __LIBRARY__
#include <unistd.h>

_syscall1(int,close,int,fd)
