//
// Created by 范宏昌 on 2018/6/3.
//
#include "stm32f10x_conf.h"
#include "init.h"
#include "utils.h"

void USART1_configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);
	
	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
}

void RCC_Configuration(void)
{
	/* Enable USART1, GPIOA, GPIOx and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = PendSV_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; //使能外部中断所在的通道
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级 2，
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02; //子优先级 2
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
//	NVIC_Init(&NVIC_InitStruct); //根据结构体信息进行优先级初始化
//
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn; //使能外部中断所在的通道
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级 2，
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02; //子优先级 2
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
//	NVIC_Init(&NVIC_InitStruct); //根据结构体信息进行优先级初始化
	
}

void TIM_Configuration(void)
{
	
	TIM_DeInit(TIM2);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitstruct;
	TIM_TimeBaseInitstruct.TIM_Prescaler = 15400 - 1;//0.2 ms
	TIM_TimeBaseInitstruct.TIM_Period = 5000 - 1;//0.2ms * 5000 = 1 s
	TIM_TimeBaseInitstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitstruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitstruct.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitstruct);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
	
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          //USART1 TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);      //A端口
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;          //USART1 RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //复用开漏输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void EXIT_Configuration(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	EXTI_InitTypeDef EXTI_InitStructure;//定义初始化结构体
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; //中断线的标号 取值范围为EXTI_Line0~EXTI_Line15
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式，可选值为中断 EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//触发方式，可以是下降沿触发 EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4; //中断线的标号 取值范围为EXTI_Line0~EXTI_Line15
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式，可选值为中断 EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//触发方式，可以是下降沿触发 EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_InitStructure);//根据结构体信息进行初始化
}

void delay_debounce()
{
	uint32_t t = 0x000FFFFF;
	while (t--);
}