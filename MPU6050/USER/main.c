#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "OLED.h"
#include "spi.h"
#include "24l01.h"

#define N 2								//中值滤波次数

//ALIENTEK 探索者STM32F407开发板 实验32
//MPU6050六轴传感器 实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
float filter(float value_buf[]);
int main(void)
{ 
	u8 t=0;			
	u8 key = 0;
	u8 tmp_buf[33];	
	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
	short temperature;	
	short i;
	short stay_flag = 0;
	short speed_flag = 0;
	short temp;	
	float roll_buf[N+1];				//roll滤波前数组
	float pitch_buf[N+1];				//pitch滤波前数组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化
	MPU_Init();					//初始化MPU6050
	KEY_Init();       //初始化与按键连接的硬件接口
//	OLED_Init();
	NRF24L01_Init();    		//初始化NRF24L01 
	
	
 	POINT_COLOR=BLUE;//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"MPU6050 OLED NRF04L01 TEST");	
	LCD_ShowString(30,90,200,16,16,"@AngryBeast");
	LCD_ShowString(30,110,200,16,16,"2019/7/24");
	while(mpu_dmp_init())
	{
		LCD_ShowString(30,130,200,16,16,"MPU6050 Error");
//		OLED_P8x16Str(0,0,"MPU6050 Error");
		delay_ms(1000);
//		OLED_CLS();
	}
	while(NRF24L01_Check())
	{
		LCD_ShowString(30,150,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}
	
//	OLED_CLS();
	LCD_ShowString(30,130,200,16,16,"MPU6050 OK");
	LCD_ShowString(30,150,200,16,16,"NRF24L01 OK");
 	LCD_ShowString(30,230,200,16,16," Temp:    . C");	
 	LCD_ShowString(30,250,200,16,16,"Pitch:    . C");	
 	LCD_ShowString(30,270,200,16,16," Roll:    . C");	 
 	LCD_ShowString(30,290,200,16,16," Yaw :    . C");	 
	
	LCD_ShowString(30,400,200,16,16,"speed flag:");
	
 	while(1)
	{
		
		key=KEY_Scan(0);		//得到键值
		
		for(i = 0; i < N; i++)
		{
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
			{ 
				temperature = MPU_Get_Temperature();	//得到温度值
				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
				MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据		
				roll_buf[i] = roll;
				pitch_buf[i] = pitch;
			}
		}
		LCD_ShowNum(30+96,400,speed_flag,1,16);
				//pitch
				if(roll<0)
				{
					LCD_ShowChar(30+48,310,'-',16,0);		//显示负号
//					roll=-roll;		//转为正数
				}
				else 
					LCD_ShowChar(30+48,310,' ',16,0);		//去掉负号 
				LCD_ShowString(30,310,200,16,16,"N_ROLL=");
				LCD_ShowNum(30+48+8,310,roll*10/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,310,(short)roll*10%10,1,16);		//显示小数部分 
		
		
//		roll = filter(roll_buf);			//滤波以后
//		pitch = filter(pitch_buf);
		
		

//			if((t%10)==0)
//			{
//				OLED_Show_dating(roll,pitch,yaw,temperature);//OLED显示
//			}

		
//		LCD_ShowString(30,170,200,16,16,"NRF24L01 TX_Mode");			//发送数据
				
				
		if (key)
		{
			switch(key)
			{
				case KEY0_PRES:			//当按键0按下，为静止模式
				{
					stay_flag = !stay_flag;		//静止标志
					if (stay_flag)
					{
						LCD_ShowString(30,380,200,16,16,"stay mood");
					}
					else
						LCD_ShowString(30,380,200,16,16,"         ");
					break;
				}
				
				
				case KEY2_PRES:
				{
					speed_flag = !speed_flag;
					break;
				}
				
			}
		}
		NRF24L01_TX_Mode();		   				 
		if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
		{
//			LCD_ShowString(30,190,239,32,16,"Sended DATA:");	
//			LCD_ShowString(0,210,lcddev.width-1,32,16,tmp_buf); 
			
//			if(roll<0)
//				tmp_buf[0] = '-';
//			else
//				tmp_buf[0] = ' ';
//			tmp_buf[1] = (char)roll;
//			tmp_buf[2] = (char)(roll*10)%10;
//			if(pitch < 0)
//				tmp_buf[3] = '-';
//			else
//				tmp_buf[3] = ' ';
//			tmp_buf[4] = (char)pitch;
//			tmp_buf[5] = (char)(pitch*10)%10;
//			tmp_buf[6] = (char) temperature;	
//			tmp_buf[7] = (char)(temperature*10)%10;
			
//			//超过char上下限
//			if(roll < -128)
//				roll = -127;
//			if(roll > 128)
//				roll = 127;
//			if(pitch > 128)
//				pitch = 127;
//			if(pitch < -128)
//				pitch = -127;

			if (stay_flag)				//停车模式
			{
				roll = 0;
				pitch = 0;
			}
			
			
			
			if (roll < 0)					//将数据存入寄存器中准备发送
			{
				tmp_buf[0] = 0;
				tmp_buf[1] = -roll;
			}
			else
			{				
				tmp_buf[0] = 1;
				tmp_buf[1] = roll;
			}
			
			if(pitch < 0)
			{
				tmp_buf[2] = 0;
				tmp_buf[3] = -pitch;
			}
			else
			{
				tmp_buf[2] = 1;
				tmp_buf[3] = pitch;
			}
			
			if(yaw < 0)
			{
				tmp_buf[4] = 0;
				tmp_buf[5] = -yaw;
			}
			else
			{
				tmp_buf[4] = 1;
				tmp_buf[5] = yaw;
			}
			
			if(temperature < 0)
			{
				tmp_buf[6] = 0;
				tmp_buf[7] = -temperature/100;
			}
			else
			{
				tmp_buf[6] = 1;
				tmp_buf[7] = temperature/100;
			}
			tmp_buf[8] = speed_flag;
			tmp_buf[32]=0;//加入结束符		   
		}else
		{										   	
			LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//清空显示			   
//			LCD_ShowString(30,190,lcddev.width-1,32,16,"Send Failed "); 
//			OLED_CLS();
//			OLED_P8x16Str(0,8,"Send Failed");
		};
		
	
		
		if((t%10)==0)
			{ 
				if(temp<0)
				{
					LCD_ShowChar(30+48,230,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,230,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,230,temperature/100,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,230,temperature%10,1,16);		//显示小数部分 
				temp=pitch*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,250,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,250,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,250,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,250,temp%10,1,16);		//显示小数部分 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,270,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,270,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,270,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,270,temp%10,1,16);		//显示小数部分 
				
				temp=yaw*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,290,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,290,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,290,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,290,temp%10,1,16);		//显示小数部分  
				
				t=0;
			}
				
				
		if (t % 10 == 0)
			LED0=!LED0;
		delay_ms(10);				        
		t++; 
	} 	
}
float filter(float value_buf[])							//滤波
{
	char i,j;
	float temp;
	for (j=0;j<N-1;j++)
	{
		 for (i=0;i<N-j;i++)
		 {
			if ( value_buf[i] > value_buf[i+1] )
			{
			 temp = value_buf[i];
			 value_buf[i] = value_buf[i+1]; 
			 value_buf[i+1] = temp;
			}
		 }
	}
	return value_buf[(N-1)/2];
}
