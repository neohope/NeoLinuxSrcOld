# NeoLinuxOld (Linux 0.12) 源码阅读指南

本项目是基于 Linux 0.12 内核源码的注释与分析版本。Linux 0.12 是 Linus Torvalds 在 1992 年初发布的早期版本，它包含了现代操作系统的所有核心要素（保护模式、分页机制、虚拟内存、VFS、进程调度、Minix 文件系统、TTY 终端等），但代码量相对精简（约 1-2 万行），是学习操作系统原理的最佳蓝本。

为了降低阅读门槛，本项目对内核核心模块（启动、调度、内存、文件系统、驱动）的 C 源码进行了系统性的**中文注释**，包括文件级功能说明、关键函数逻辑解析以及设计思想的阐述。

## 1. 目录结构

内核源码主要分布在以下目录中：

| 目录 | 说明 |
| :--- | :--- |
| **boot/** | **启动代码**。包含 BIOS 引导扇区代码 (`bootsect.s`)、硬件检测与保护模式切换 (`setup.s`) 以及内核入口汇编 (`head.s`)。 |
| **init/** | **内核初始化**。仅包含 `main.c`，是内核从汇编跳转到 C 语言后的第一个执行点，负责初始化所有子系统并创建进程 0。 |
| **kernel/** | **核心代码**。包含进程调度 (`sched.c`)、系统调用 (`sys.c`)、异常处理 (`traps.c`)、信号 (`signal.c`) 等核心逻辑。 |
| ├── **blk_drv/** | **块设备驱动**。硬盘 (`hd.c`)、软盘 (`floppy.c`)、RAM 盘 (`ramdisk.c`) 及请求队列管理 (`ll_rw_blk.c`)。 |
| ├── **chr_drv/** | **字符设备驱动**。控制台 (`console.c`)、串口 (`serial.c`)、TTY 终端子系统 (`tty_io.c`)。 |
| └── **math/** | **数学协处理器仿真**。针对无 FPU 的 CPU 提供浮点指令的软件仿真 (`math_emulate.c`)。 |
| **mm/** | **内存管理**。物理内存管理、页表映射、缺页异常处理 (`memory.c`) 以及交换空间管理 (`swap.c`)。 |
| **fs/** | **文件系统**。虚拟文件系统 (VFS) 框架及 Minix 文件系统实现。包含缓冲区 (`buffer.c`)、Inode (`inode.c`)、文件操作 (`file_dev.c`)、Exec (`exec.c`) 等。 |
| **lib/** | **内核库函数**。供内核使用的基础函数（如 `string.c`、`malloc.c`）以及部分系统调用的用户态封装（如 `open.c`、`write.c`）。 |
| **include/** | **头文件**。定义了内核数据结构、常量和宏。`include/linux/` 下为内核专用，`include/asm/` 为架构相关。 |
| **tools/** | **构建工具**。包含 `build.c`，用于将编译好的各部分目标文件拼装成最终的可引导内核镜像。 |

## 2. 内核启动流程 (Boot Process)

1.  **BIOS 加载**: PC 启动后，BIOS 将 `boot/bootsect.s` 加载到内存 `0x7c00` 并跳转执行。
2.  **Self-Move & Load**: `bootsect` 将自己移动到 `0x90000`，然后加载 `setup.s` 到 `0x90200`，加载 `system` (内核主体) 到 `0x10000`。
3.  **Setup**: 跳转到 `setup.s`，获取机器系统数据（内存大小、显卡参数等），关闭中断，将 `system` 移动到物理地址 `0x00000`。
4.  **保护模式**: `setup.s` 设置 IDT/GDT，开启 A20 地址线，设置 CR0 寄存器进入 32 位保护模式，跳转到 `0x00000` (即 `head.s`)。
5.  **Head**: `boot/head.s` 初始化页目录表和页表，开启分页机制，重新设置 GDT/IDT，最后压栈 `main` 函数地址并执行 `ret`，跳转到 `init/main.c`。
6.  **Main**: `main()` 函数初始化内存、中断、块设备、字符设备、TTY、时间等，切换到用户模式 (Ring 3)，`fork` 出进程 1 (`init`)。
7.  **Init**: 进程 1 执行 `/bin/sh` 或 `/etc/rc`，启动 Shell，系统启动完成。

## 3. 关键文件导读

以下文件已添加详细中文注释，建议按顺序阅读：

### 核心层
*   [init/main.c](init/main.c): **内核主程序**。理解 Linux 初始化全过程的最佳入口。
*   [kernel/sched.c](kernel/sched.c): **调度器**。核心函数 `schedule()` 和 `sleep_on()`，理解进程切换和等待队列。
*   [kernel/sys.c](kernel/sys.c): **系统调用**。包含 `sys_uid` 等杂项系统调用，以及系统调用表。
*   [kernel/exit.c](kernel/exit.c): **进程退出**。`do_exit()` 和 `sys_waitpid()`，理解僵尸进程和资源回收。
*   [kernel/fork.c](kernel/fork.c): **进程创建**。`copy_process()`，理解进程描述符 (`task_struct`) 的复制。

### 内存管理
*   [mm/memory.c](mm/memory.c): **缺页处理**。`do_no_page()` (缺页加载) 和 `do_wp_page()` (写时复制 COW) 的实现。
*   [mm/swap.c](mm/swap.c): **交换机制**。内存不足时如何将页面换出到磁盘。

### 文件系统
*   [fs/buffer.c](fs/buffer.c): **高速缓冲区**。`getblk()` 和 `brelse()`，理解磁盘块在内存中的缓存机制。
*   [fs/inode.c](fs/inode.c): **Inode 管理**。`iget()` 和 `iput()`，文件系统元数据的内存管理。
*   [fs/super.c](fs/super.c): **超级块管理**。文件系统的挂载 (`sys_mount`) 和卸载。
*   [fs/exec.c](fs/exec.c): **程序执行**。`do_execve()`，理解如何加载 ELF/a.out 可执行文件并替换当前进程镜像。

### 驱动程序
*   [kernel/blk_drv/ll_rw_blk.c](kernel/blk_drv/ll_rw_blk.c): **块设备请求**。`make_request()`，理解 I/O 电梯调度算法。
*   [kernel/chr_drv/console.c](kernel/chr_drv/console.c): **控制台**。`con_write()`，理解字符如何显示到屏幕（含 VT102 转义序列处理）。
*   [kernel/chr_drv/tty_io.c](kernel/chr_drv/tty_io.c): **TTY 子系统**。终端设备的通用读写接口。

## 4. 学习建议

1.  **从 Main 开始**: 先看 `init/main.c`，了解系统初始化做了什么。
2.  **理解调度**: 接着看 `kernel/sched.c`，搞懂 Linux 0.12 简单的链表调度和状态切换。
3.  **攻克内存**: `mm/memory.c` 中的写时复制 (COW) 和缺页加载是操作系统的精髓。
4.  **文件系统**: 结合 `fs/buffer.c` 和 `fs/inode.c` 理解数据是如何从磁盘“流动”到用户空间的。
5.  **动手修改**: 尝试修改 `printk` 输出，或者在 `sys_call_table` 中增加一个简单的系统调用。

---
*Happy Hacking!*
