/**
@page TouchGFX Demonstration for STM32F469 Discovery board

@verbatim
/**
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
@endverbatim

/*
 * Copyright (c) 2021-2023 Firmware Modules Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 */

To get this reference design to build, your best bet is to download the actual full STM32CubeF4 package from ST version 1.27.0,
then copy over Middlewares/ST/TouchGFX/touchgfx and Middlewares/ST/STM32_Audio/Addons/* 
These components are missing from the GitHub version.

The TouchGFX demo's GUI assets comprise over half of the external flash's capacity (16 MB).  Since we need a download slot (SLOT1) equal in
size to the active slot (SLOT0), and the download slot must reside in external flash, we have to trim the GUI assets down to fit
into the portion of SLOT0 assigned to the external flash (SEG1).

How we've done this is to allocate 8 MB to SLOT1 and then split SLOT0 into 1 MB internal flash and 7 MB external flash.
This is pre-defined and configured in the stm32-secure-patching-bootloader_DISCO-F469I_vXXXX.bin

Then we reduced the GUI asset requirement by cutting out the "Bird Eat Coin" (GAME2D) portion of the demo.
This reduction is controlled by C++ preprocessor define TOUCHGFX_DISABLE_GAME2D.  With this defined,
total GUI assets in QSPI (SLOT0 SEG1) are around 6.58 MB.  The executable application portion in SLOT0 SEG0 sits at about 223 KB.

Connect a terminal to the STLINK VCOM port to see bootloader progress.  The application launches as soon as the bootloader
finishes verifying the application binary in SLOT0.

To load the application .hex file, use STM32CubeProgrammer and ensure to select an appropriate external flash loader before
connecting to the board and programming.  The file to program is in Project/Binary/BOOT_TouchGFX_STM32469I-Discovery_v1.0.0.hex

[  0.000] Bootloader starting up.
STM32 Secure Patching Bootloader. github.com/firmwaremodules/stm32-secure-patching-bootloader
Build: v1.4-preview1
Registered to: unregistered
Target: DISCO-F469I
UID: 1650335351326f20
Clock:HSE,8,180 Crypto:SW UART:3,115200,N81
SLOT0:08020000 SLOT1:90800000 SIZE:800000
APP RAMSTART:20002700 VTOR:08020200
[  0.031] Target check: CPUID:410FC241 IDCODE:10006434 FLASHSZ:0800 .. OK.
[  0.289] Verify bootloader.
[  0.319] SHA256: 023f7996d2b9456571aa792e177bd89f1fa8b911d4cf50675fae27829c71fc90 Valid
[  0.327] UART loader check trigger.
[  1.332] Check USB flash media.
[  2.334] No valid firmware found on flash media, status=1
[  2.340] Verify slot 1 header.
[  2.343] Slot 1 is empty.
[  2.345] Verify slot 0.
[  2.347] Verify slot 0 header.
[  2.483] Verify slot 0 signature.
[  7.384] Verify slot 0 ready.
[  7.386] Verify slot 0 fw ver.
[  7.522] Slot 0 has valid active firmware version 1.0.0
[  7.527] Preparing to launch application in slot 0.



Other than the disabling of the GAME2D, the demo is left the same except of course for the stm32-secure-patching-bootloader integration.
The bootloader will verify and boot the application and will allow you to update it from
the bootloader's built-in YMODEM/UART updater or USB flash drive.  Place the TouchGFX_STM32469I-Discovery_v1.0.0.sfb or TouchGFX_STM32469I-Discovery_v1.0.0_v1.1.0.sfbp file 
onto the root of a USB flash drive and have it connected to the "USB USER" port of the discovery kit (through USB micro - female A adapter) when powering/resetting it.

There is no IAP update demo implemented at this time, but it is fully capable if updating itself using methods we demonstrated in these
projects and others:

https://github.com/firmwaremodules/STM32CubeF4/tree/master/Projects/STM32F429ZI-Nucleo/Applications/LwIP/LwIP_HTTP_Server_Netconn_RTOS
https://github.com/firmwaremodules/STM32CubeL4/tree/master/Projects/32L4R9IDISCOVERY/Applications/FreeRTOS/FreeRTOS_LowPower



@par Description

Official TouchGFX demonstration developed jointly between ST and Draupner Graphics
for the STM32F469 Discovery board.

Requirements:
	Resolution: 800 * 480
	BPP: 16

The demonstration features the following modules:
    - Bird Eats Coin Game
    - Graph Demo
    - Custom Controls Demo
    - TouchGFX Presentation Demo

@par Keywords

Demonstration, TouchGFX

@par How to use it ?

Please use the STM32CubeProgrammer utility to flash the application's HEX file to the STM32 target.


*/
