/***************************************************************************************
 *	FileName					:		Ray		
 *	CopyRight					:
 *	ModuleName				:		IIC_Simulation.c
 *
 *	CPU							:			STM32F103ZE
 *	RTOS						:
 *
 *	Create Data					:	2015/03/11
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:		ģ��IIC
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1		v1.0			2015/3/11	Ray												Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
# include "IIC_Simulation.h"

/**************************************************************
*	Macro Define Section
**************************************************************/
#define SlaveAddress	0xD0

/**************************************************************
*	Struct Define Section
**************************************************************/


/**************************************************************
*	Prototype Declare Section
**************************************************************/
static void delay_us(u16 time);
static void IIC_Start();
static void IIC_Stop();
static Status IIC_RecvACK();
static void IIC_SendByte(uint8_t dat);
static uint8_t IIC_RecvByte();

/**************************************************************
*	Global Variable Declare Section
**************************************************************/


/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/

/**
 *  @name						void IIC_Start();
 *	@description		����IIC�Ŀ�ʼ�ź�
 *	@param			
 *	@return		
 *  @notice
 */
static void IIC_Start()
{
	GPIO_SetBits(IIC_GPIO_Group,SDA);
	GPIO_SetBits(IIC_GPIO_Group,SCL);
	delay_us(5);
	GPIO_ResetBits(IIC_GPIO_Group,SDA);	//��SCLΪ�ߵ�ƽ�ڼ䣬��SDA��1������0��Ϊ��ʼ�ź� ���½��أ�
	delay_us(5);
	GPIO_ResetBits(IIC_GPIO_Group,SCL);	//�������ʱ���ߣ�����SDA�߱仯�����ں����send��receive����
					
	return;
}

/**
 *  @name					void IIC_Stop()
 *	@description	����IIC��ֹͣ�ź�
 *	@param			
 *	@return		
 *  @notice
 */
static void IIC_Stop()
{
	GPIO_ResetBits(IIC_GPIO_Group,SDA);
	GPIO_SetBits(IIC_GPIO_Group,SCL);
	delay_us(5);	
	GPIO_SetBits(IIC_GPIO_Group,SDA);		//��SCLΪ�ߵ�ƽ�ڼ䣬��SDA��0������1Ϊֹͣ�ź�	�������أ�
	delay_us(5);
}

/**
 *  @name					Status IIC_RecvACK();
 *	@description	�ȴ�Ӧ���ź�
 *	@param			
 *	@return		
 *  @notice
 */
static Status IIC_RecvACK()
{
	uint16_t i;
	
	GPIO_SetBits(IIC_GPIO_Group,SCL);
	delay_us(5);
	while(GPIO_ReadInputDataBit(IIC_GPIO_Group,SDA)==1 && i<0X2B0)		//�ȴ�Ӧ��
		i++;
	i = GPIO_ReadInputDataBit(IIC_GPIO_Group,SDA);
	GPIO_ResetBits(IIC_GPIO_Group,SCL);
	delay_us(5);	
	
	return (i?FALSE:TRUE);
}


/**
 *  @name					void IIC_SendByte(uint8_t dat)
 *	@description	ͨ��IIC����һ���ֽڵ�����
 *	@param			
 *	@return		
 *  @notice
 */
static void IIC_SendByte(uint8_t dat)
{
	uint8_t i,tmp;
	
	GPIO_ResetBits(IIC_GPIO_Group,SCL);
	for(i=0; i<8; i++)
	{
		tmp = dat&0x80;
		if(tmp)
			GPIO_SetBits(IIC_GPIO_Group,SDA);
		else
			GPIO_ResetBits(IIC_GPIO_Group,SDA);
		GPIO_SetBits(IIC_GPIO_Group,SCL);							//����ʱ����
		delay_us(5);
		GPIO_ResetBits(IIC_GPIO_Group,SCL);						//����ʱ���ߣ������������ݱ仯
		delay_us(5);
		dat <<= 1;
	}	
	
	IIC_RecvACK();
}

/**
 *  @name						static uint8_t IIC_RecvByte()
 *	@description		��IIC�����н���һ���ֽڵ�����
 *	@param			
 *	@return		
 *  @notice
 */
static uint8_t IIC_RecvByte()
{
	uint8_t i,dat = 0;
	
	GPIO_ResetBits(IIC_GPIO_Group,SCL);
	delay_us(5);
	GPIO_SetBits(IIC_GPIO_Group,SDA);
	delay_us(5);
	for(i=0; i<8; i++)
	{
		GPIO_SetBits(IIC_GPIO_Group,SCL);
		delay_us(5);
		dat = (dat<<1) | (uint8_t)GPIO_ReadInputDataBit(IIC_GPIO_Group,SDA);
		delay_us(5);
		GPIO_ResetBits(IIC_GPIO_Group,SCL);
		delay_us(5);
	}
	
	return dat;
}

/**
 *  @name					void Single_WriteIIC(uint8_t REG_Address,uint8_t REG_data);			
 *	@description	��IIC�豸��ָ����ַ��д��һ���ֽڵ�����	
 *	@param				REG_Address		-- uint8_t,Ҫд���(�Ĵ���)��ַ
									REG_data			-- uint8_t,Ҫд�������
 *	@return		
 *  @notice
 */
void Single_WriteIIC(uint8_t REG_Address,uint8_t REG_data)
{
	IIC_Start();
	IIC_SendByte(SlaveAddress);
	IIC_SendByte(REG_Address);
	IIC_SendByte(REG_data);	
	IIC_Stop();
}

/**
 *  @name					uint8_t Single_ReadIIC(uint8_t REG_Address)				
 *	@description	��IIC�豸��ָ����ַ�ж�ȡһ������
 *	@param				REG_Address		-- uint8_t,Ҫд���(�Ĵ���)��ַ
 *	@return				uint8_t		--  ��IIC�豸�н��յ�����
 *  @notice
 */
uint8_t Single_ReadIIC(uint8_t REG_Address)
{
	uint8_t REG_data;

	IIC_Start();
	IIC_SendByte(SlaveAddress);
	IIC_SendByte(REG_Address);
	IIC_Start();
	IIC_SendByte(SlaveAddress+1);
	REG_data = IIC_RecvByte();
	IIC_Stop();
	
	return REG_data;
}


/**
 *  @name						void IIC_GPIO_Configuration()
 *	@description		GPIO�����ã�SCL(GPIOB.6),SDA(GPIOB.7)��©���������ģ��IIC
 *	@param			
 *	@return		
 *  @notice					�궨��SCL,SDA,IIC_GPIO_Group,IIC_APB_Periph��ͷ�ļ��ж���
 */
void IIC_GPIO_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(IIC_APB_Periph,ENABLE);	
	GPIO_InitStructure.GPIO_Pin = SCL | SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_GPIO_Group,&GPIO_InitStructure);

	return;
}

/**
 *  @name						void delay_nus(u16 time)			
 *	@description		΢�뼶�ӳٺ���
 *	@param			
 *	@return		
 *  @notice
 */
static void delay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  	
      while(i--) ;    
   }
}
/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */
