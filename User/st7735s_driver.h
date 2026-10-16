#include "ch32v30x_gpio.h"
#include "ch32v30x_spi.h"

#ifndef ST7735S
#define ST7735S


#define CSX GPIO_Pin_1//片选
#define DCX GPIO_Pin_3//命令选择，低为命令，高为数据
#define SCL GPIO_Pin_5
#define SDA GPIO_Pin_7

#define BULE 0x0010
#define GREEN   0x0400
#define RED  0x8000
#define CYAN    0x0410//青色
#define PURPLE  0x8010
#define YELLOW  0x8400
#define BLACK   0x0000
#define WHITE   0X8410
/*外部文件定义的变量*/
extern GPIO_InitTypeDef GPIOA7_shuchu;
extern GPIO_InitTypeDef GPIOA7_shuru;
extern uint8_t Ready;
/*本文件定义的变量*/
extern uint8_t *Command_P;
extern uint8_t *Parameter_P;
extern uint16_t Remaining_Progress;

#define jie接收模式 SPI1->CTLR1&=0xBFFF;
#define fa发送模式 SPI1->CTLR1|=0x4000;
#define shu数据 GPIO_SetBits(GPIOA,GPIO_Pin_3);
#define ming命令 GPIO_ResetBits(GPIOA,GPIO_Pin_3);
#define GPIOA7设置为输出 GPIO_Init(GPIOA, &GPIOA7_shuchu);
#define GPIOA7设置为输入 GPIO_Init(GPIOA, &GPIOA7_shuru);

#define TXE_IT_set SPI1->CTLR2=SPI1->CTLR2 | 0x80
#define TXE_IT_reset SPI1->CTLR2=SPI1->CTLR2 & 0x7F

#include "debug.h"
void SPI_st7735s_Init(void);
void SPI1_IT_INT(void);
void M_SPI_st7735s_Init(void);

void SPI1_WriteByte(u8 TxData);
void SPI1_WriteByte_IT(u8 *Command_queue);
void SPI1_WriteWord(u16 TxData);
void SPI1_WriteByte_D(u8 *TxData,u8 size);
void SPI1_Write_Command_Byte(u8 TxData);
void SPI1_Auto_Write_Command_p(uint8_t cmd);

void M_SPI1_Write(u8 TxData);
void M_SPI1_Write_Command_Byte(u8 TxData);
uint8_t M_SPI1_Receive_Parameters_Byte(void);
void 虚拟时钟(void);
void M_SPI1_Write_D(u8 *TxData,u8 size);
u8 SPI1_ReadByte(void);
#endif