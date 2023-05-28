# Lab5 实验报告

## 思考题

### Thinking 5.1

对于读取操作，如果设备对应IO接口的值是“可变的”，如RTC时钟、串口设备等，通过cache读取时可能无法获取最新的数据。对于写入操作，如果在cache的数据未同步时写入，则最后同步时实际仅最后一次写操作有效。

### Thinking 5.2

文件控制块256字节，磁盘块4096字节，得1个磁盘块中最多能存储 4096/256=16 个文件控制块。一个indirect磁盘块可储存1024个磁盘块指针，则1个目录下最多能有 1024\*16=16384 个文件。支持的单个文件最大为 1024\*4096=4MB 。

### Thinking 5.3

```c
#define DISKMAX 0x40000000
```

1GB。

### Thinking 5.4

```c
// fs/serv.h
#define PTE_DIRTY 0x0002 // file system block cache is dirty
#define BY2SECT 512		    /* Bytes per disk sector */
#define SECT2BLK (BY2BLK / BY2SECT) /* sectors to a block */
/* Maximum disk size we can handle (1GB) */
#define DISKMAX 0x40000000

// user/include/fs.h
// Number of (direct) block pointers in a File descriptor
#define NDIRECT 10
#define NINDIRECT (BY2BLK / 4) // 1024
struct File {
    ......
	uint32_t f_direct[NDIRECT];
	uint32_t f_indirect;
	......
	char f_pad[BY2FILE - MAXNAMELEN - (3 + NDIRECT) * 4 - sizeof(void *)]; // padding
} __attribute__((aligned(4), packed));

#define FILE2BLK (BY2BLK / sizeof(struct File)) // 16

```

### Thinking 5.5

会。因为进程的文件描述符储存在虚拟地址`FDTABLE`处，实际上是fs服务进程共享给用户进程的页面（具有`PTE_LIBRARY`位），因此在fork时此处内存被共享给子进程，相当于父子进程共享了文件描述符。

### Thinking 5.6

```c
struct Fd {
	u_int fd_dev_id; // 文件对应设备类型
	u_int fd_offset; // 文件的读写指针，即偏移量
	u_int fd_omode; // 文件打开方式
};
struct Filefd {
	struct Fd f_fd; // 文件描述符
	u_int f_fileid; // 文件编号
	struct File f_file; // 文件控制块，是磁盘块的内存缓存
};
struct Open {
	struct File *o_file; // 指向文件控制块的指针
	u_int o_fileid;	     // 文件编号
	int o_mode;	     // 文件打开方式
	struct Filefd *o_ff; // 指向文件描述符的指针
};
```

### Thinking 5.7

内核初始化两个进程，一个是用户进程，一个是文件服务进程。

在用户进程中，所有对文件的基本操作都是通过与文件服务进程IPC通信进行的。文件服务进程有能力对设备进行底层操作，维护文件系统，返回文件描述符、读取文件并映射到内存，从IPC返回通信结果或共享内存页面至用户进程。

## 难点分析

实验难点一在于设备驱动，图解见指导书图5.2，表格见指导书表5.1、5.2。

二在于文件系统的设计结构，图解见指导书图5.3、5.4。

三是文件系统服务进程的核心逻辑部分，也是我个人认为这一lab**最难**的部分，尤其是对文件系统的维护与各类文件控制结构体、文件描述符的维护，还有对应的内存-磁盘缓存/映射操作，图解见指导书图5.5、5.6。

四是用户文件接口及其与文件服务进程的IPC通信逻辑，到此可以推出MOS的文件逻辑设计的全貌。图解指导书图5.1、5.7。

## 实验体会

Lab5的实验带我了解了MOS系统的文件系统设计与运行逻辑。私以为这一部分的**设计复杂度极高**，需要细细地、慢慢地啃设计与源码。

