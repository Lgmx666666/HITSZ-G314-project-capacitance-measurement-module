//
// Created by LiangGuang on 2023/4/16.
//

#include "pcap04.h"

#define PCAP04_I2C  0x28 << 1
// Opcode 0x40 + Result Register RES01 0x04
#define REG         0x44
// OPCODES
#define W_MEM      0xA0
#define W_CONFIG   0xA3
#define POR         0x88
#define INIT  0x8A
#define CDC_START   0x8C
#define RDC_START   0x8E
#define NV_STORE    0x96
#define TEST        0x7e

pcap_results_t* pcap_results;
pcap_status_t* pcap_status;

extern I2C_HandleTypeDef hi2c1;

void pcap04_configure_registers(PCAP04IIC &pcap, pcap_config_t * pcap_config, unsigned char i2cAddress){

  //Serial.println("current config");
  //pcap.print_config();

  *pcap_config = pcap.get_config();
  //Reg0 settings
//    pcap_config->I2C_A = i2cAddress;          //Set I2C address to 43
//    pcap_config->OLF_CTUNE = 0x01;            //Set low frequency clock
//    pcap_config->OLF_FTUNE = 0x07;            //Finetune low frequency clock
//    //Reg1 settings
//    //Reg2 settings
//    pcap_config->RDCHG_INT_SEL1 = 0x00;       //Discharge resistors PC4-PC5
//    pcap_config->RDCHG_INT_SEL0 = 0x00;       //Discharge resistors PC0-3 plus PC6
//    pcap_config->RDCHG_INT_EN = 0x01;         //Enable internal discharge resistors
//    //Reg3 settings
//    pcap_config->RCHG_SEL = 0x00;             //Charging resistors to limit charging
//    //Reg4 settings
  pcap_config->C_REF_INT = 1;            //Use the internal reference
//    pcap_config->C_COMP_EXT = 0x00;           //External cap compensation (DISABLE FOR NON FLOATING CAPS)
  pcap_config->C_COMP_INT = 0x01;            //Internal cap compensation
//    pcap_config->C_DIFFERENTIAL = 0x00;       //No differential caps
//    pcap_config->C_FLOATING = 0x00;           //Floating caps 此处修改
//    //Reg5 settings
//    pcap_config->CY_HFCLK_SEL = 0x00;         //Low frequency clock (50 kHz)
//    //Reg6 settings
  pcap_config->C_PORT_EN = 0x36;   // 端口开启选择位
//    //Reg7:8 settings
//    pcap_config->C_AVRG = 0x0d;               //Samples to take average from
//    //Reg11:9
  pcap_config->CONV_TIME = 0x01F4;          //Conversion time (10000):0x2710
//    //Reg12 settings
//    pcap_config->DISCHARGE_TIME = 0x10;       //Discharge time (x*50 kHz)
//    //Reg13 settings
//    pcap_config->C_STARTONPIN = 0x00;         //Pin that triggers a CDC measurement (only if trig enabled)
    pcap_config->C_TRIG_SEL = 0x00;           //Timer triggered reading  此处修改
//    //Reg14 settings
//    pcap_config->PRECHARGE_TIME = 0x10;       //Precharge time (x*50 kHz)
//    //Reg15 settings
//    pcap_config->C_FAKE = 0x04;               //Number of "fake" or "warm-up" measurements before "real"
//    //Reg16 Settings
//    pcap_config->FULLCHARGE_TIME =  0x10;     //Time to charge without I_lim
//    //Reg17 settings
  pcap_config->C_REF_SEL = 1;  //Reference capacitances (0 = min, 1 = 1pF ... 31 = 31 pF) 此处修改
//    //Reg18 settings
  pcap_config->C_G_OP_EXT = 0;
  pcap_config->C_G_EN = 0b000000;           //Guard enabled for ports
//    //Reg19 settings
//    pcap_config->C_G_OP_VU = 0;               //Gain for the guard (0 = 1.00, 3 = 1.03)
//    //Reg20 settings
//    pcap_config->C_G_OP_TR = 7;               //Guard OP current trim
//    //Reg21 settings
//    pcap_config->R_TRIG_PREDIV = 0x0a;
//    //Reg22 settings
//    pcap_config->R_TRIG_SEL = 0x00;           //Trigger for RDC. 5 = asynchronous at end of CDC
//    pcap_config->R_AVRG = 0x00;               //Sample size for mean value
//    //Reg23 settings
//    pcap_config->R_PORT_EN = 0b00;            //Activate R_PORTS, bit0 = PT0REF, bit1 = PT1
//    pcap_config->R_PORT_EN_IMES = 0b1;        //Activate internal aluminium temp sensor
//    pcap_config->R_PORT_EN_IREF = 0b1;        //Activate internal reference resistor
//    pcap_config->R_FAKE = 0x00;               //Number of "fake" or "warm-up" measurements before "real"
//    //Reg24, 25, 26 settings (Mandatory bits)
//    //Reg27-29 settings
//    //Reg30 settings
//    pcap_config->PG5_INTN_EN = true;          //Route INTN signal to PG5
//    //Reg35 settings
//    pcap_config->CDC_GAIN_CORR = 0x40;        //Firmware defined gain correlation factor
//    //Reg29 settings
//
//    //Reg42 settings
//    pcap_config->EN_ASYNC_READ = 0;           //Enable Synchronised read (Update results based on pin interrupt)
//
//    pcap_config->RUNBIT = 1;

  pcap.update_config(pcap_config);

  //Serial.println("updated config");
  //pcap.print_config();

}

