/*
 * 本文件维护系统级的打开文件表：
 * - file_table[]：保存所有打开文件的 struct file 结构
 * - 每个进程的 filp[] 数组持有指向 file_table[] 元素的指针
 * 通过引用计数控制文件对象的共享与释放。
 */

#include <linux/fs.h>

struct file file_table[NR_FILE];
