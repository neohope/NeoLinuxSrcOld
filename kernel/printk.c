/*
 * 本文件实现内核打印接口 printk()：
 * - 使用内部 buffer 格式化字符串（调用 vsprintf）
 * - 调用 console_print() 将结果输出到控制台
 * - 是内核调试和信息输出的主要手段
 */

#include <stdarg.h>
#include <stddef.h>

#include <linux/kernel.h>

static char buf[1024];

extern int vsprintf(char * buf, const char * fmt, va_list args);

int printk(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	console_print(buf);
	return i;
}
