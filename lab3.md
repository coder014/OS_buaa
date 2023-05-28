# Lab3 实验报告

## 思考题

### Thinking 3.1

这段代码的作用是将进程页目录自身地址加入页目录项，创建了自映射页目录结构，使得用户进程可以在`UVPT`读取进程自身页表。

### Thinking 3.2

`map_page()`中的`data`来源于`load_icode()`函数的调用，其中`data`参数实际上是进程控制块`e`的指针，用于`map_page()`函数将对应数据加载到对应结构中。没有`data`参数不行，因为这样回调函数就无法将数据加载到对应的结构中。

### Thinking 3.3

由于加载到内存的程序段的虚拟地址可能不是页对齐的，而内核需要以页为单位加载，因此需要为`bin`考虑：

* 可能不对齐的前段
* 页对齐的中间段
* 可能需要新的一页但未全部使用的后段

至此代码、静态常量部分已加载完毕，但可能还存在存储变量等的程序内存区，而整个程序段需要保证虚拟内存上的连续，即此部分需要与前部`bin`部分连续，因此需要为`mem`考虑：

* 需要与`bin`的后段连接的前段（可能在同一页）
* 页对齐的中间段
* 可能需要新的一页但未全部使用的后段

### Thinking 3.4

是虚拟地址。

### Thinking 3.5

`handle_int`函数位于`kern/genex.S`，`handle_mod`与`handle_sys`尚未在Lab3中实现，`handle_tlb`事实上由`kern/genex.S`中的`BUILD_HANDLER tlb do_tlb_refill`宏语句重定向到`kern/tlb_asm.S`中的`do_tlb_refill`函数。

### Thinking 3.6

```assembly
LEAF(enable_irq)
	li      t0, (STATUS_CU0 | STATUS_IM4 | STATUS_IEc) # 打开全局中断开关、启用4号中断、打开“co-processor 0 usable”开关（使用户态能使用部分CPO相关的特权指令），准备写入CP0寄存器
	mtc0    t0, CP0_STATUS # 更新CP0_Status寄存器的值
	jr      ra # 函数返回
END(enable_irq)
```

```assembly
NESTED(handle_int, TF_SIZE, zero)
	# 分发中断到处理函数
timer_irq: # 时钟中断处理
	sw      zero, (KSEG1 | DEV_RTC_ADDRESS | DEV_RTC_INTERRUPT_ACK) # 向时钟设备写值，响应时钟中断
	li      a0, 0 # 传入参数yield=0
	j       schedule # 调用schedule函数(不返回)
END(handle_int)
```

### Thinking 3.7

操作系统建立了一个`RUNNABLE`进程的循环队列，并且对每一个进程进行时间片计数，每次时钟中断发生时将当前进程的计数减1。一旦当前进程的时间片用完，则再将这个进程移动到就绪队列的尾端并重置其时间片计数，取出队列头部的进程继续执行并计数。用此种机制便实现了根据时钟中断切换进程。

## 难点分析

实验难点一在于二进制可执行文件的加载，分清加载时页不对齐带来的各种特殊情况，图解见指导书图3.2。

二在于中断的分发、处理机制，尤其是几处汇编代码的理解、CP0各寄存器的各个位的作用，图解见指导书图3.1、图3.3、图3.4。

三是基于时间片的调度机制，要理解使用循环链表来调度的原理，辨析调度时的一些特殊情况。此外还要理解从内核态到用户态的关键跳转原理。

## 实验体会

Lab3的实验带我了解了MOS系统的进程调度、异常中断与可执行文件加载的过程与原理。尤其是可执行文件加载的部分，使我明白了操作系统的设计需要严谨的逻辑、严格遵守规范。

