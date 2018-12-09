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
 ALIENTEKս��STM32������ʵ��33
 ����ͨ�� ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

u8 tmp_buf[33] = {48,49,50,51,52,53,54,55,56,57};	
u8 Result;
u8 Mode;

 int main(void)
 {	 
	
	u16 t=0;			 

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	TIM_TimeBaseInitStruct.TIM_Prescaler = (9-1);				//72MHZ/9���ڶ�ʱ��������1���Ӽ����Ĵ�����Ҳ����8MHZ����ÿ����һ��ʱ��Ϊ0.000125ms
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TIM4_PERIOD;			//����10000��,��Ӧʱ����1ms
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; 	///����ʱ�ӷָ�:TDTS = Tck_tim

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	delay_init();	    										//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;				//���ó�ʼ������TIM4���ж�	
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;		//������ռ���ȼ�Ϊ2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;				//������Ӧ���ȼ��������ȼ���Ϊ0
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;				//ʹ�ܶ�ʱ��4����ж�
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);					//ʹ��TIM4��TIM_IT_Update�ж�
	TIM_Cmd(TIM4, ENABLE);										//ʹ�ܶ�ʱ������ʼ����
	
	uart_init(115200);	 										//���ڳ�ʼ��Ϊ115200
 	LED_Init();		  											//��ʼ����LED���ӵ�Ӳ���ӿ�
//	KEY_Init();													//��ʼ������
//	LCD_Init();			   										//��ʼ��LCD  
 	NRF24L01_Init();    										//��ʼ��NRF24L01 

	while( NRF24L01_Check() );
	NRF24L01_RX_Mode();
	
/*	switch(Result)
	{
		case MAX_TX: GPIO_ResetBits(GPIOE,GPIO_Pin_5);break; 					 //PE.5 ����� 
		case TX_OK:  GPIO_ResetBits(GPIOB,GPIO_Pin_5);break;					 //PB.5 ����� 
		default :
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_5);
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
		}
	}*/
	while(1)
	{
		if(Mode == Tx_mode_Flag)
		{
			NRF24L01_TX_Mode();
			Result = NRF24L01_TxPacket(tmp_buf);
			NRF24L01_RX_Mode();
			Mode=Rx_mode_Flag;
		}
		
		NRF24L01_RxPacket(tmp_buf);
	}
}	

void TIM4_IRQHandler()
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) == 1)
	{
		NRF24L01_PowerDown_Mode();
		Mode = Tx_mode_Flag;
		TIM_ClearFlag(TIM4,TIM_IT_Update);
	}
}


