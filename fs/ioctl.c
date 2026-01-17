/*
 * 本文件是 ioctl 系统调用在 VFS 层的入口：
 * - sys_ioctl()：根据文件描述符找到 inode，并区分管道/字符/块设备
 * - 通过 ioctl_table[] 将命令分发给具体设备（如 TTY、管道等）
 * 统一了不同设备驱动对 ioctl 的处理接口。
 */

#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <linux/sched.h>

extern int tty_ioctl(int dev, int cmd, int arg);
extern int pipe_ioctl(struct m_inode *pino, int cmd, int arg);

typedef int (*ioctl_ptr)(int dev,int cmd,int arg);

#define NRDEVS ((sizeof (ioctl_table))/(sizeof (ioctl_ptr)))

static ioctl_ptr ioctl_table[]={
	NULL,		/* nodev */
	NULL,		/* /dev/mem */
	NULL,		/* /dev/fd */
	NULL,		/* /dev/hd */
	tty_ioctl,	/* /dev/ttyx */
	tty_ioctl,	/* /dev/tty */
	NULL,		/* /dev/lp */
	NULL};		/* named pipes */
	

int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg)
{	
	struct file * filp;
	int dev,mode;

	if (fd >= NR_OPEN || !(filp = current->filp[fd]))
		return -EBADF;
	if (filp->f_inode->i_pipe)
		return (filp->f_mode&1)?pipe_ioctl(filp->f_inode,cmd,arg):-EBADF;
	mode=filp->f_inode->i_mode;
	if (!S_ISCHR(mode) && !S_ISBLK(mode))
		return -EINVAL;
	dev = filp->f_inode->i_zone[0];
	if (MAJOR(dev) >= NRDEVS)
		return -ENODEV;
	if (!ioctl_table[MAJOR(dev)])
		return -ENOTTY;
	return ioctl_table[MAJOR(dev)](dev,cmd,arg);
}
