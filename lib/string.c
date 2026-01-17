/*
 * 本文件提供内核/用户态共用的字符串/内存处理函数实现：
 * - 实现 memcpy/strcmp/strlen 等标准 C 字符串接口
 * - 使用 inline/extern 技巧以便被多个目标文件重用
 */

#ifndef __GNUC__
#error I want gcc!
#endif

#define extern
#define inline
#define __LIBRARY__
#include <string.h>
