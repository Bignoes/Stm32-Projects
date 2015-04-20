/***************************************************************************************
 *	FileName					:	
 *	CopyRight					:
 *	ModuleName				:	
 *
 *	CPU							:
 *	RTOS						:
 *
 *	Create Data							:	2015/01/31
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:	
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1		v1.0								Ray												Create this file
 *	2   v2.0			2015/4/13 Ray												�����DMPģ�������ϵת��
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include "stm32f10x.h"
#include "IIC_Simulation.h"
#include "USART_Serial.h"
#include "stdio.h"
#include "MPU6050.h"
#include "math.h"

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "i2c.h"
#include "clock.h"

/**************************************************************
*	Macro Define Section
**************************************************************/
#define PI 3.14
#define FRAME_DATA_CNT 0x01
#define FRAME_SUM_LEN (4+FRAME_DATA_CNT*4) 


#define BUG_DETECT_PRINT(a,has_bug,no_bug) { if(a) \
printf("%s",has_bug); \
else \
printf("%s",no_bug);}

#define DEFAULT_MPU_HZ  (100)


/**************************************************************
*	Struct Define Section
**************************************************************/


/**************************************************************
*	Prototype Declare Section
**************************************************************/
extern void NVIC_Configuration();
extern void TIM2_Configuration();

void USART_SendByte(uint8_t ch);

void UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec);
void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,int16_t hx,int16_t hy,int16_t hz);

void DMP_Init(signed char gyro_orientation[],struct int_param_s *int_param);


/**************************************************************
*	Global Variable Declare Section
**************************************************************/
int16_t ax,ay,az,gx,gy,gz;
float angleAx,gyroGy;
extern float angle,angle_dot;


/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/
int main(void)
{	
	signed char gyro_orientation[9] = {1, 0, 0, 0,1, 0, 0, 0, 1};
	struct int_param_s int_param;
	long quat[4];
	short sensors = INV_XYZ_GYRO| INV_XYZ_ACCEL | INV_WXYZ_QUAT;
	unsigned char more;
	int16_t gyro[3], accel[3],acc_ave[3];
	unsigned long timestamp,time_pre;

	SystemInit();
	clock_conf();
	IIC_GPIO_Configuration();
	USART_GPIO_Configuration();
	USART_Configuration();
	USART_ClearFlag(USART1,USART_FLAG_TC);
	InitMPU6050();
//	Self_Revise(&AccelReviseType);				//��У����ʼ��
//	NVIC_Configuration();
	DMP_Init(gyro_orientation,&int_param);

// 	TIM2_Configuration();
	
	
	dmp_read_fifo(gyro,accel,quat,&time_pre,&sensors,&more);
	while(1)
	{
		//GetAccel();
		//MovementEndCheck();
		dmp_read_fifo(gyro,accel,quat,&timestamp,&sensors,&more);
		
		UART1_ReportMotion(accel[0],accel[1],accel[2],gyro[0],gyro[1],gyro[2],0,0,0);
		//UART1_ReportMotion((int16_t)(VelocityType.VelocityY[1] >> 14),(int16_t)(DpmType.DpmY[1] >> 14),2,0,0,0,0,0,0);
//		UART1_ReportMotion((int16_t)AccelType.AccelX[1],1,2,0,0,0,0,0,0);
//		UART1_ReportMotion(AccelType.AccelX[0],AccelType.AccelY[0],AccelType.AccelZ[0],gx,gy,gz,0,0,0);
			
	}


	return 0;
}


/**
 *  @name					void USART_SendByte(uint8_t ch)		
 *	@description	ͨ������1����һ���ֽڵ�����
 *	@param			
 *	@return		
 *  @notice
 */
void USART_SendByte(uint8_t ch)
{
		USART_SendData(USART1,ch);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET)
			;
}



/**
 *  @name						void DMP_Init()
 *	@description		MPU6050 DMPģ���ʼ������
 *	@param					gyro_orientation --	signed char []����������������ķ���
										int_param -- struct int_param_s *, DMP��ʼ���ṹ��
 *	@return		
 *  @notice
 */
void DMP_Init(signed char gyro_orientation[],struct int_param_s *int_param)
{
	   ANBT_I2C_Configuration();		//IIC��ʼ��
    //BUG_DETECT_PRINT(i2c_CheckDevice(0x68<<1),"\r\n δ��⵽MPU6050 \r\n","\r\n ��⵽MPU6050 \r\n");

    //BUG_DETECT_PRINT((result = DMP_MPU6050_DEV_CFG()),"\r\n MPU6050ʧ��\r\n","\r\n MPU6050 \r\n");

    BUG_DETECT_PRINT(mpu_init(int_param),"\r\n MPU6050��ʼ��ʧ��\r\n","\r\n MPU6050��ʼ���ɹ�\r\n");

    BUG_DETECT_PRINT(mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL),"\r\n DMP���ô�����ʧ��\r\n","\r\n DMP���ô������ɹ�\r\n");

    /* Push both gyro and accel data into the FIFO. */
    BUG_DETECT_PRINT(mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL),"\r\n ����FIFOʧ��\r\n","\r\n ����FIFO�ɹ�\r\n");

    BUG_DETECT_PRINT(mpu_set_sample_rate(DEFAULT_MPU_HZ),"\r\n ���ò�����ʧ��\r\n","\r\n ���ò����ʳɹ�\r\n");

    BUG_DETECT_PRINT(dmp_load_motion_driver_firmware(),"\r\n ���ع̼�ʧ��\r\n","\r\n ���سɹ�\r\n");

    BUG_DETECT_PRINT(dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)),"\r\n DMP���ó�ʼ����ʧ��\r\n","\r\n DMP���ó�ʼ����ɹ�\r\n");

    BUG_DETECT_PRINT(dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT  | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL),"\r\n DMP��ʼ������ʧ��\r\n","\r\n DMP��ʼ�����Գɹ�\r\n");

    BUG_DETECT_PRINT(dmp_set_fifo_rate(DEFAULT_MPU_HZ),"\r\n ����FIFO�������ʧ��\r\n","\r\n ����FIFO������ʳɹ�\r\n");

    run_self_test();

    mpu_set_dmp_state(1);
}




