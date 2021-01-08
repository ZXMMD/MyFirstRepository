## 实验一 ：shellcode

### 实验要求

- [x] 把把[这个链接](https://www.exploit-db.com/shellcodes/48116)中的shellcode能够成功运行
- [x] 能从汇编源码编译通过并成功dump
- [x] 自行查阅资料，搜索Windows PEB结构体，PE文件导入表导出表相关数据结构的文档，解读shellcode原理
- [x] 修改shellcode功能为运行记事本程序notepad.exe

### 实验过程

#### 成功运行弹出计算器的shellcode

- 将扩展名由.cpp改为.c

- 关闭数据执行保护编译选项

![](images/property.PNG)

- 运行结果(动图展示)

![](images/calc.gif)

#### 编译通过并dump

![](images/dumpbin.PNG)

#### shellcode原理
```bash
理解可能有误，请老师指正。

想要使用shellcode，我们需要动态获取所需要的系统API地址。
这是因为shellcode的很多操作涉及系统底层，必然会调用windows的API。
正常情况下，windows针对3环用户态程序提供LoadLibrary加载dll，返回dll的基址。
然后通过GetProcAddress从dll种获取函数基址；但这两个函数本身也是windwos 的API，其地址依然需要动态获取。
这时候就用到PEB结构体了。
每个进程都有PEB结构体，记录了进程的各种信息。
比如PEB结构体中的PEB_LDR_DATA结构体，记录了顺序加载的模块链表。
首先找到链表的头指针，遍历链表，查找kernerl32.dll，
kernerl32.dll 基址找到后再根据PE的导出表能找到GetProcAddr的地址，
有了GetProcAddr，又能继续查找LoadLibrary的地址。
有了LoadLibrary和GetProcAddr两大函数地址，任何dll的任何函数入口便都能找到了。
在缓冲区溢出漏洞中，我们把返回地址改成我们自己的函数地址，就可以执行我们自己的程序了。
```

#### 修改shellcode弹出记事本

- 核心是将calc.exe修改为notepad.exe,参照[示例](https://www.exploit-db.com/shellcodes/48116),具体修改如下图所示：

![](images/compare.PNG)

- 将修改后的文件传到VirtualBox中的Kali系统中，执行如下命令,得到十六进制代码
```bash
nasm -f win32 win32-WinExec_Calc-Exit.asm -o win32-WinExec_Calc-Exit.o

for i in $(objdump -D win32-WinExec_Calc-Exit.o | grep "^ " | cut -f2); do echo -n '\x'$i; done; echo
```
![](images/Code.PNG)

- 运行结果(动图展示)

![](images/notepad.gif)