#include<iostm8l151k4.h>
#include <stdint.h>
#include <stdio.h>
#include"usart1.h"
#include"i2c.h"

/*******************************************************************************
**函数名称：void delay(unsigned int ms)     Name: void delay(unsigned int ms)
**功能描述：大概延时
**入口参数：unsigned int ms   输入大概延时数值
**输出：无
*******************************************************************************/
void Delayms(unsigned int ms)
{
  unsigned int x , y;
  for(x = ms; x > 0; x--)           /*  通过一定周期循环进行延时*/
    for(y = 3000 ; y > 0 ; y--);
}
/*******************************************************************************
**函数名称:void IIC_Init() 
**功能描述:初始化IIC1接口
**入口参数:
**输出:无
*******************************************************************************/
void IIC_Init()
{	
    CLK_PCKENR1_bit.PCKEN13 = 1;    //使能I2C1时钟		 
   
    I2C1_CR2_bit.SWRST = 1;             //I2C1->CR1 |= 1 << 15; 		//复位I2C1寄存器
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    I2C1_CR2_bit.SWRST = 0;            	//清除复位设置
    
    //I2C1->CR2 &= ~I2C_CR2_FREQ;												//
    I2C1_FREQR_bit.FREQ = 16;      	//Freq = 36M

    I2C1_CCRH_bit.F_S = 1;   		//用于快速模式
    I2C1_CCRH_bit.DUTY = 0;
    I2C1_CCRL = 40;           		//设置I2C1的时钟频率为200KHZ  I2CFreq = PCLK1 / CCR / DUTY[位]		=   16MHZ / 40 / 2 = 200KHZ
    
    I2C1_TRISER_bit.TRISE = 16 + 1;								
    
   // I2C1->CR1 &= ~I2C_CR1_SMBUS; 	//设置为I2C模式
   // I2C1_CR1_bit.ENGC = 1;		//设置为广播呼叫模式
    I2C1_CR1_bit.PE = 1;		        //打开I2C(使能I2C)
	
}


/*******************************************************************************
**函数名称:void IIC_Write(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
**功能描述:向IIC器件写数据
**入口参数:
          subaddr :  从器件地址
          Byte_addr : 确定器件写地址的起始地址
          *buffer   : 写数据的起址地址
          num				: 要写数据的个数
**输出:无
*******************************************************************************/
void IIC_Write(unsigned char subaddr , unsigned char Opcode ,unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
{
	unsigned char i2csr1;
	
	
	//while(I2C1->SR2 & I2C_SR2_BUSY);          //判断I2C模块是否忙
	
	//发送起始信号
	I2C1_CR2_bit.START = 1;    
	while(I2C1_SR1_bit.SB == 0);	//等待起始信号产生
	i2csr1 = I2C1_SR1; //SR1.AF
	I2C1_DR = (subaddr << 1);		//发送器件地地址，并清除SB标志位
	while(I2C1_SR1_bit.ADDR == 0);	//等待器件地址发送完成
	i2csr1 = I2C1_SR1;
	i2csr1 = I2C1_SR3;		//读状态寄存器1和状态寄存器3清除发送器件地址标志位
	

	I2C1_DR = Opcode;             //发送器件存储首地址

	while(I2C1_SR1_bit.BTF == 0);  //等待移位发送器发送完成
	i2csr1 = I2C1_SR1;	      //清除BIT标志位
        
        I2C1_DR = Byte_addr;             //发送器件存储首地址

	while(I2C1_SR1_bit.BTF == 0);  //等待移位发送器发送完成
	i2csr1 = I2C1_SR1;	      //清除BIT标志位

	while(num > 0)
	{
            I2C1_DR = *buffer;		//发送器件存储首地址

            while(I2C1_SR1_bit.BTF == 0);//等待移位发送器发送完成
            i2csr1 = I2C1_SR1;		//清除BIT标志位
            buffer++;
            num--;
	}
	I2C1_CR2_bit.STOP = 1;   	//发送停止信号结束数据传输
}

void IIC_Write_Pcap04(unsigned char subaddr , unsigned char Opcode)
{
	unsigned char i2csr1;
	
	
	//while(I2C1->SR2 & I2C_SR2_BUSY);          //判断I2C模块是否忙
	
	//发送起始信号
	I2C1_CR2_bit.START = 1;    
	while(I2C1_SR1_bit.SB == 0);	//等待起始信号产生
	i2csr1 = I2C1_SR1; //SR1.AF
	I2C1_DR = (subaddr << 1);		//发送器件地地址，并清除SB标志位
	while(I2C1_SR1_bit.ADDR == 0);	//等待器件地址发送完成
	i2csr1 = I2C1_SR1;
	i2csr1 = I2C1_SR3;		//读状态寄存器1和状态寄存器3清除发送器件地址标志位

        I2C1_DR = Opcode;		//发送器件存储首地址

        while(I2C1_SR1_bit.BTF == 0);//等待移位发送器发送完成
        i2csr1 = I2C1_SR1;		//清除BIT标志位
  

	I2C1_CR2_bit.STOP = 1;   	//发送停止信号结束数据传输
}




/*******************************************************************************
**函数名称:void IIC_Read(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
**功能描述:向IIC器件读数据
**入口参数:
          subaddr :  从器件地址
          Byte_addr : 确定器件写地址的起始地址
          *buffer   : 读数据的缓冲区起始地址
          num				: 要读数据的个数
**输出:无
*******************************************************************************/
void IIC_Read(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
{
  unsigned char i2csr1;
  
  
 
  
  I2C1_CR2_bit.ACK = 1;	        //产生应答信号
  
  I2C1_CR2_bit.START = 1;	//发送起始信号
  while(I2C1_SR1_bit.SB == 0);	//等待起始信号产生
  i2csr1 = I2C1_SR1;		//SR1.AF??
  I2C1_DR = (subaddr << 1);		//发送器件地地址，并清除SB标志位
  while(I2C1_SR1_bit.ADDR == 0);	//等待器件地址发送完成
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;		//读状态寄存器1和状态寄存器3清除发送器件地址标志位

  I2C1_DR = Byte_addr;
  while(I2C1_SR1_bit.BTF == 0);//等待移位发送器发送完成
  i2csr1 = I2C1_SR1;	//清除BIT标志位
  
  
  //重新发送起始信号
  I2C1_CR2_bit.START = 1;//I2C1->CR1 |= I2C_CR1_START;
  while(I2C1_SR1_bit.SB == 0);//等待起始信号产生

  i2csr1 = I2C1_SR1;//SR1.AF??
  I2C1_DR = (char)((subaddr << 1)| 0x01);	//发送器件地地址，并清除SB标志位
  while(I2C1_SR1_bit.ADDR == 0);         //等待器件地址发送完成
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;			//读状态寄存器1和状态寄存器2清除发送器件地址标志位

  while(num)
  {
    if(num == 1)
    {
        I2C1_CR2_bit.ACK = 0;          //最后一个字节不产生应答信号
        I2C1_CR2_bit.STOP = 1;         //发送停止信号结束数据传输
    }

    while(I2C1_SR1_bit.RXNE == 0);
    i2csr1 = I2C1_SR1;

    *buffer = I2C1_DR;	
    buffer++;
    num--;
  }
}

unsigned char IIC_Read_Pcap04(unsigned char subaddr , unsigned char Opcode)
{
  unsigned char buffer;
  unsigned char i2csr1;
  
  I2C1_CR2_bit.ACK = 1;	        //产生应答信号
  
  I2C1_CR2_bit.START = 1;	//发送起始信号
  while(I2C1_SR1_bit.SB == 0);	//等待起始信号产生
  i2csr1 = I2C1_SR1;		//SR1.AF??
  I2C1_DR = (subaddr << 1) ;		//发送器件地地址，并清除SB标志位
  
  while(I2C1_SR1_bit.ADDR == 0);	//等待器件地址发送完成
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;		//读状态寄存器1和状态寄存器3清除发送器件地址标志位

  I2C1_DR = Opcode;
  while(I2C1_SR1_bit.BTF == 0);//等待移位发送器发送完成
  i2csr1 = I2C1_SR1;	//清除BIT标志位  
  
  I2C1_CR2_bit.ACK = 1;	        //产生应答信号
  
  //重新发送起始信号
  I2C1_CR2_bit.START = 1;//I2C1->CR1 |= I2C_CR1_START;
  while(I2C1_SR1_bit.SB == 0);//等待起始信号产生
  
  i2csr1 = I2C1_SR1;//SR1.AF??
  //I2C1_DR = ((subaddr << 1) | 0x01);	//发送器件地地址，并清除SB标志位
  I2C1_DR = (char)((subaddr << 1)| 0x01);	//发送器件地地址，并清除SB标志位
  while(I2C1_SR1_bit.ADDR == 0);         //等待器件地址发送完成
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;			//读状态寄存器1和状态寄存器2清除发送器件地址标志位
  
  while(I2C1_SR1_bit.RXNE == 0);
  buffer = I2C1_DR;
  
//  i2csr1 = I2C1_SR1;
    
  I2C1_CR2_bit.ACK = 0;          //最后一个字节不产生应答信号
  I2C1_CR2_bit.STOP = 1;         //发送停止信号结束数据传输
  
  return buffer;

}


