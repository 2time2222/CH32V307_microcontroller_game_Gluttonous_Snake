#include "st7735s_driver.h"

uint8_t *Command_P;//当前在哪个命令队列
uint8_t *Parameter_P;//预备发送的参数的指针
uint16_t Remaining_Progress;//要发送多少个命令参数

GPIO_InitTypeDef GPIOA7_shuchu = {
    .GPIO_Pin=GPIO_Pin_7,
    .GPIO_Mode=GPIO_Mode_Out_PP,
    .GPIO_Speed=GPIO_Speed_50MHz};

GPIO_InitTypeDef GPIOA7_shuru = {
    .GPIO_Pin=GPIO_Pin_7,
    .GPIO_Mode=GPIO_Mode_IPD,};


//硬件SPI初始化
void SPI_st7735s_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//cs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//w25q32的片选，拉高防止误触
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_2, 1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//命令选择，低为命令，高为数据
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_3);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//CLK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//SPI1_MOSI,半双工模式下数据输入输出引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);    
}
//中断控制器中关于SPI1的部分的初始化，但是SPI控制寄存器还没有哪个中断被允许
void SPI1_IT_INT(void){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;

    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}
//模拟SPI初始化
void M_SPI_st7735s_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//cs，片选
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//w25q32的片选，拉高防止误触
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_2, 1);

    GPIO_InitStructure.GPIO_Pin = DCX;//命令选择，低为命令，高为数据
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, DCX);

    GPIO_InitStructure.GPIO_Pin = SCL;//CLK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
    
    GPIOA7设置为输出//SDA线
}
//硬件SPI发送一个字节，发送完成后才会释放
void SPI1_WriteByte(u8 TxData)
{   
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//非空就卡住
    SPI_I2S_SendData(SPI1, TxData);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);//忙标志
}
//列队式发送命令和数据，等待命令发送完才会释放
void SPI1_WriteByte_IT(u8 *Command_queue){
    while (*Command_queue!=0) {
        uint8_t parameter_quantity=0;
        GPIO_ResetBits(GPIOA,CSX|DCX);//拉低片选和选择命令
        SPI_I2S_SendData(SPI1,*Command_queue);
        if ((*Command_queue&0xFE) == 0x2A){//说明是设置行和列地址
            parameter_quantity = 4;
        }
        else if ((*Command_queue)==0x2C) {//说明是填充内存
            Command_queue++;
            parameter_quantity = *Command_queue;
        }
        Command_queue++;

        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
        shu数据
        
        while (parameter_quantity) {
            while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
            SPI_I2S_SendData(SPI1,*Command_queue);
            parameter_quantity--;
            Command_queue++;
        }
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);
    }
}
//硬件SPI发送一个字，不会等待发送结束
void SPI1_WriteWord(u16 TxData){
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//非空就卡住
    SPI_I2S_SendData(SPI1, (TxData>>8));//先发送高位
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//非空就卡住
    SPI_I2S_SendData(SPI1, TxData);
}
//一次发送多个数据，
void SPI1_WriteByte_D(u8 *TxData,u8 size){
    while (size--) {
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//发送缓冲区非空就卡住
        SPI_I2S_SendData(SPI1, *TxData);
        TxData++;
    }
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);//忙标志
}
//硬件SPI发送一个指令，会自动把片选和DCX引脚拉低，等待发送完成才会释放，函数末尾会把DCX拉高
void SPI1_Write_Command_Byte(u8 TxData){
    GPIO_ResetBits(GPIOA,CSX|DCX);//拉低片选和选择命令
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//非空就卡住
    SPI_I2S_SendData(SPI1, TxData);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);//忙标志
    shu数据
}
//发送命令，并使用中断方式发送参数
void SPI1_Auto_Write_Command_p(uint8_t cmd){
    SPI1_Write_Command_Byte(cmd);
    TXE_IT_set;
}
//发送一个八位数据
void M_SPI1_Write(u8 TxData){
    for (uint8_t i=0X80; i!=0; i=i>>1)
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);//时钟线拉低
        GPIO_WriteBit(GPIOA,GPIO_Pin_7,TxData&i);//数据线设置
        Delay_Us(10);
        GPIO_SetBits(GPIOA,GPIO_Pin_5);//时钟线升高
        Delay_Us(10);
    }
}
//发送一个指令，会自动把片选和DCX引脚拉低，函数末尾会把DCX拉高
void M_SPI1_Write_Command_Byte(u8 TxData){
    GPIO_WriteBit(GPIOA,CSX|DCX,0);//拉低片选和选择命令
    GPIOA7设置为输出
    for (uint8_t i=0X80; i!=0; i=i>>1)
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);//时钟线拉低
        GPIO_WriteBit(GPIOA,GPIO_Pin_7,TxData&i);//数据线设置
        Delay_Us(10);
        GPIO_SetBits(GPIOA,GPIO_Pin_5);//时钟线升高
        Delay_Us(10);
    }
    shu数据
}
//接收一个参数字节，会自动将GPIOA7设置为输入，且函数末尾不会恢复
uint8_t M_SPI1_Receive_Parameters_Byte(void){
    GPIO_ResetBits(GPIOA,CSX);//拉低片选
    GPIOA7设置为输入
    uint8_t Data=0;
    for(int8_t i = 7; i != -1; i--)
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);//拉低时钟线
        Delay_Us(10);
        GPIO_SetBits(GPIOA,GPIO_Pin_5);//时钟线拉高
        Data+=(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))<<i;
        Delay_Us(10);
    }
    return Data;
}
void 虚拟时钟(void){
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);//拉低时钟线
        Delay_Us(10);
        GPIO_SetBits(GPIOA,GPIO_Pin_5);//时钟线拉高
        Delay_Us(10);
}
//发送多个字节，需要指定字节大小
void M_SPI1_Write_D(u8 *TxData,u8 size){
    GPIO_ResetBits(GPIOA,GPIO_Pin_1);//拉低片选
    while (size) {
        for (uint8_t i=0X80; i!=0; i=i>>1)
        {
            GPIO_ResetBits(GPIOA,GPIO_Pin_5);//时钟线拉低
            GPIO_WriteBit(GPIOA,GPIO_Pin_7,*TxData&i);//数据线设置
            Delay_Us(10);
            GPIO_SetBits(GPIOA,GPIO_Pin_5);//时钟线升高
            Delay_Us(10);
        }
        size--;
        TxData++;
    }
    
    GPIO_SetBits(GPIOA,GPIO_Pin_1);//拉高片选
}
u8 SPI1_ReadByte(void)
{   
    // GPIO_WriteBit(GPIOA,GPIO_Pin_1,0);
    jie接收模式
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//空就卡住
    fa发送模式
    uint8_t b=SPI_I2S_ReceiveData(SPI1);;
    // GPIO_WriteBit(GPIOA,GPIO_Pin_1,1);
    return b;
}