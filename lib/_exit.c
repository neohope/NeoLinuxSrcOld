/*
 * 本文件提供系统调用 _exit() 的最小封装：
 * - 直接通过 int 0x80 触发内核的 sys_exit
 * - 不做标准库清理，仅立即终止当前进程
 */

#define __LIBRARY__
#include <unistd.h>

volatile void _exit(int exit_code)
{
	__asm__("int $0x80"::"a" (__NR_exit),"b" (exit_code));
}
