# CH32V307_microcontroller_game_Gluttonous_Snake

[中文版](https://github.com/2time2222/CH32V307_microcontroller_game_Gluttonous_Snake/blob/main/README.md)
[English](https://github.com/2time2222/CH32V307_microcontroller_game_Gluttonous_Snake/blob/main/English.md)

A Snake game based on the CH32V307 (RISC-V) and ST7735S TFT LCD. Work in progress (WIP).
Once the hardware is connected and powered on, it should display “Press the Up button to start the game”.
![start game](https://github.com/2time2222/CH32V307_microcontroller_game_Gluttonous_Snake/blob/main/img/start_game.png)  

The Chinese text in the picture translates to "Start the game".

# Hardware Connections

| Microcontroller | ST7735S LCD Module |
| --- | --- |
| PA1 | CSX |
| PA3 | DCX |
| PA5 | SLC |
| PA7 | SDA |

GPIOA_0/2/4/6 on the microcontroller correspond to the Up/Down/Left/Right buttons in the game logic, respectively. They are configured as internal pull-downs, so the buttons should be connected to VCC.

| MCU |  |
| --- | --- |
| PA0, PA2, PA4, PA6 | Buttons |
