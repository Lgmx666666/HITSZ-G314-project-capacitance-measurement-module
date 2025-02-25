# 使用说明

## 版本对应
代码工程文件夹的命名与Hardware的版本号相对应。

## 开发环境
1.本人使用的开发环境为STM32CUBEMX+Clion，相应的搭建教程可以参考[博客：点击此处](https://blog.csdn.net/ctrlthh/article/details/131471750)
2.另外还可以选择使用STM32CUBEMX+Keil来开发，只需对给出的工程文件进行移植

## PCAP04的驱动问题
该芯片寄存器较多，使用难度大，在实际编写pcap04驱动代码的过程中参考了[开源代码：点击此处](https://github.com/tomsalden/PCAP04),可以结合工程里面的代码来理解并使用