void pcap04_init(PCAP04IIC *pcap, pcap_config_t *configuration, unsigned char pcap_addr) {
  int pcap_addr_conf = 0x00;
//    pcap->update_address(defaultAddress);
//    //Enable I2C on the selected device
//
//    while (pcap->test_connection() == false){  //Test if connection can be made with the default address
//        usb_printf("Default address failed, trying with already correct address\r\n");
//      Serial.println("Default address failed, trying with already correct address");
//        pcap->update_address(pcap_addr);
//        if (pcap->test_connection() == true){    //Test if the connection can be made with the already changed address
//                   //Do a power on reset to reset the address
//            pcap->update_address(defaultAddress);
//            break;
//        }
//        pcap->update_address(defaultAddress);
//
//        usb_printf("Connection to PCAP04 failed!! Retrying in 3 second\r\n");
//        HAL_Delay(3000);
//    }
//
//    //Connection has been made begin initialisation sequence
//    usb_printf(" Initialising PCAP04 \r\n ");
//
//    pcap->init_nvram();
//    pcap04_configure_registers(*pcap, configuration,pcap_addr_conf);
//    pcap->initializeIIC();

  pcap->send_command(POR_RESET);
  pcap->send_command(INITIALIZE_OP);
  pcap->test_connection();
  pcap->init_nvram();
  pcap->initializeIIC();
  pcap04_configure_registers(*pcap, configuration,pcap_addr_conf);
  pcap->send_command(CDC_START);


}

extern float resultArray[3][6][9];
void pcap04_updateResults(PCAP04IIC * pcap, int pcapIndex){
  pcap_status = pcap->get_status(false);
  pcap->cdc_complete_flag = false;
  pcap_results = pcap->get_results();


  if (pcap_status->COMB_ERR){
    usb_printf("OUTPUT ERROR IN PCAP04- %d\r\n", pcapIndex+1);
    return;
  }
//    resultArray[pcapIndex][0][0] = (pcap_results->C0_over_CREF * multiplicationFactors[pcapIndex] - zeroingFactors[pcapIndex][0]);
//    resultArray[pcapIndex][1][0] = (pcap_results->C1_over_CREF * multiplicationFactors[pcapIndex] - zeroingFactors[pcapIndex][1]);
//    resultArray[pcapIndex][2][0] = (pcap_results->C2_over_CREF * multiplicationFactors[pcapIndex] - zeroingFactors[pcapIndex][2]);
//    resultArray[pcapIndex][3][0] = (pcap_results->C3_over_CREF * multiplicationFactors[pcapIndex] - zeroingFactors[pcapIndex][3]);
//    resultArray[pcapIndex][4][0] = (pcap_results->C4_over_CREF * multiplicationFactors[pcapIndex] - zeroingFactors[pcapIndex][4]);
//    resultArray[pcapIndex][5][0] = (pcap_results->C5_over_CREF * multiplicationFactors[pcapIndex] - zeroingFactors[pcapIndex][5]);
//     resultArray[pcapIndex][0][0] = pcap_results->C0_over_CREF; //- 3.0950;
  resultArray[pcapIndex][1][0] = pcap_results->C1_over_CREF; //- 3.0450;
  resultArray[pcapIndex][2][0] = pcap_results->C2_over_CREF; //- 3.1300;
  resultArray[pcapIndex][3][0] = pcap_results->C3_over_CREF; //- 3.2750;
  resultArray[pcapIndex][4][0] = pcap_results->C4_over_CREF; //- 3.2950;
  resultArray[pcapIndex][5][0] = pcap_results->C5_over_CREF; //- 3.1750;

  //resultIndexes[pcapIndex] = resultIndexes[pcapIndex] + 1;
  // if (resultIndexes[pcapIndex] > sizeof(resultArray[pcapIndex][0][resultIndexes[pcapIndex]])/sizeof(float) - 1)
  // {
  //   resultIndexes[pcapIndex] = 0;
  // }

//    newResults = true;


}


void printResults(){
  usb_printf("PC1=%.4f\r\n", resultArray[0][1][0]);
//    usb_printf("Printing PCAP Value>> PC0:%.4f PC1:%.4f PC2:%.4f PC3:%.4f PC4:%.4f PC5:%.4f PC6:%.4f\r\n",
//               resultArray[0][0][0], resultArray[0][1][0], resultArray[0][2][0], resultArray[0][3][0],
//               resultArray[0][4][0], resultArray[0][5][0], resultArray[0][6][0]);
}

float getCh1value(){
  return resultArray[0][1][0];
}

float getCh2value(){
  return resultArray[0][2][0];
}

float getCh4value(){
  return resultArray[0][4][0];
}

float getCh5value(){
  return resultArray[0][5][0];
}