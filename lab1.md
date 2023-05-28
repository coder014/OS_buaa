# Lab1 实验报告

## 思考题

### Thinking 1.1

```
$ gcc -g -c hello.c
$ objdump -DS hello.o

hello.o:     file format elf64-x86-64
......
int main()
{
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
    printf("Hello World!\n");
   4:   48 8d 3d 00 00 00 00    lea    0x0(%rip),%rdi        # b <main+0xb>
   b:   e8 00 00 00 00          callq  10 <main+0x10>
......

$ gcc -g -o hello hello.c
$ objdump -DS hello

hello:     file format elf64-x86-64
......
int main()
{
    1135:       55                      push   %rbp
    1136:       48 89 e5                mov    %rsp,%rbp
    printf("Hello World!\n");
    1139:       48 8d 3d c4 0e 00 00    lea    0xec4(%rip),%rdi        # 2004 <_IO_stdin_used+0x4>
    1140:       e8 eb fe ff ff          callq  1030 <puts@plt>
......
```

```
$ mips-linux-gnu-gcc -g -c hello.c
$ mips-linux-gnu-objdump -S --disassemble=main hello.o

hello.o:     file format elf32-tradbigmips

   0:   3c1c0000        lui     gp,0x0
......
    printf("Hello World!\n");
  20:   8f820000        lw      v0,0(gp)
  24:   24440000        addiu   a0,v0,0
  28:   8f820000        lw      v0,0(gp)
  2c:   0040c825        move    t9,v0
  30:   0320f809        jalr    t9
......

$ mips-linux-gnu-gcc -g -o hello hello.c
$ mips-linux-gnu-objdump -S --disassemble=main hello

hello:     file format elf32-tradbigmips

 7c0:   3c1c0002        lui     gp,0x2
......
    printf("Hello World!\n");
 7e0:   8f828034        lw      v0,-32716(gp)
 7e4:   244409c0        addiu   a0,v0,2496
 7e8:   8f82805c        lw      v0,-32676(gp)
 7ec:   0040c825        move    t9,v0
 7f0:   0320f809        jalr    t9
......
```

由`objdump`工具的help输出与`man`手册可得：

>-D, --disassemble-all    Display assembler contents of all sections
>
>-S, --source             Intermix source code with disassembly

`-D`参数开关是反汇编所有程序节，`-S`参数开关是将源代码与反汇编混合输出（若程序存在调试信息）

### Thinking 1.2

从代码中可知，我们的`readelf`工具只支持`ELF32`这一种ELF文件格式，而在我们的编译环境（x86_64架构）中使用原生编译工具链得到的默认ELF可执行文件格式为`ELF64`；我们的`readelf`工具正是通过默认参数构建的，而`hello`程序则通过编译参数`-m32`指定了目标架构，构建出了`ELF32`格式的可执行文件，因此可以解析`hello`程序但无法解析自身。

也可以由系统`readelf`工具进行验证：

```
$ readelf -h hello
ELF Header:
  Magic:   .. .. .. ..
  Class:                             ELF32

$ readelf -h readelf
ELF Header:
  Magic:   .. .. .. ..
  Class:                             ELF64
```

### Thinking 1.3

实际上启动地址处存放的不是内核而是Bootloader。由于体系结构的初始启动环境受限，因此需要Bootloader和内核配合来进行“多级火箭”式的多段式启动。Bootloader启动后会初始化相应的硬件，然后寻找存储在文件系统中的内核文件并按内存布局载入RAM，最后跳转到内核入口处，完成启动的交接。

而我们实验课的实操环境是由`gxemul`模拟器模拟的`testmips`架构，`gxemul`模拟器本身就具有Bootloader的功能：准备硬件环境、读取ELF文件并加载到内存指定位置并设置入口点，因此可以保证入口被跳转到。

## 难点分析

主要难点在于操作系统内核从系统加电开始的引导过程，示意图可参考指导书图A.1；以及程序编译与链接的过程，示意图可参考指导书图A.2。

## 实验体会

Lab1的实验让我深入理解了操作系统的引导过程，初步了解了程序编译与链接的原理，简要学习了`ELF32`可执行文件的格式并尝试编写一个可以解析该格式的程序，简单体会并自己实现了一个简单的类`printf()`格式化输出函数。
