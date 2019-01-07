#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24l01.h" 	 
 

#define TIM4_PERIOD 8000
#define Tx_mode_Flag 0
#define Rx_mode_Flag 0xff
/************************************************
 ALIENTEK战舰STM32开发板实验33
 无线通信 实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//u8 tmp_buf[33] = {48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80};
u8 tmp_buf[1] = {48};
u8 tmp = 0;
u8 Result;
u8 Mode = 0xff;
u16 T = 0;

 int main(void)
 {	 
	u8 t;

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	delay_init();												//延时函数初始化	 
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE);
	 
	TIM_TimeBaseInitStruct.TIM_Prescaler = (9-1);				//72MHZ/9等于定时器计数器1秒钟计数的次数，也就是8MHZ，那每计数一次时间为0.000125ms
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TIM4_PERIOD;			//计数8000次,对应时间是1ms
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; 	//设置时钟分割:TDTS = Tck_tim

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
		    										 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;				//设置初始化的是TIM4的中断	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;		//设置抢占优先级为2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;				//设置响应优先级（子优先级）为0
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;				//使能定时器4这个中断
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);					//使能TIM4的TIM_IT_Update中断
	TIM_Cmd(TIM4, ENABLE);										//使能定时器，开始计数
	

 	NRF24L01_Init();    										//初始化NRF24L01 

	while( NRF24L01_Check() );
	NRF24L01_RX_Mode();

	while(1)
	{
		if(Mode == Tx_mode_Flag)
		{
			NRF24L01_PowerDown_Mode();
			NRF24L01_TX_Mode();
			Result = NRF24L01_TxPacket(tmp_buf);
			NRF24L01_PowerDown_Mode();
			NRF24L01_RX_Mode();
			Mode=Rx_mode_Flag;
		}	
		t = NRF24L01_RxPacket(tmp_buf);
			
	}
}	

void TIM4_IRQHandler()
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == 1)
	{
		T++;
		if(T == 50)
		{
			Mode = Tx_mode_Flag;
			T = 0;
		}

		TIM_ClearFlag(TIM4,TIM_IT_Update);
	}
}


