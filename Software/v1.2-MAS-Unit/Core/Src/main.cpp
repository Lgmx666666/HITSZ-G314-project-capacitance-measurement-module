/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <cstdarg>
#include <cstdio>
#include "pcap04IIC.h"
#include "pcap04.h"
#include "angle_calculation.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APP_TX_DATA_SIZE  2048
#define READ_TIMES 3
#define WINDOW_LENGTH 50
#define OFFSET 0

#define CALIBRATION 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

uint16_t i2c_address = 0x28 << 1;
uint8_t readFlag = 0;
float resultArray[3][6][9];
float raw_chA, raw_chB, raw_chC, raw_chD;
float array_chA[WINDOW_LENGTH], array_chB[WINDOW_LENGTH], array_chC[WINDOW_LENGTH], array_chD[WINDOW_LENGTH], array_deltaC[WINDOW_LENGTH], array_ch0[WINDOW_LENGTH];
float data_chA, data_chB, data_chC, data_chD;
float coef1[3] = {-24.7624, 114.3560, 89.5810};
float coef2[3] = {-21.6856,-103.3736, 288.4259};
float coef3[3] = {108.1006, 353.0144, 4.5206};
float coef4[3] = {132.9996, -352.0608, 173.8599};
float edgevalue[2] = {-0.3571,  0.5352};
float bias = 0;
float output_theta = 0;

pcap_config_t my_Config_PCAP;
PCAP04IIC my_pcap(pcap04_version_t::PCAP04_V1, pcap_measurement_modes_t::STANDARD, defaultAddress, my_Config_PCAP);
ENCODER my_encoder(coef1, coef2, coef3, coef4, edgevalue, bias, fit_model::POLY2);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

void usb_printf(const char *format, ...)
{
  va_list args;
  uint32_t length;

  va_start(args, format);
  length = vsnprintf((char *)UserTxBufferFS, APP_TX_DATA_SIZE, (char *)format, args);
  va_end(args);
  HAL_UART_Transmit(&huart1, UserTxBufferFS, length, 100);
}

float medianAverageFilter(float *array, uint8_t len)
{
  float sum=0;
  float max=array[0], min=array[0];
  uint8_t i;

  for(i=0; i<len; i++)
  {
    if(array[i] < min) min = array[i];
    if(array[i] > max) max = array[i];
    sum += array[i];
  }
  return (sum - min - max)/((float)len-2);
}

void enqueue(float value, float *arr, uint8_t len){
  uint16_t i;
  for(i=len-1; i>0; i--){
    arr[i] = arr[i-1];
  }
  arr[0] = value;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(RLED_GPIO_Port, RLED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BLED_GPIO_Port, BLED_Pin, GPIO_PIN_SET);
  pcap04_init(&my_pcap, &my_Config_PCAP, i2c_address);
  HAL_Delay(100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    my_pcap.send_command(CDC_START);
    if(readFlag){
      HAL_NVIC_DisableIRQ(EXTI4_IRQn);
      readFlag = 0;
//      pcap04_updateResults(&my_pcap, 0);
//      printResults();
      raw_chA = getCh1value();
      raw_chB = getCh2value();
      raw_chC = getCh3value();
      raw_chD = getCh4value();
      enqueue(raw_chA, array_chA, WINDOW_LENGTH);
      enqueue(raw_chB, array_chB, WINDOW_LENGTH);
      enqueue(raw_chC, array_chC, WINDOW_LENGTH);
      enqueue(raw_chD, array_chD, WINDOW_LENGTH);

      HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    }

    data_chA = medianAverageFilter(array_chA, WINDOW_LENGTH);
    data_chB = medianAverageFilter(array_chB, WINDOW_LENGTH);
    data_chC = medianAverageFilter(array_chC, WINDOW_LENGTH);
    data_chD = medianAverageFilter(array_chD, WINDOW_LENGTH);
    output_theta = my_encoder.calculate_angle(data_chA, data_chB, data_chC, data_chD);

#if CALIBRATION
//    usb_printf("data:%.5f,%.5f\n", data_chB-data_chC, data_chD-data_chA);
    usb_printf("data:%.5f,%.5f,%.5f,%.5f\n", data_chA, data_chB, data_chC, data_chD);
#else
    usb_printf("data:%.2f\n", output_theta);
#endif

    HAL_GPIO_TogglePin(GLED_GPIO_Port, GLED_Pin);

    HAL_Delay(1);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RLED_Pin|GLED_Pin|BLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RLED_Pin GLED_Pin BLED_Pin */
  GPIO_InitStruct.Pin = RLED_Pin|GLED_Pin|BLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : EXT4_INTN_Pin */
  GPIO_InitStruct.Pin = EXT4_INTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EXT4_INTN_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_4){
    readFlag = 1;
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
