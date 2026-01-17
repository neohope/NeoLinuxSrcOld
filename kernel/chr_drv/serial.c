/*
 * 本文件是串行端口（rs232）驱动：
 * - rs_init()：初始化串口控制器和中断向量
 * - rs_write()：从 TTY 写队列中取数据，触发串口发送中断
 * - 中断处理函数：响应串口接收/发送中断，将数据送入 TTY 读队列
 */

#include <linux/tty.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <asm/io.h>

#define WAKEUP_CHARS (TTY_BUF_SIZE/4)

extern void rs1_interrupt(void);
extern void rs2_interrupt(void);

static void init(int port)
{
	outb_p(0x80,port+3);	/* set DLAB of line control reg */
	outb_p(0x30,port);	/* LS of divisor (48 -> 2400 bps */
	outb_p(0x00,port+1);	/* MS of divisor */
	outb_p(0x03,port+3);	/* reset DLAB */
	outb_p(0x0b,port+4);	/* set DTR,RTS, OUT_2 */
	outb_p(0x0d,port+1);	/* enable all intrs but writes */
	(void)inb(port);	/* read data port to reset things (?) */
}

void rs_init(void)
{
	set_intr_gate(0x24,rs1_interrupt);
	set_intr_gate(0x23,rs2_interrupt);
	init(tty_table[64].read_q->data);
	init(tty_table[65].read_q->data);
	outb(inb_p(0x21)&0xE7,0x21);
}

/*
 * This routine gets called when tty_write has put something into
 * the write_queue. It must check wheter the queue is empty, and
 * set the interrupt register accordingly
 *
 *	void _rs_write(struct tty_struct * tty);
 */
void rs_write(struct tty_struct * tty)
{
	cli();
	if (!EMPTY(tty->write_q))
		outb(inb_p(tty->write_q->data+1)|0x02,tty->write_q->data+1);
	sti();
}
