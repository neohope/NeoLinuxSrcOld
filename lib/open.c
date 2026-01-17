/*
 * 本文件实现用户空间的 open() 封装：
 * - 使用可变参数传递 mode，并通过 int 0x80 调用内核 sys_open
 * - 在出错时设置 errno 并返回 -1
 */

#define __LIBRARY__
#include <unistd.h>
#include <stdarg.h>

int open(const char * filename, int flag, ...)
{
	register int res;
	va_list arg;

	va_start(arg,flag);
	__asm__("int $0x80"
		:"=a" (res)
		:"0" (__NR_open),"b" (filename),"c" (flag),
		"d" (va_arg(arg,int)));
	if (res>=0)
		return res;
	errno = -res;
	return -1;
}
