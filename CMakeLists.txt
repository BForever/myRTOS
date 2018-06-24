cmake_minimum_required(VERSION 3.9)
project(FRTOS)

set(CMAKE_CXX_STANDARD 11)
ADD_DEFINITIONS(-DSTM32F10X_MD -DSYSCLK_FREQ_72MHz -DUSE_STDPERIPH_DRIVER)
include_directories(Libraries/CMSIS/CM3/CoreSupport)
include_directories(Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x)
include_directories(Libraries/STM32F10x_StdPeriph_Driver/inc)
include_directories(src/include)
add_executable(cmcup
        src/main.c
        src/stm32f10x_it.c
        src/system.c
        src/startup.c
        src/init.c
        src/sched.c
        src/utils.c
        src/sem.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c
        Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c
        Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
        )
