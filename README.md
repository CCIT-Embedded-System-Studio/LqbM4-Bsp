# LqbM4-Bsp
蓝桥杯嵌入式M4开发板（G431）板级支持包
<br/>
## 项目介绍
本项目提供了蓝桥杯M4竞赛板及扩展板大部分外设的板级驱动
<br/>
## 文件树介绍
├─Inc<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─M4Bsp.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─M4Bsp_conf.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─fonts.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─i2c.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─lcd.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─main.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─stm32g4xx_hal_conf.h<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─stm32g4xx_it.h<br/>
├─src<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─M4Bsp.c<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─i2c.c<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─lcd.c<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─main.c<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─stm32g4xx_hal_msp.c.c<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─stm32g4xx_it.c.c<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─system_stm32g4xx.c.c<br/>
└─LqbM4-Bsp.ioc<br/>
<br/>
**Bsp文件**：M4Bsp_conf.h、M4Bsp.h、M4Bsp.c，其中M4Bsp_conf.h用于配置启用某个BSP模块<br/>
**驱动文件**：i2c.h、i2c.c、lcd.h、lcd.c、fonts.h为蓝桥杯官方提供驱动文件<br/>
**其它文件**：这些文件由STM32CubeMX生成<br/>
<br/>
**当前版本**: v1.0<br/>
