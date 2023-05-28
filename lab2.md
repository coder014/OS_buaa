# Lab2 实验报告

## 思考题

### Thinking 2.1

指针变量存储的地址是虚拟地址，`lw`和`sw`使用的也是虚拟地址。

### Thinking 2.2

链表宏可以为C语言补充实现类似_泛型_的功能，使其可以绑定到各种数据类型上；而使用传统函数形式的链表则需要为每种类型创建一套方法，可重用性差。

Linux系统的链表实现中：单向链表只能向后插入，无法向前插入，但插入操作时无需维护prev指针因此较双向链表快；而删除操作时则需要遍历链表来获取前驱节点，因此较双向链表慢。循环链表事实上也是单向的，支持向后插入与向末尾插入，无需维护prev指针因此较双向链表快；删除操作时同样需要遍历链表来获取前驱节点，较双向链表慢。

### Thinking 2.3

正确的展开结构为 **C** 。

### Thinking 2.4

操作系统中每个进程都有自己的地址空间，将不同的ASID分配给不同进程用以区分地址空间；没有ASID，则不同进程存取地址发生碰撞时就会产生错误。

ASID有 $6$ 位，则可容纳不同地址空间的最大数量为 $2^6=64$ 。

### Thinking 2.5

`tlb_invalidate`函数调用`tlb_out`函数。

`tlb_invalidate`函数使TLB中具有指定VPN和ASID的表项失效。

```asm
LEAF(tlb_out) # 定义tlb_out函数（叶函数）
.set noreorder
	mfc0    t0, CP0_ENTRYHI # 暂存当前EntryHi寄存器的值
	mtc0    a0, CP0_ENTRYHI # 将传给tlb_out的参数写入EntryHi
	nop
	tlbp # 根据EntryHi的VPN与ASID查找表项
	nop
	mfc0    t1, CP0_INDEX # 取出Index的值
.set reorder
	bltz    t1, NO_SUCH_ENTRY # 是否找到表项
.set noreorder
	mtc0    zero, CP0_ENTRYHI # 写入0，清空表项
	mtc0    zero, CP0_ENTRYLO0 # 写入0，清空表项
	nop
	tlbwi # 写入对应表项
.set reorder
NO_SUCH_ENTRY:
	mtc0    t0, CP0_ENTRYHI # 恢复EntryHi寄存器的值
	j       ra # 函数返回
END(tlb_out) # 标记函数末尾
```

### Thinking 2.6 (x86)

* X86体系结构中的内存管理机制使用了分段式和分页式相结合的方法来管理内存。其中分段式将逻辑地址分为段号和偏移量两部分，段号指示了该地址所在的段，而偏移量则指示了该地址在该段内的偏移量。每个段可以有不同的大小和权限。而分页式则将内存划分为固定大小的页，这些页可以被映射到物理内存中的任何位置。
* MIPS体系结构中的内存管理机制则使用了简单的分段式方法（若不使用多级页表），将逻辑地址分为高16位和低16位，其中高16位被解释为段号，而低16位则是段内偏移量。

### Thinking A.1

* 三级页表页目录的基地址 $PD_{base}$ 为 $PT_{base} + (PT_{base} >> 12) * 8 = PT_{base} + (PT_{base} >> 9)$
* 映射到页目录自身的页目录项为

$$\begin{align}PD_{base}+((PD_{base}-PT_{base})>>12)*8 \\&= PD_{base}+(PT_{base}>>18)\\&=PT_{base}+(PT_{base}>>9)+(PT_{base}>>18)\end{align}$$

## 难点分析

实验难点一在于物理内存管理部分中**链表宏**的理解，图解见指导书图2.2。二在于虚拟内存管理部分中两级页表结构的理解，图解见指导书图2.3；以及几个页表相关函数的理解，尤其是`pgdir_walk`函数，图解见指导书图2.4。

## 实验体会

Lab2的实验让我初识了MOS系统的内存管理机制，其中有物理内存管理、虚拟内存管理和TLB维护三大模块，带我一步步从未初始化的内存空间走向完成可灵活分配的内存空间之旅。
