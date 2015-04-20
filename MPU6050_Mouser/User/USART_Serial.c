/***************************************************************************************
 *	FileName					:		USART_Serial.c			
 *	CopyRight					:
 *	ModuleName				:		Ray
 *
 *	CPU							:			STM32F103ZE
 *	RTOS						:
 *
 *	Create Data					:	2015/03/11
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:	
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
# include "USART_Serial.h"

/**************************************************************
*	Macro Define Section
**************************************************************/


/**************************************************************
*	Struct Define Section
**************************************************************/


/**************************************************************
*	Prototype Declare Section
**************************************************************/


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
 *  @name					void USART_GPIO_Configuration()
 *	@description	USART��GPIO�����ã��ɺ궨��TXD,RXD,USART_GPIO_Groupȷ����
 *	@param			
 *	@return		
 *  @notice				STM32��USART��ӳ���GPIO���ƺ��ǹ̶���,������ˣ���궨�岻������
 */
void USART_GPIO_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(USART_APB_Periph,ENABLE);
	/* ����USART��TXD��Ϊ�ڶ������������ */
	GPIO_InitStructure.GPIO_Pin = TXD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_GPIO_GROUP,&GPIO_InitStructure);
	
	/* ����USART��RXD��Ϊ��������� */\
	GPIO_InitStructure.GPIO_Pin = RXD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_GPIO_GROUP,&GPIO_InitStructure);
}


/**
 *  @name					void USART_Configuration()
 *	@description	USART�����ã�������9600
 *	@param			
 *	@return		
 *  @notice 
 */
void USART_Configuration()
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}
/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */
