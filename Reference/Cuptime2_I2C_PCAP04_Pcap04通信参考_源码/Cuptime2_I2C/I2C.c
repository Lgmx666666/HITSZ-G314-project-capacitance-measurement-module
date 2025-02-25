#include<iostm8l151k4.h>
#include <stdint.h>
#include <stdio.h>
#include"usart1.h"
#include"i2c.h"

/*******************************************************************************
**�������ƣ�void delay(unsigned int ms)     Name: void delay(unsigned int ms)
**���������������ʱ
**��ڲ�����unsigned int ms   ��������ʱ��ֵ
**�������
*******************************************************************************/
void Delayms(unsigned int ms)
{
  unsigned int x , y;
  for(x = ms; x > 0; x--)           /*  ͨ��һ������ѭ��������ʱ*/
    for(y = 3000 ; y > 0 ; y--);
}
/*******************************************************************************
**��������:void IIC_Init() 
**��������:��ʼ��IIC1�ӿ�
**��ڲ���:
**���:��
*******************************************************************************/
void IIC_Init()
{	
    CLK_PCKENR1_bit.PCKEN13 = 1;    //ʹ��I2C1ʱ��		 
   
    I2C1_CR2_bit.SWRST = 1;             //I2C1->CR1 |= 1 << 15; 		//��λI2C1�Ĵ���
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    I2C1_CR2_bit.SWRST = 0;            	//�����λ����
    
    //I2C1->CR2 &= ~I2C_CR2_FREQ;												//
    I2C1_FREQR_bit.FREQ = 16;      	//Freq = 36M

    I2C1_CCRH_bit.F_S = 1;   		//���ڿ���ģʽ
    I2C1_CCRH_bit.DUTY = 0;
    I2C1_CCRL = 40;           		//����I2C1��ʱ��Ƶ��Ϊ200KHZ  I2CFreq = PCLK1 / CCR / DUTY[λ]		=   16MHZ / 40 / 2 = 200KHZ
    
    I2C1_TRISER_bit.TRISE = 16 + 1;								
    
   // I2C1->CR1 &= ~I2C_CR1_SMBUS; 	//����ΪI2Cģʽ
   // I2C1_CR1_bit.ENGC = 1;		//����Ϊ�㲥����ģʽ
    I2C1_CR1_bit.PE = 1;		        //��I2C(ʹ��I2C)
	
}


