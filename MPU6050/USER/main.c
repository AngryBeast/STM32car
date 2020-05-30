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

#define N 2								//��ֵ�˲�����

//ALIENTEK ̽����STM32F407������ ʵ��32
//MPU6050���ᴫ���� ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK
float filter(float value_buf[]);
int main(void)
{ 
	u8 t=0;			
	u8 key = 0;
	u8 tmp_buf[33];	
	float pitch,roll,yaw; 		//ŷ����
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;	//������ԭʼ����
	short temperature;	
	short i;
	short stay_flag = 0;
	short speed_flag = 0;
	short temp;	
	float roll_buf[N+1];				//roll�˲�ǰ����
	float pitch_buf[N+1];				//pitch�˲�ǰ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ��
	MPU_Init();					//��ʼ��MPU6050
	KEY_Init();       //��ʼ���밴�����ӵ�Ӳ���ӿ�
//	OLED_Init();
	NRF24L01_Init();    		//��ʼ��NRF24L01 
	
	
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
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
		
		key=KEY_Scan(0);		//�õ���ֵ
		
		for(i = 0; i < N; i++)
		{
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
			{ 
				temperature = MPU_Get_Temperature();	//�õ��¶�ֵ
				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
				MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������		
				roll_buf[i] = roll;
				pitch_buf[i] = pitch;
			}
		}
		LCD_ShowNum(30+96,400,speed_flag,1,16);
				//pitch
				if(roll<0)
				{
					LCD_ShowChar(30+48,310,'-',16,0);		//��ʾ����
//					roll=-roll;		//תΪ����
				}
				else 
					LCD_ShowChar(30+48,310,' ',16,0);		//ȥ������ 
				LCD_ShowString(30,310,200,16,16,"N_ROLL=");
				LCD_ShowNum(30+48+8,310,roll*10/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,310,(short)roll*10%10,1,16);		//��ʾС������ 
		
		
//		roll = filter(roll_buf);			//�˲��Ժ�
//		pitch = filter(pitch_buf);
		
		

//			if((t%10)==0)
//			{
//				OLED_Show_dating(roll,pitch,yaw,temperature);//OLED��ʾ
//			}

		
//		LCD_ShowString(30,170,200,16,16,"NRF24L01 TX_Mode");			//��������
				
				
		if (key)
		{
			switch(key)
			{
				case KEY0_PRES:			//������0���£�Ϊ��ֹģʽ
				{
					stay_flag = !stay_flag;		//��ֹ��־
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
			
//			//����char������
//			if(roll < -128)
//				roll = -127;
//			if(roll > 128)
//				roll = 127;
//			if(pitch > 128)
//				pitch = 127;
//			if(pitch < -128)
//				pitch = -127;

			if (stay_flag)				//ͣ��ģʽ
			{
				roll = 0;
				pitch = 0;
			}
			
			
			
			if (roll < 0)					//�����ݴ���Ĵ�����׼������
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
			tmp_buf[32]=0;//���������		   
		}else
		{										   	
			LCD_Fill(0,170,lcddev.width,170+16*3,WHITE);//�����ʾ			   
//			LCD_ShowString(30,190,lcddev.width-1,32,16,"Send Failed "); 
//			OLED_CLS();
//			OLED_P8x16Str(0,8,"Send Failed");
		};
		
	
		
		if((t%10)==0)
			{ 
				if(temp<0)
				{
					LCD_ShowChar(30+48,230,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,230,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,230,temperature/100,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,230,temperature%10,1,16);		//��ʾС������ 
				temp=pitch*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,250,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,250,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,250,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,250,temp%10,1,16);		//��ʾС������ 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,270,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,270,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,270,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,270,temp%10,1,16);		//��ʾС������ 
				
				temp=yaw*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,290,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,290,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,290,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,290,temp%10,1,16);		//��ʾС������  
				
				t=0;
			}
				
				
		if (t % 10 == 0)
			LED0=!LED0;
		delay_ms(10);				        
		t++; 
	} 	
}
float filter(float value_buf[])							//�˲�
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
