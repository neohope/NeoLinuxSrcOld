/*
 * 本文件实现伪终端（PTY）设备：
 * - 成对提供 master/slave 终端，用于用户态程序模拟终端会话
 * - mpty_write()/spty_write()：在 master 与 slave 之间转发数据
 * - pty_copy()：从一端的写队列复制数据到另一端的读队列
 */

#include <linux/tty.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <asm/io.h>

static inline void pty_copy(struct tty_struct * from, struct tty_struct * to)
{
	char c;

	while (!from->stopped && !EMPTY(from->write_q)) {
		if (FULL(to->read_q)) {
			if (FULL(to->secondary))
				break;
			copy_to_cooked(to);
			continue;
		}
		GETCH(from->write_q,c);
		PUTCH(c,to->read_q);
		if (current->signal & ~current->blocked)
			break;
	}
	copy_to_cooked(to);
	wake_up(&from->write_q->proc_list);
}

/*
 * This routine gets called when tty_write has put something into
 * the write_queue. It copies the input to the output-queue of it's
 * slave.
 */
void mpty_write(struct tty_struct * tty)
{
	int nr = tty - tty_table;

	if ((nr >> 6) != 2)
		printk("bad mpty\n\r");
	else
		pty_copy(tty,tty+64);
}

void spty_write(struct tty_struct * tty)
{
	int nr = tty - tty_table;

	if ((nr >> 6) != 3)
		printk("bad spty\n\r");
	else
		pty_copy(tty,tty-64);
}
