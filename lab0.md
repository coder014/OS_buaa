# Lab0 实验报告

## 思考题

### Thinking 0.1

`Untracked`状态下，README文件位于工作目录且未被跟踪；`Stage`状态下，README文件位于暂存区。

与第一次执行 `git status`效果不一样，因为README文件已用 `git add`命令跟踪，工作区文件与暂存区不一致，处于修改状态。

### Thinking 0.2

`git add`、`git add`和 `git commit`。

### Thinking 0.3

1. `git restore print.c`
2. `git restore --staged print.c; git restore print.c`
3. `git rm --cached hello.txt`

### Thinking 0.4

第一次reset后提交记录只有1和2，第二次reset后提交记录只有1，第三次reset后提交记录恢复到3次。

### Thinking 0.5

```shell
echo first
```

终端输出 `first`。

```shell
echo second > output.txt
```

终端无输出，`output.txt`文件内容为 `second`。

```shell
echo third > output.txt
```

终端无输出，`output.txt`文件内容为 `third`。

```shell
echo fourth > output.txt
```

终端无输出，`output.txt`文件内容为 `third`\n(换行)`fourth`。

### Thinking 0.6

`command`文件内容：

```
echo echo Shell Start... > test
echo echo set a = 1 >> test
echo a=1 >> test
echo echo set b = 2 >> test
echo b=2 >> test
echo echo set c = a+b >> test
echo 'c=$[$a+$b]' >> test
echo 'echo c = $c' >> test
echo echo save c to ./file1 >> test
echo 'echo $c>file1' >> test
echo echo save b to ./file2 >> test
echo 'echo $b>file2' >> test
echo echo save a to ./file3 >> test
echo 'echo $a>file3' >> test
echo echo save file1 file2 file3 to file4 >> test
echo 'cat file1>file4' >> test
echo 'cat file2>>file4' >> test
echo 'cat file3>>file4' >> test
echo echo save file4 to ./result >> test
echo 'cat file4>>result' >> test
```

`result`文件内容：

```
3
2
1
```

先由 `a=1` `b=2`给变量赋值，由 `c=$[$a+$b]`为变量 `c`赋值为3。依次用echo将他们输出并重定向到file_文件，并用 `cat`与重定向将3个文件合并为 `file4`文件，最后写入 `result`文件。

`echo echo Shell Start`中 `echo Shell Start`作为要输出的内容被 `echo`输出，结果为 `echo Shell Start`；<code>echo &#96;echo Shell Start&#96;</code>中 `echo Shell Start`作为命令被执行，返回结果 `Shell Start`被第一个echo输出，结果为 `Shell Start`。

## 难点分析

主要为git的流程与几个区的辨析，具体可见指导书 `图0.10`；以及Bash Shell的编写。

## 实验体会

学习了git与Bash Shell，为后续操作系统课程学习做铺垫：可利用shell编写自动化脚本方便编译，利用git做代码版本管理等。
