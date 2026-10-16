/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "debug.h"
#include "img_data.h"
#include "st7735s_driver.h"
#include "ch32v30x_rng.h"
/* Global typedef */

/* Global define */
#define Input_Up GPIO_Pin_0
#define Input_Down GPIO_Pin_2
#define Input_Left GPIO_Pin_4
#define Input_Right GPIO_Pin_6

#define Stop_Systick SysTick->CTLR &=0xFE;
#define Start_Systick SysTick->CTLR |=0x01;
/* Global Variable */
const uint8_t die_command_queue[]={0x2A,0x00,0x32,0x00,0x52,0x2B,0x00,0x06,0x00,0xA6,0x00};

uint8_t cartographic_information[64][64];//[x][y]，地图、身体的布局信息，0为没有东西，1表示这里的身体趋势是向上，2向下，3向左，4向右，5为苹果
__IO uint8_t State=0;//bit0-帧更新，当系统定时器到达设定值时会值1，bit1：苹果更新，应该放置一个苹果了
uint8_t input = 1;//1向上，2向下，3向左，4向右
uint8_t last_input = 1;

uint16_t Head_Position = 0x1F1F;//头部坐标，高8位为y值，低8位为x值，取值范围都是1~62，初始为31，31
uint8_t Tail_Update_Command[]={0x2A,0x00,64,0x00,65,0x2B,0x00,101,0x00,102,0x00};//尾部更新命令
uint16_t Tail_Position = 0x221F;//尾部坐标，高8位为y值，低8位为x值，初始值31，34
uint8_t s[10]={0};
uint8_t x[10]={0};
uint8_t z[10]={0};
uint8_t y[10]={0};
/*函数定义和声明*/
//系统定时器初始化，注意，使用了系统定时器后就不能使用delay_xx延时函数了
void SYSTICK_Init_Config(u_int64_t ticks)
{
//13500000-1为0.75秒
    SysTick->SR &= ~(1 << 0);//clear State flag
    SysTick->CMP = ticks;//计数比较器
    SysTick->CNT = 0;//计数器
    SysTick->CTLR = 0xF;

    NVIC_SetPriority(SysTicK_IRQn, 48);//设置中断优先级
//    NVIC_EnableIRQ(SysTicK_IRQn);
    NVIC->IENR[0]=(1<<12);//和上面被注释的那句一样的功能，使能中断
}
//4个游戏输入GPIO的初始化
void Input_Gpio_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_6;//分别对应上下左右
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//配置为下拉，按钮接应该接VCC
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//随机数发生器使能
void RNG_Enables(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG,ENABLE);
	RNG_Cmd(ENABLE);
}
//x坐标到绘画地址
uint8_t coordinate_x(uint8_t x){
	return x*2+2;
}
//y坐标到绘画地址
uint8_t coordinate_y(uint8_t y){
	return y*2+33;
}
//图片解码

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	uint8_t Rows_and_columns[11]={0x2A,0x00,0x02,0x00,0x81,
								  0x2B,0x00,0x01,0x00,0xA0,0x00};
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(9600);
	
	printf("This is printf example\r\n");
	
	SPI_st7735s_Init();
	Input_Gpio_Init();
	RNG_Enables();
	// M_SPI_st7735s_Init();
	// SPI1_IT_INT();

	Delay_Us(2);

	
	SPI1_Write_Command_Byte(0x01);//软件复位
	GPIO_SetBits(GPIOA,CSX);
	Delay_Ms(120);

	SPI1_Write_Command_Byte(0x11);//退出睡眠
	GPIO_SetBits(GPIOA,CSX);
	Delay_Ms(120);

	SPI1_Write_Command_Byte(0x39);//进入idle模式
	GPIO_SetBits(GPIOA,CSX);//片选拉高
	Delay_Us(10);

	SPI1_Write_Command_Byte(0x29);//打开显示
	GPIO_SetBits(GPIOA,CSX);//片选拉高
	Delay_Us(10);

	SPI1_Write_Command_Byte(0x3A);//设置颜色输入格式
	SPI1_WriteByte(0x05);
	GPIO_SetBits(GPIOA,CSX);//片选拉高
	Delay_Us(10);

	SPI1_WriteByte_IT(Rows_and_columns);
	GPIO_SetBits(GPIOA,CSX);//片选拉高
	Delay_Us(10);

	SPI1_Write_Command_Byte(0x2C);
	uint32_t number = 5248;
	while (number--) {
		SPI1_WriteWord(WHITE);
	}

	//开始解压文字图片
	draw(tan_chi_she,sizeof(tan_chi_she));
	draw(kai_shi,sizeof(kai_shi));
	
	number = 9472;
	while (number--) {
		SPI1_WriteWord(WHITE);
	}

	printf("成功");
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);//忙标志
	GPIO_SetBits(GPIOA,CSX);//片选拉高
	Delay_Ms(1000);
	while (!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)){};//等待按钮按下
	printf("按下按钮");
	Rows_and_columns[7]+=32;
	Rows_and_columns[9]+=32;//行地址+32
	SPI1_WriteByte_IT(Rows_and_columns);

	SPI1_Write_Command_Byte(0x2C);
	number=256;
	while (number--) {
		SPI1_WriteWord(BLACK);
	}
	number=124;
	while (number--) {
		SPI1_WriteWord(BLACK);
		SPI1_WriteWord(BLACK);
		uint16_t ls =124;
		while (ls--) {
			SPI1_WriteWord(WHITE);
		}
		SPI1_WriteWord(BLACK);
		SPI1_WriteWord(BLACK);
	}
	number=258;
	while (number--) {
		SPI1_WriteWord(BLACK);
	}

	Rows_and_columns[2]=64;
	Rows_and_columns[4]=65;
	Rows_and_columns[7]=95;
	Rows_and_columns[9]=102;//一节头3节身体
	cartographic_information[31][32]=1;
	cartographic_information[31][33]=1;
	cartographic_information[31][34]=1;
	cartographic_information[31][30]=5;//放置苹果
	SPI1_WriteByte_IT(Rows_and_columns);
	
	SPI1_Write_Command_Byte(0x2C);
	number=4;
	while (number--) {
		SPI1_WriteWord(BLACK);
	}
	number=12;
	while (number--) {
		SPI1_WriteWord(GREEN);
	}
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
	GPIO_SetBits(GPIOA,CSX);//片选拉高
	Delay_Ms(100);
	SYSTICK_Init_Config(13500000-1);
	//游戏主循环
	while (1) {
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) && (last_input!=2)) {
		input = 1;
	}else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) && (last_input!=1)) {
		input = 2;
	}else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) && (last_input!=4)) {
		input =3;
	}
	else if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) && (last_input!=3)) {
		input =4;
	}

	if (State & 1) {
		last_input = input;
		State = 0;
		cartographic_information[Head_Position&0xFF][Head_Position>>8]=input;//将地图上位于 头部坐标 的内存设置为输入的值
		switch(input)//根据输入更新头部坐标
		{
			case 1:
				Head_Position-=0x100;
				break;
			case 2:
				Head_Position+=0x100;
				break;
			case 3:
				Head_Position-=0x1;
				break;
			case 4:
				Head_Position+=0x1;
				break;
		}
		if ((Head_Position>>8)>62 || (Head_Position>>8)<1) {//如果头部y值超出边界，说明撞到墙，游戏结束
			break;
		}
		if ((Head_Position&0xFF)>62 || (Head_Position&0xFF)<1 ) {//如果头部x值超出边界，说明撞到墙，游戏结束
			break;
		}

		if (cartographic_information[Head_Position&0xFF][Head_Position>>8]==5) {//吃到苹果，身体变长，不清除尾部
			printf("ap");
			cartographic_information[Head_Position&0xFF][Head_Position>>8]=0;//清除苹果，不然下面会判定撞到身体
			State += 2;
		}
		else  {//普通移动，清除尾部
			Tail_Update_Command[2]=coordinate_x(Tail_Position&0xFF);
			Tail_Update_Command[4]=Tail_Update_Command[2]+1;
			Tail_Update_Command[7]=coordinate_y(Tail_Position>>8);
			Tail_Update_Command[9]=Tail_Update_Command[7]+1;
			SPI1_WriteByte_IT(Tail_Update_Command);
			SPI1_Write_Command_Byte(0x2C);
			for (uint8_t i =0; i!=4; i++) {
				SPI1_WriteWord(WHITE);
			}
			uint8_t ls = cartographic_information[Tail_Position&0xFF][Tail_Position>>8];
			cartographic_information[Tail_Position&0xFF][Tail_Position>>8]=0;//地图中的尾部坐标清0
			
			switch (ls) 
			{
				case 1:
					Tail_Position-=0x100;
					break;
				case 2:
					Tail_Position+=0x100;
					break;
				case 3:
					Tail_Position-=0x1;
					break;
				case 4:
					Tail_Position+=0x1;
					break;
			}
		}
		if (cartographic_information[Head_Position&0xFF][Head_Position>>8]!=0) {//检测有没有撞到身体
			// printf("碰到身体");
			break;
		}
		//接下来是移动头部
		switch(input)//根据输入更新头部坐标
		{
			case 1:
				Rows_and_columns[2]=coordinate_x(Head_Position&0xFF);
				Rows_and_columns[4]=Rows_and_columns[2]+1;
				Rows_and_columns[7]=coordinate_y(Head_Position>>8);
				Rows_and_columns[9]=Rows_and_columns[7]+3;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
				SPI1_WriteByte_IT(Rows_and_columns);
				SPI1_Write_Command_Byte(0x2C);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				break;
			case 2:
				Rows_and_columns[2]=coordinate_x(Head_Position&0xFF);
				Rows_and_columns[4]=Rows_and_columns[2]+1;
				Rows_and_columns[7]=coordinate_y((Head_Position>>8)-1);
				Rows_and_columns[9]=Rows_and_columns[7]+3;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
				SPI1_WriteByte_IT(Rows_and_columns);
				SPI1_Write_Command_Byte(0x2C);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				break;
			case 3:
				Rows_and_columns[2]=coordinate_x(Head_Position&0xFF);
				Rows_and_columns[4]=Rows_and_columns[2]+3;
				Rows_and_columns[7]=coordinate_y(Head_Position>>8);
				Rows_and_columns[9]=Rows_and_columns[7]+1;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
				SPI1_WriteByte_IT(Rows_and_columns);
				SPI1_Write_Command_Byte(0x2C);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				break;
			case 4:
				Rows_and_columns[2]=coordinate_x((Head_Position&0xFF)-1);
				Rows_and_columns[4]=Rows_and_columns[2]+3;
				Rows_and_columns[7]=coordinate_y(Head_Position>>8);
				Rows_and_columns[9]=Rows_and_columns[7]+1;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
				SPI1_WriteByte_IT(Rows_and_columns);
				SPI1_Write_Command_Byte(0x2C);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				SPI1_WriteWord(GREEN);SPI1_WriteWord(GREEN);
				SPI1_WriteWord(BLACK);SPI1_WriteWord(BLACK);
				break;
		}
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
		GPIO_SetBits(GPIOA,CSX);

		if (State&0x2) {//等身体和头部都稳定下来后再更新苹果
			// printf("ua");
			uint16_t ap_po = RNG_GetRandomNumber();
			uint8_t x = (ap_po&0xFF)%62 + 1;
			uint8_t y = (ap_po>>8)%62 + 1;//限制苹果坐标在1~63之间
			ap_po = (y<<8) + x;
			if ((!cartographic_information[x][y]) && ap_po!=Head_Position) {//如果地图对应坐标没有身体，并且和头部不重叠，就可以放置苹果
				cartographic_information[x][y]=5;
				State &=0xFD;
				Rows_and_columns[2]=coordinate_x(x);
				Rows_and_columns[4]=Rows_and_columns[2]+1;
				Rows_and_columns[7]=coordinate_y(y);
				Rows_and_columns[9]=Rows_and_columns[7]+1;
				SPI1_WriteByte_IT(Rows_and_columns);
				SPI1_Write_Command_Byte(0x2C);
				for (uint8_t i=0; i!=4; i++) {
					SPI1_WriteWord(RED);
				}
			}
		}
		}
	}
	Stop_Systick
	GPIO_SetBits(GPIOA,CSX);
	printf("失败y:0x%X x:0x%x",Head_Position>>8,Head_Position&0xFF);
	Rows_and_columns[2]=50;
	Rows_and_columns[4]=81;
	Rows_and_columns[7]=1;
	Rows_and_columns[9]=16;
	SPI1_WriteByte_IT(Rows_and_columns);
	SPI1_Write_Command_Byte(0x2C);
	draw(shi_bai,sizeof(shi_bai));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
	GPIO_SetBits(GPIOA,CSX);
	/*模拟与硬件SPI分界线*/

	// M_SPI_st7735s_Init();
	// Delay_Us(20);
	// uint8_t b[5];
	// M_SPI1_Write_Command_Byte(0x01);//软件复位
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Ms(120);

	// M_SPI1_Write_Command_Byte(0x11);//退出睡眠
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Ms(120);


	// M_SPI1_Write_Command_Byte(0x39);//进入idle模式
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0x29);//打开显示
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0x0A);//读取电力模式
	// b[0]=M_SPI1_Receive_Parameters_Byte();
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0x3A);//设置像素格式
	// M_SPI1_Write(5);
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0X0B);//读取状态
	// b[1]=M_SPI1_Receive_Parameters_Byte();
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0X0C);

	// M_SPI1_Write_Command_Byte(0X2A);//列地址设置
	// M_SPI1_Write(0x00);
	// M_SPI1_Write(2);
	// M_SPI1_Write(0x00);
	// M_SPI1_Write(129);
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0X2B);//行地址设置
	// M_SPI1_Write(0x00);
	// M_SPI1_Write(32);
	// M_SPI1_Write(0x00);
	// M_SPI1_Write(95);
	// GPIO_SetBits(GPIOA,CSX);//片选拉高
	// Delay_Us(20);

	// M_SPI1_Write_Command_Byte(0X2C);//内存写入
	// // GPIO_ResetBits(GPIOA,CSX);//拉低片选
	// uint16_t ccc = WHITE;
	// for (uint16_t i=0; i!=256; i=i+1) {
	// 	M_SPI1_Write(ccc>>8);
	// 	M_SPI1_Write(ccc&0xFF);
	// }
	// GPIO_SetBits(GPIOA,CSX);//片选拉高


	// printf("读取电力模式 0x%X ，像素格式 0x%X",b[1],b[2]);

	while(1)
    {
		// }
	}
}