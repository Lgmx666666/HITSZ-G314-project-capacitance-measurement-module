#include<iostm8l151k4.h>
#include <stdint.h>
#include <stdio.h>
#include"usart1.h"
#include"pcap04.h"
#include"i2c.h"

unsigned char flag;

/*******************************************************************************
**�������ƣ�void TIM2_DelayMs(unsigned int ms)
**������������ʱ��2�ν��о�ȷ��ʱ����СΪ1΢�룬���65535
**��ڲ�����unsigned int ms     1=< us <= 65535 
**�������
*******************************************************************************/
void DelaynUs(unsigned int ms)
{ 
    
    TIM2_CR1 = 0x81;        //������ʱ��2��ʼ����
    while(ms--)
    {
      while( !(TIM2_SR1 & 0x01)); //�ȴ������Ƿ�ﵽ1΢��
      TIM2_SR1 &= ~(0x01);  //�������1���룬�����Ӧ�ı�־
    }
    TIM2_CR1 = 0x00;       //��ʱȫ���������رն�ʱ��2
}

void INTN_Init()
{
  PD_DDR_bit.DDR3 = 0;      //GPD->PIN3 ���ö˿�PD->3�������������Ĵ���Ϊ���뷽��
  PD_CR1_bit.C13 = 1;       //GPD->PIN3 ��������������
 // PD_CR2_bit.C23 = 1;       //GPD->PIN3  ʹ���ⲿ�ж�
  PD_CR2_bit.C23 = 0;       //GPD->PIN3  ʹ���ⲿ�ж� 

  
//  EXTI_CR3_bit.PDIS = 2;    //PortD�˿� �ⲿ�ж���2Ϊ�½��ش����ж�
//  EXTI_CONF_bit.PDLIS = 1;  //����PD��Ϊ�ⲿͨ���ж�
}


/*******************************************************************************
**�������ƣ�void CLKInit()
**��������������ϵͳʱ�Ӽ���������Ƶʱ��
**��ڲ�������
**�������
*******************************************************************************/
void CLKInit()
{
  CLK_CKDIVR = 0x00;                  //�ڲ�����ʱ��16MHZ ����1��Ƶ
  
  CLK_ICKCR_bit.LSION = 1;             // ��оƬ�ڲ��ĵ�������LSI
  
  while(CLK_ICKCR_bit.LSIRDY == 0);    // �ȴ������ȶ�
 
  CLK_CBEEPR_bit.CLKBEEPSEL0 = 1;     //ѡ���ڲ��ĵ�Ƶ/����ʱ����Ϊ��������ʱ��
  CLK_CBEEPR_bit.CLKBEEPSEL1 = 0;
}

int main(void)
{

  
  flag = 0;
  asm("sim");
  CLKInit();
  UART1_Init(9600);               //���ô���2��ʼ�������������ò�����Ϊ9600 bps
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


