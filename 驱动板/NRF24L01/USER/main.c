#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "spi.h"
#include "key.h" 
#include "24l01.h"
#include "OLED.h"
#include "myiic.h"
#include "pwm.h"

#define SIZE sizeof(TEXT_Buffer)	 
	

 #define LEFT 2200			//3333重装载值下舵机转动的参数
 #define MID  1900
 #define RIGHT 1600
 #define SPEED_INIT 900
void Control_Steering(int roll);
void Motor_Control(int pitch);
short speed_flag = 0;

int main(void)
{ 
	u16 t=0;			 
	u8 tmp_buf[33];	
	int roll,pitch,temperature,yaw;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	LED_Init();					//初始化LED  
	IIC_Init();
 	NRF24L01_Init();    		//初始化NRF24L01 
	TIM14_PWM_Init(3333-1,84-1);	//84M/84=1Mhz的计数频率,重装载值3333，所以PWM频率为 1M/3333.3333=300hz.  飞思卡尔S-D5舵机  
//	TIM13_PWM_Init(1000-1,84-1);	//1M/1000 = 1000HZ, T = 1ms
	TIM41_PWM_Init(1000-1,84-1);
	TIM42_PWM_Init(1000-1,84-1);
	//F9舵机   B6左 B7右
	TIM_SetCompare1(TIM14,MID);			//mid 电机归中
	printf("Init seccess\n");
	delay_ms(10);	
	while(NRF24L01_Check())
	{
		OLED_P8x16Str(0,0,"NRF24L01 Error");
		delay_ms(400);
	}
		NRF24L01_RX_Mode();		  
		while(1)
		{	  		  
			
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
			{
				tmp_buf[32]=0;//加入字符串结束符
				
				//数据分析
				if(tmp_buf[0] == 0)
					roll = -tmp_buf[1];
				else
					roll = tmp_buf[1];
				
				if(tmp_buf[2] == 0)
					pitch	= -tmp_buf[3];
				else
					pitch = tmp_buf[3];
				
				if(tmp_buf[4] == 0)
					yaw = -tmp_buf[5];
				else
					yaw = tmp_buf[5];
				
				if(tmp_buf[6] == 0)
					temperature = -tmp_buf[7];
				else
					temperature = tmp_buf[7];
					
				speed_flag = tmp_buf[8];
				delay_ms(10);
				
				
				
				printf("roll = %d,pitch = %d \n\n",roll,pitch);
				
				Control_Steering(roll);
			
				Motor_Control(pitch);
				delay_ms(10);
				
				
			}else delay_us(100);	 
			t++; 
		};	
	
		
}


void Motor_Control(int pitch)
{
		int speed;
//		speed = SPEED_INIT;
		
		if (pitch > -3&&pitch <3)
		{
			TIM_SetCompare1(TIM4,0);
			TIM_SetCompare2(TIM4,0);
			return;
		}
		if (speed_flag)
		{
			speed = 130;
		}
		else
			speed = 80;
		if (pitch < 0)
			speed = speed - pitch;
		if (pitch > 0)
			speed = speed + pitch;
		if (pitch < -3)
		{ 
			TIM_SetCompare1(TIM4,speed);
			TIM_SetCompare2(TIM4,0);
		}
		if (pitch > 3)
		{
			TIM_SetCompare2(TIM4,speed);
			TIM_SetCompare1(TIM4,0);
		}
		delay_ms(10);
		
}



void Control_Steering(int roll)
{
	TIM_SetCompare1(TIM14,MID+ 5*roll);
	delay_ms(10);
}
