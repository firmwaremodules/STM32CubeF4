/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
   
#include "cmsis_os.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/ 

/* For testing patching firmware update via the web interface:
 *  Set this to '1' for the initial firmware to load onto the board.
 *  Then set this to '2' before building the updated version.
 *  A difference is required between loads to generate a patch; and this
 *  define changes a bit of the binary to make this happen.
 * For regular full-image firmware updates this does not matter.
 * 
 * Regardless of whether created a patch or a new full image, the 'to' version in the
 * post-build line has to be updated for the build that will be used to update with
 * over the web interface.
 *          to      from
 * .... "v1.0.0" "v1.0.0" "NUCLEO-F429ZI" ...
 * 
 * 1. Create v1.0.0 :   "v1.0.0" "v1.0.0"
 * 2. Set FW_UPDATE_VERSION  2
 * 3. Create v1.1.0 :   "v1.1.0" "v1.0.0"   <- A patch v1.0.0_v1.1.0.sfbp as well as v1.1.0.sfb will be created.
 */
#define FW_UPDATE_VERSION       1
 
/*Static IP ADDRESS*/
#define IP_ADDR0   ((uint8_t)192U)
#define IP_ADDR1   ((uint8_t)168U)
#define IP_ADDR2   ((uint8_t)0U)
#define IP_ADDR3   ((uint8_t)10U)

/*NETMASK*/
#define NETMASK_ADDR0   ((uint8_t)255U)
#define NETMASK_ADDR1   ((uint8_t)255U)
#define NETMASK_ADDR2   ((uint8_t)255U)
#define NETMASK_ADDR3   ((uint8_t)0U)

/*Gateway Address*/
#define GW_ADDR0   ((uint8_t)192U)
#define GW_ADDR1   ((uint8_t)168U)
#define GW_ADDR2   ((uint8_t)0U)
#define GW_ADDR3   ((uint8_t)1U)

/* ST-LINK UART */
#define COM_UART                                USART3
#define COM_UART_CLK_ENABLE()                   __HAL_RCC_USART3_CLK_ENABLE()
#define COM_UART_CLK_DISABLE()                  __HAL_RCC_USART3_CLK_DISABLE()

#define COM_UART_TX_AF                          GPIO_AF7_USART3
#define COM_UART_TX_GPIO_PORT                   GPIOD
#define COM_UART_TX_PIN                         GPIO_PIN_8
#define COM_UART_TX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define COM_UART_TX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()

#define COM_UART_RX_AF                          GPIO_AF7_USART3
#define COM_UART_RX_GPIO_PORT                   GPIOD
#define COM_UART_RX_PIN                         GPIO_PIN_9
#define COM_UART_RX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define COM_UART_RX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */  


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
