#include "timer.h"
#include "led.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTE精英STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

extern u16 motor1,motor2,motor3,motor4;//控制步进电机
extern u16 mg1,mg2,mg3,mg4;//控制最上方的舵机
u16 num;
u8 flag;
u8 L_flag,R_flag,P_flag,F_flag,G_flag;//左手 右手 放下 脱机 读取rfid
extern u16 usart1_len,usart2_len;//串口数据长度
void TIM4_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
							 
}
//定时器4中断服务程序
void TIM4_IRQHandler(void)   //TIM4中断
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		}
		if(motor1){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_6)){
				
				GPIO_ResetBits(GPIOB,GPIO_Pin_6);
				motor1--;
			}
			else {
			GPIO_SetBits(GPIOB,GPIO_Pin_6);
			
			
			}
		}
		
		if(motor2){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_7)){
				
				GPIO_ResetBits(GPIOB,GPIO_Pin_7);
				motor2--;
			}
			else {
			GPIO_SetBits(GPIOB,GPIO_Pin_7);
			
			
			}
		}
		if(motor3){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8)){
				
				GPIO_ResetBits(GPIOB,GPIO_Pin_8);
				motor3--;
			}
			else {
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			
			
			}
		}
		
		if(motor4){
			if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9)){
				
				GPIO_ResetBits(GPIOB,GPIO_Pin_9);
				motor4--;
			}
			else {
			GPIO_SetBits(GPIOB,GPIO_Pin_9);
			
			
			}
		}
		
		




}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5   TIM3_CH1->PB4 TIM3_CH3->PB0 TIM3_CH4->PB1           
 
   //设置该引脚为复用输出功能,输出TIM3 CH1的PWM脉冲波形	GPIOB.4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	
	
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
    //设置该引脚为复用输出功能,输出TIM3 CH3的PWM脉冲波形	GPIOB.0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC3

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR3上的预装载寄存器
	
	  //设置该引脚为复用输出功能,输出TIM3 CH4的PWM脉冲波形	GPIOB.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR4上的预装载寄存器
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	

}


void get_motor(void)
{
	if(USART1_RX_STA&0x8000)//是否有接收到东西
		{
		
			usart1_len = USART1_RX_STA&0x3fff;//得到此次接收到的数据长度	
			switch (USART1_RX_BUF[0]) {
				case 'L':
					if(USART1_RX_BUF[1]=='q'){
						L_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='L'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'L');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'R': 
					if(USART1_RX_BUF[1]=='q'){
						R_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='R'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'R');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'P': 
					if(USART1_RX_BUF[1]=='q'){
						P_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='P'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'P');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'F':
					if(USART1_RX_BUF[1]=='q'){
						F_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='F'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'F');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 'G':
					if(USART1_RX_BUF[1]=='q'){
						G_flag=1;
						while(USART1_RX_STA&0x8000&&USART1_RX_BUF[0]=='G'&&USART1_RX_BUF[1]=='q'){		
						USART_SendData(USART1, 'G');
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//是否发送完成
						USART_SendData(USART1, 'r');
						USART1_RX_STA=0;
						}
					}
					break;
				case 't'://步进电机
					switch(USART1_RX_BUF[1]){
						case 1:
							motor1=USART1_RX_BUF[2];
							break;
						case 2:
							motor2=USART1_RX_BUF[2];
							break;
						case 3:
							motor3=USART1_RX_BUF[2];
							break;
						case 4:
							motor4=USART1_RX_BUF[2];
							break;
						default:
							break;
					}
					case 'g':
						switch(USART1_RX_BUF[1]){
						case 1:
							mg1=USART1_RX_BUF[2];
							break;
						case 2:
							mg2=USART1_RX_BUF[2];
							break;
						case 3:
							mg3=USART1_RX_BUF[2];
							break;
						case 4:
							mg4=USART1_RX_BUF[2];
							break;
						default:
							break;
					}
				}
			
		}
		
			/*
			if(F_flag==2){//脱机上升
			motor1=num1;
			motor2=num2;
			motor3=num3;
			motor4=num4;
		}
			else {//停止上升
			motor1=0;
			motor2=0;
			motor3=0;
			motor4=0;
			
			}
			*/
}
