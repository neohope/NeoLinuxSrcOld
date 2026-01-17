/*
 * 本文件通过 _syscall3 宏封装 write() 系统调用：
 * - 在用户空间提供与 POSIX 兼容的 write 接口
 */

#define __LIBRARY__
#include <unistd.h>

_syscall3(int,write,int,fd,const char *,buf,off_t,count)
