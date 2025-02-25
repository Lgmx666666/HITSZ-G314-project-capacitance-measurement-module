#ifndef ___I2C_H___
#define ___I2C_H___

void IIC_Init(void);
void IIC_Write(unsigned char subaddr , unsigned char Opcode ,unsigned char Byte_addr , unsigned char *buffer , unsigned short num);
void IIC_Write_Pcap04(unsigned char subaddr , unsigned char Opcode);
void IIC_Read(unsigned char subaddr , unsigned char Byte_addr , unsigned char *buffer , unsigned short num);
unsigned char IIC_Read_Pcap04(unsigned char subaddr , unsigned char Opcode);

#endif
