/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/main.c 
  * @author  MCD Application Team
  * @brief   This sample code implements a http server application based on 
  *          Netconn API of LwIP stack and FreeRTOS. This application uses 
  *          STM32F4xx the ETH HAL API to transmit and receive data. 
  *          The communication is done with a web browser of a remote PC.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "app_ethernet.h"
#include "httpserver-netconn.h"
#include "stm32_secure_patching_bootloader_interface_v1.3.0.h"
#ifdef USE_LCD
#include "lcd_log.h"
#endif
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif gnetif; /* network interface structure */

static UART_HandleTypeDef           UartHandle; /*!< Uart Handler*/

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void StartThread(void const * argument);
static void BSP_Config(void);
static void Netif_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();  
  
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  
  /* Initialize LCD and LEDs */
  BSP_Config();

  /* stm32-secure-patching-bootloader v1.3.0 errata:
      [1] CRC hardware peripheral's power must be enabled by the user application
          prior to calling SE_PATCH_Data() APIs.
    */
  __HAL_RCC_CRC_CLK_ENABLE();

  printf("\nLwIP_HTTP_Server_Netconn_RTOS - NUCLEO-F429ZI\n");

  printf("Built FW_UPDATE_VERSION=%d\n", FW_UPDATE_VERSION);

  /* Get the firmware version embedded in the active slot header using the
 * Secure Engine services.
 */
  SE_StatusTypeDef se_Status = SE_KO;
  SE_APP_ActiveFwInfo sl_FwInfo;
  memset(&sl_FwInfo, 0xFF, sizeof(SE_APP_ActiveFwInfo));
  SE_APP_GetActiveFwInfo(&se_Status, &sl_FwInfo);
  printf("Firmware Version: %ld.%ld.%ld\n",
      FW_VERSION_MAJOR(sl_FwInfo.ActiveFwVersion),
      FW_VERSION_MINOR(sl_FwInfo.ActiveFwVersion),
      FW_VERSION_PATCH(sl_FwInfo.ActiveFwVersion));
  char bootver[32];
  memset(bootver, 0, sizeof(bootver));
  SE_APP_GetBootVer(&se_Status, bootver, sizeof(bootver));
  printf("Bootloader Version: %s\n", bootver);

  /* Init thread */
#if defined(__GNUC__)
  osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
#else
  osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
#endif
  
  osThreadCreate (osThread(Start), NULL);
  
  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}

/**
  * @brief  Start Thread 
  * @param  argument not used
  * @retval None
  */
static void StartThread(void const * argument)
{
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);
  
  /* Initialize the LwIP stack */
  Netif_Config();
  
  /* Initialize webserver demo */
  http_server_netconn_init();

  for( ;; )
  {
    /* Delete the Init Thread */ 
    osThreadTerminate(NULL);
  }
}

/**
  * @brief  Initializes the STM324x9I-EVAL's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
static void BSP_Config(void)
{
#ifdef USE_LCD

  /* Initialize the LCD */
  BSP_LCD_Init();

  /* Initialize the LCD Layers */
  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);
  
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  
  /* Initialize LCD Log module */
  LCD_LOG_Init();
  
  /* Show Header and Footer texts */
  LCD_LOG_SetHeader((uint8_t *)"Webserver Application Netconn API");
  LCD_LOG_SetFooter((uint8_t *)"STM324x9I-EVAL board");
  
  LCD_UsrLog ((char *)"  State: Ethernet Initialization ...\n");

#else
  /* Configure LED1 and LED2 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

#endif /* USE_LCD */

  UartHandle.Instance = COM_UART;
  UartHandle.Init.BaudRate = 115200U;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_RX | UART_MODE_TX;

  HAL_UART_Init(&UartHandle);
}

/**
  * @brief  Configures the network interface
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

#if LWIP_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* LWIP_DHCP */

  /* add the network interface */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  osThreadDef(EthLink, ethernet_link_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE *2);
  osThreadCreate (osThread(EthLink), &gnetif);
#endif

#if LWIP_DHCP
  /* Start DHCPClient */
  osThreadDef(DHCP, DHCP_Thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(DHCP), &gnetif);
#endif
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
   while(1) {};
  }
  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
   while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
   while(1) {};
  }
}

/**
  * @brief  UART MSP Init.
  * @param  huart: UART handler pointer.
  * @retval None.
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (huart->Instance == COM_UART)
    {
        /* Peripheral Clock Enable */
        COM_UART_CLK_ENABLE();

        /* GPIO Ports Clock Enable */
        COM_UART_TX_GPIO_CLK_ENABLE();
        COM_UART_RX_GPIO_CLK_ENABLE();

        /*Configure GPIO pins : COM_UART_TX_Pin  */
        GPIO_InitStruct.Pin = COM_UART_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = COM_UART_TX_AF;
        HAL_GPIO_Init(COM_UART_TX_GPIO_PORT, &GPIO_InitStruct);

        /*Configure GPIO pins : COM_UART_RX_Pin  */
        GPIO_InitStruct.Pin = COM_UART_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = COM_UART_RX_AF;
        HAL_GPIO_Init(COM_UART_RX_GPIO_PORT, &GPIO_InitStruct);
    }
}

#if defined(__ICCARM__)
#error not supported
#elif defined(__CC_ARM)
#error not supported
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define WRITE_PROTOTYPE int _write(int file, char* ptr, int len)
#endif

WRITE_PROTOTYPE
{
    HAL_UART_Transmit(&UartHandle, (uint8_t*)ptr, len, 0xFFFF);
    return len;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval ch
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART and Loop until the end of transmission */
    HAL_UART_Transmit(&UartHandle, (uint8_t*)&ch, 1U, 0xFFFFU);

    return ch;
}

void print_buf(const uint8_t* buf, int len)
{
    for (int i = 0; i < len; i++) {
        uint8_t c = buf[i];
        if (c == 0) {
            char b[] = { 'n','u','l' };
            HAL_UART_Transmit(&UartHandle, (uint8_t*) & b, sizeof(b), 100);
        } else {           
            HAL_UART_Transmit(&UartHandle, &c, 1, 100);
        }
    }
    uint8_t n = '\n';
    HAL_UART_Transmit(&UartHandle, &n, 1, 100);
}

void print_hex_buf(const uint8_t* buf, int len)
{
    for (int i = 0; i < len; i++) {
        uint8_t c = buf[i];
        printf("%02X ", c);
        if (((i + 1) % 16) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif
