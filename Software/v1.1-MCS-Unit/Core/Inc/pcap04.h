//
// Created by LiangGuang on 2023/4/16.
//

#ifndef GSRD_CORE1_PCAP04_H
#define GSRD_CORE1_PCAP04_H

#include "main.h"
#include "pcap04IIC.h"

#define defaultAddress (0x28 << 1)  //Default I2C address of PCAP04


void pcap04_cdc_complete_callback(void);
void pcap04_configure_registers(PCAP04IIC &pcap, pcap_config_t * pcap_config, uint8_t i2cAddress);
void pcap04_init(PCAP04IIC *pcap, pcap_config_t *configuration, unsigned char pcap_addr);
void pcap04_updateResults(PCAP04IIC * pcap, int pcapIndex);

void printResults(void);
float getCh1value(void);
float getCh2value(void);
float getCh4value(void);
float getCh5value(void);

#endif //GSRD_CORE1_PCAP04_H
