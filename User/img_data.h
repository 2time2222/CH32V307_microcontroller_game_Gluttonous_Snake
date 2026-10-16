#ifndef DATA
#define DATA
#include "st7735s_driver.h"
#include "stdio.h"
//字模数据
extern const uint8_t tan_chi_she[462];
extern const uint8_t kai_shi[538];
extern const uint8_t shi_bai[157];
//会用到的函数
uint16_t *end_p(uint8_t compressed_data,uint16_t *target);
uint32_t jy(uint8_t compressed_data);
void draw(const uint8_t *s,uint16_t size);
#endif