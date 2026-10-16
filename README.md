# CH32V307_microcontroller_game_Gluttonous_Snake

[中文版](https://github.com/2time2222/CH32V307_microcontroller_game_Gluttonous_Snake/blob/main/README.md)
[English](https://github.com/2time2222/CH32V307_microcontroller_game_Gluttonous_Snake/blob/main/English.md)

基于 CH32V307 (RISC-V) 与 ST7735S TFT LCD 的贪吃蛇游戏，开发中 (WIP)。
当你连接好了硬件，上电后应该会显示“按上键开始游戏”

# 硬件连接

| 微控制器 | ST7735s LCD 模块 |
| --- | ---   |
| PA1 |  CSX  |
| PA3 |  DCX  |
| PA5 |  SLC  |
| PA7 |  SDA  |

微控制器的GPIOA_0/2/4/6 分别对应游戏逻辑的上键/下键/左键/右键，被设置为内部下拉，因此按钮应该连接到VCC

| 单片机 |  |
| --- | --- |
| PA0,PA2,PA4,PA6 |  按钮 |
