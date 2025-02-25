#include<iostm8l151k4.h>
#include <stdint.h>
#include <stdio.h>
#include"usart1.h"
#include"pcap04.h"
#include"i2c.h"

unsigned char flag;

/*******************************************************************************
**函数名称：void TIM2_DelayMs(unsigned int ms)
**功能描述：定时器2参进行精确延时，最小为1微秒，最大65535
**入口参数：unsigned int ms     1=< us <= 65535 
**输出：无
*******************************************************************************/
void DelaynUs(unsigned int ms)
{ 
    
    TIM2_CR1 = 0x81;        //启动定时器2开始计数
    while(ms--)
    {
      while( !(TIM2_SR1 & 0x01)); //等待计数是否达到1微秒
      TIM2_SR1 &= ~(0x01);  //计数完成1毫秒，清除相应的标志
    }
    TIM2_CR1 = 0x00;       //延时全部结束，关闭定时器2
}

void INTN_Init()
{
  PD_DDR_bit.DDR3 = 0;      //GPD->PIN3 设置端口PD->3的输入输出方向寄存器为输入方向
  PD_CR1_bit.C13 = 1;       //GPD->PIN3 带上拉电阻输入
 // PD_CR2_bit.C23 = 1;       //GPD->PIN3  使能外部中断
  PD_CR2_bit.C23 = 0;       //GPD->PIN3  使能外部中断 

  
//  EXTI_CR3_bit.PDIS = 2;    //PortD端口 外部中断线2为下降沿触发中断
//  EXTI_CONF_bit.PDLIS = 1;  //设置PD作为外部通用中断
}


/*******************************************************************************
**函数名称：void CLKInit()
**功能描述：配置系统时钟及蜂鸣器低频时钟
**入口参数：无
**输出：无
*******************************************************************************/
void CLKInit()
{
  CLK_CKDIVR = 0x00;                  //内部高速时钟16MHZ 进行1分频
  
  CLK_ICKCR_bit.LSION = 1;             // 打开芯片内部的低速振荡器LSI
  
  while(CLK_ICKCR_bit.LSIRDY == 0);    // 等待振荡器稳定
 
  CLK_CBEEPR_bit.CLKBEEPSEL0 = 1;     //选择内部的低频/低速时钟作为蜂鸣器的时钟
  CLK_CBEEPR_bit.CLKBEEPSEL1 = 0;
}

int main(void)
{

  
  flag = 0;
  asm("sim");
  CLKInit();
  UART1_Init(9600);               //调用串口2初始化函数，并设置波特率为9600 bps
  IIC_Init();
  INTN_Init();
  asm("rim");
  
  printf("\r\n Pcap04 test!!!");
  PCap04_PowerON_RESET();  
  PCap04_Init();
  
  PCap04_Test();
  
  PCap04_writeFirmware();
  PCap04_writeConfig();
  
  
  
  PCap04_CDCStart();
  
  
  while(1)
  {
     

    if (!PD_ODR_bit.ODR3)
    {
      PCap04_ReadResult();
    }
   
  }
}