/*******************************************************************************
**��������:void IIC_Write(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
**��������:��IIC����д����
**��ڲ���:
          subaddr :  ��������ַ
          Byte_addr : ȷ������д��ַ����ʼ��ַ
          *buffer   : д���ݵ���ַ��ַ
          num				: Ҫд���ݵĸ���
**���:��
*******************************************************************************/
void IIC_Write(unsigned char subaddr , unsigned char Opcode ,unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
{
	unsigned char i2csr1;
	
	
	//while(I2C1->SR2 & I2C_SR2_BUSY);          //�ж�I2Cģ���Ƿ�æ
	
	//������ʼ�ź�
	I2C1_CR2_bit.START = 1;    
	while(I2C1_SR1_bit.SB == 0);	//�ȴ���ʼ�źŲ���
	i2csr1 = I2C1_SR1; //SR1.AF
	I2C1_DR = (subaddr << 1);		//���������ص�ַ�������SB��־λ
	while(I2C1_SR1_bit.ADDR == 0);	//�ȴ�������ַ�������
	i2csr1 = I2C1_SR1;
	i2csr1 = I2C1_SR3;		//��״̬�Ĵ���1��״̬�Ĵ���3�������������ַ��־λ
	

	I2C1_DR = Opcode;             //���������洢�׵�ַ

	while(I2C1_SR1_bit.BTF == 0);  //�ȴ���λ�������������
	i2csr1 = I2C1_SR1;	      //���BIT��־λ
        
        I2C1_DR = Byte_addr;             //���������洢�׵�ַ

	while(I2C1_SR1_bit.BTF == 0);  //�ȴ���λ�������������
	i2csr1 = I2C1_SR1;	      //���BIT��־λ

	while(num > 0)
	{
            I2C1_DR = *buffer;		//���������洢�׵�ַ

            while(I2C1_SR1_bit.BTF == 0);//�ȴ���λ�������������
            i2csr1 = I2C1_SR1;		//���BIT��־λ
            buffer++;
            num--;
	}
	I2C1_CR2_bit.STOP = 1;   	//����ֹͣ�źŽ������ݴ���
}

void IIC_Write_Pcap04(unsigned char subaddr , unsigned char Opcode)
{
	unsigned char i2csr1;
	
	
	//while(I2C1->SR2 & I2C_SR2_BUSY);          //�ж�I2Cģ���Ƿ�æ
	
	//������ʼ�ź�
	I2C1_CR2_bit.START = 1;    
	while(I2C1_SR1_bit.SB == 0);	//�ȴ���ʼ�źŲ���
	i2csr1 = I2C1_SR1; //SR1.AF
	I2C1_DR = (subaddr << 1);		//���������ص�ַ�������SB��־λ
	while(I2C1_SR1_bit.ADDR == 0);	//�ȴ�������ַ�������
	i2csr1 = I2C1_SR1;
	i2csr1 = I2C1_SR3;		//��״̬�Ĵ���1��״̬�Ĵ���3�������������ַ��־λ

        I2C1_DR = Opcode;		//���������洢�׵�ַ

        while(I2C1_SR1_bit.BTF == 0);//�ȴ���λ�������������
        i2csr1 = I2C1_SR1;		//���BIT��־λ
  

	I2C1_CR2_bit.STOP = 1;   	//����ֹͣ�źŽ������ݴ���
}




/*******************************************************************************
**��������:void IIC_Read(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
**��������:��IIC����������
**��ڲ���:
          subaddr :  ��������ַ
          Byte_addr : ȷ������д��ַ����ʼ��ַ
          *buffer   : �����ݵĻ�������ʼ��ַ
          num				: Ҫ�����ݵĸ���
**���:��
*******************************************************************************/
void IIC_Read(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num)
{
  unsigned char i2csr1;
  
  
 
  
  I2C1_CR2_bit.ACK = 1;	        //����Ӧ���ź�
  
  I2C1_CR2_bit.START = 1;	//������ʼ�ź�
  while(I2C1_SR1_bit.SB == 0);	//�ȴ���ʼ�źŲ���
  i2csr1 = I2C1_SR1;		//SR1.AF??
  I2C1_DR = (subaddr << 1);		//���������ص�ַ�������SB��־λ
  while(I2C1_SR1_bit.ADDR == 0);	//�ȴ�������ַ�������
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;		//��״̬�Ĵ���1��״̬�Ĵ���3�������������ַ��־λ

  I2C1_DR = Byte_addr;
  while(I2C1_SR1_bit.BTF == 0);//�ȴ���λ�������������
  i2csr1 = I2C1_SR1;	//���BIT��־λ
  
  
  //���·�����ʼ�ź�
  I2C1_CR2_bit.START = 1;//I2C1->CR1 |= I2C_CR1_START;
  while(I2C1_SR1_bit.SB == 0);//�ȴ���ʼ�źŲ���

  i2csr1 = I2C1_SR1;//SR1.AF??
  I2C1_DR = (char)((subaddr << 1)| 0x01);	//���������ص�ַ�������SB��־λ
  while(I2C1_SR1_bit.ADDR == 0);         //�ȴ�������ַ�������
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;			//��״̬�Ĵ���1��״̬�Ĵ���2�������������ַ��־λ

  while(num)
  {
    if(num == 1)
    {
        I2C1_CR2_bit.ACK = 0;          //���һ���ֽڲ�����Ӧ���ź�
        I2C1_CR2_bit.STOP = 1;         //����ֹͣ�źŽ������ݴ���
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
  
  I2C1_CR2_bit.ACK = 1;	        //����Ӧ���ź�
  
  I2C1_CR2_bit.START = 1;	//������ʼ�ź�
  while(I2C1_SR1_bit.SB == 0);	//�ȴ���ʼ�źŲ���
  i2csr1 = I2C1_SR1;		//SR1.AF??
  I2C1_DR = (subaddr << 1) ;		//���������ص�ַ�������SB��־λ
  
  while(I2C1_SR1_bit.ADDR == 0);	//�ȴ�������ַ�������
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;		//��״̬�Ĵ���1��״̬�Ĵ���3�������������ַ��־λ

  I2C1_DR = Opcode;
  while(I2C1_SR1_bit.BTF == 0);//�ȴ���λ�������������
  i2csr1 = I2C1_SR1;	//���BIT��־λ  
  
  I2C1_CR2_bit.ACK = 1;	        //����Ӧ���ź�
  
  //���·�����ʼ�ź�
  I2C1_CR2_bit.START = 1;//I2C1->CR1 |= I2C_CR1_START;
  while(I2C1_SR1_bit.SB == 0);//�ȴ���ʼ�źŲ���
  
  i2csr1 = I2C1_SR1;//SR1.AF??
  //I2C1_DR = ((subaddr << 1) | 0x01);	//���������ص�ַ�������SB��־λ
  I2C1_DR = (char)((subaddr << 1)| 0x01);	//���������ص�ַ�������SB��־λ
  while(I2C1_SR1_bit.ADDR == 0);         //�ȴ�������ַ�������
  i2csr1 = I2C1_SR1;
  i2csr1 = I2C1_SR3;			//��״̬�Ĵ���1��״̬�Ĵ���2�������������ַ��־λ
  
  while(I2C1_SR1_bit.RXNE == 0);
  buffer = I2C1_DR;
  
//  i2csr1 = I2C1_SR1;
    
  I2C1_CR2_bit.ACK = 0;          //���һ���ֽڲ�����Ӧ���ź�
  I2C1_CR2_bit.STOP = 1;         //����ֹͣ�źŽ������ݴ���
  
  return buffer;

}


