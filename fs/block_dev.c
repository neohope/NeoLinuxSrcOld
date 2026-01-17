/*
 * 本文件封装对块设备的顺序读写操作：
 * - block_read()/block_write()：按字节偏移访问块设备
 * - 内部使用缓冲区层（getblk/bread/brelse）读写扇区
 * - 按块大小对齐访问范围，并进行简单的越界检查
 */

#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>

extern int *blk_size[];

int block_write(int dev, long * pos, char * buf, int count)
{
	int block = *pos >> BLOCK_SIZE_BITS;
	int offset = *pos & (BLOCK_SIZE-1);
	int chars;
	int written = 0;
	int size;
	struct buffer_head * bh;
	register char * p;

	if (blk_size[MAJOR(dev)])
		size = blk_size[MAJOR(dev)][MINOR(dev)];
	else
		size = 0x7fffffff;
	while (count>0) {
		if (block >= size)
			return written?written:-EIO;
		chars = BLOCK_SIZE - offset;
		if (chars > count)
			chars=count;
		if (chars == BLOCK_SIZE)
			bh = getblk(dev,block);
		else
			bh = breada(dev,block,block+1,block+2,-1);
		block++;
		if (!bh)
			return written?written:-EIO;
		p = offset + bh->b_data;
		offset = 0;
		*pos += chars;
		written += chars;
		count -= chars;
		while (chars-->0)
			*(p++) = get_fs_byte(buf++);
		bh->b_dirt = 1;
		brelse(bh);
	}
	return written;
}

int block_read(int dev, unsigned long * pos, char * buf, int count)
{
	int block = *pos >> BLOCK_SIZE_BITS;
	int offset = *pos & (BLOCK_SIZE-1);
	int chars;
	int size;
	int read = 0;
	struct buffer_head * bh;
	register char * p;

	if (blk_size[MAJOR(dev)])
		size = blk_size[MAJOR(dev)][MINOR(dev)];
	else
		size = 0x7fffffff;
	while (count>0) {
		if (block >= size)
			return read?read:-EIO;
		chars = BLOCK_SIZE-offset;
		if (chars > count)
			chars = count;
		if (!(bh = breada(dev,block,block+1,block+2,-1)))
			return read?read:-EIO;
		block++;
		p = offset + bh->b_data;
		offset = 0;
		*pos += chars;
		read += chars;
		count -= chars;
		while (chars-->0)
			put_fs_byte(*(p++),buf++);
		brelse(bh);
	}
	return read;
}