int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET)	
		;
	
	return ch;
}




/**************************ʵ�ֺ���********************************************
*����ԭ��:		void UART1_Put_Char(unsigned char DataToSend)
*��������:		RS232����һ���ֽ�
���������
		unsigned char DataToSend   Ҫ���͵��ֽ�����
���������û��	
*******************************************************************************/
void UART1_Put_Char(unsigned char DataToSend)
{
	//��Ҫ���͵��ֽ�д��UART1�ķ��ͻ�����
	USART_SendData(USART1, (unsigned char) DataToSend);
	//�ȴ��������
  	while (!(USART1->SR & USART_FLAG_TXE));
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll
				,int16_t alt,int16_t tempr,int16_t press)
*��������:		����λ�����;�����������̬����
���������
		int16_t yaw ���������ĺ���Ƕȡ���λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
		int16_t pitch ����õ��ĸ����Ƕȣ���λ 0.1�ȡ�-900 - 900 ��Ӧ -90.0 -> 90.0 ��
		int16_t roll  �����õ��ĺ���Ƕȣ���λ0.1�ȡ� -1800 -> 1800 ��Ӧ -180.0  ->  180.0��
		int16_t alt   ��ѹ�߶ȡ� ��λ0.1�ס�  ��Χһ�����ͱ���
		int16_t tempr �¶� �� ��λ0.1���϶�   ��Χ��ֱ����ĵ�·�岻����������
		int16_t press ��ѹѹ������λ10Pa  һ������ѹǿ��101300pa ����Ѿ�����һ�����͵ķ�Χ����Ҫ����10�ٷ�����λ��
		int16_t IMUpersec  ��̬�������ʡ�����IMUpersecÿ�롣
���������û��	
*******************************************************************************/
void UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll
,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec)
{
 	unsigned int temp=0xaF+2+2;
	char ctemp;
	UART1_Put_Char(0xa5);
	UART1_Put_Char(0x5a);
	UART1_Put_Char(14+2);
	UART1_Put_Char(0xA1);

	if(yaw<0)yaw=32768-yaw;
	ctemp=yaw>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=yaw;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(pitch<0)pitch=32768-pitch;
	ctemp=pitch>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=pitch;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(roll<0)roll=32768-roll;
	ctemp=roll>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=roll;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(alt<0)alt=32768-alt;
	ctemp=alt>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=alt;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(tempr<0)tempr=32768-tempr;
	ctemp=tempr>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=tempr;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(press<0)press=32768-press;
	ctemp=press>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=press;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	
	ctemp=IMUpersec>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=IMUpersec;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	
	UART1_Put_Char(temp%256);
	UART1_Put_Char(0xaa);
}




/**************************ʵ�ֺ���********************************************
*����ԭ��:		void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz)
*��������:		����λ�����͵�ǰ�����������ֵ
���������
	int16_t ax  ���ٶ� X��ADC��� ��Χ ��һ���з�������
	int16_t ay  ���ٶ� Y��ADC��� ��Χ ��һ���з�������
	int16_t az  ���ٶ� Z��ADC��� ��Χ ��һ���з�������
	int16_t gx  ������ X��ADC��� ��Χ ��һ���з�������
	int16_t gy  ������ Y��ADC��� ��Χ ��һ���з�������
	int16_t gz  ������ Z��ADC��� ��Χ ��һ���з�������
	int16_t hx  ������ X��ADC��� ��Χ ��һ���з�������
	int16_t hy  ������ Y��ADC��� ��Χ ��һ���з�������
	int16_t hz  ������ Z��ADC��� ��Χ ��һ���з�������
	
���������û��	
*******************************************************************************/
void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz)
{
 	unsigned int temp=0xaF+9;
	char ctemp;
	UART1_Put_Char(0xa5);
	UART1_Put_Char(0x5a);
	UART1_Put_Char(14+8);
	UART1_Put_Char(0xA2);

	if(ax<0)ax=32768-ax;
	ctemp=ax>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=ax;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(ay<0)ay=32768-ay;
	ctemp=ay>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=ay;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(az<0)az=32768-az;
	ctemp=az>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=az;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(gx<0)gx=32768-gx;
	ctemp=gx>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gx;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(gy<0)gy=32768-gy;
	ctemp=gy>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gy;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
//-------------------------
	if(gz<0)gz=32768-gz;
	ctemp=gz>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gz;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(hx<0)hx=32768-hx;
	ctemp=hx>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hx;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(hy<0)hy=32768-hy;
	ctemp=hy>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hy;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(hz<0)hz=32768-hz;
	ctemp=hz>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hz;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	UART1_Put_Char(temp%256);
	UART1_Put_Char(0xaa);
}


/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */



