/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/06
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_it.h"
#include "st7735s_driver.h"
/*全局变量定义*/
uint8_t control_byte=2;//控制字节，0位控制参数指针是否自增，1位控制发送16位数据
uint8_t state_byte=1;//状态字节，0位为1表示将要发送数据高位，高位低位交替切换发送

extern uint8_t State;

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SPI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
  NVIC_SystemReset();
  while (1)
  {
  }
}
//spi回调函数
void SPI1_IRQHandler(void)
{
  if(Remaining_Progress!=0){//检查命令参数有没有发送完成
    
    if (control_byte&2){
      if(state_byte&1){//发送高位
        SPI_I2S_SendData(SPI1,*(Parameter_P+1));
        state_byte = state_byte - 1;//第0位设置为0
      }else {
        SPI_I2S_SendData(SPI1,*Parameter_P);
        state_byte = state_byte + 1;//第0位设置为1
      }
    }else {
      SPI_I2S_SendData(SPI1,*Parameter_P);
    }
    if (control_byte&1){
      Parameter_P++;
    }
    Remaining_Progress--;
  }
  else {
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);//等待当前数据发送完成
    GPIO_SetBits(GPIOA,CSX);
    TXE_IT_reset;
  }
}
//系统定时器回调函数
void SysTick_Handler(void){
    if(SysTick->SR == 1)
    {
        SysTick->SR = 0;//clear State flag
        State=State+1;
    }
}