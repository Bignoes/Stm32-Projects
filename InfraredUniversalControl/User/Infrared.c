/***************************************************************************************
 *	FileName					:		Infrared.c
 *	CopyRight					:
 *	ModuleName				:	
 *
 *	CPU							:
 *	RTOS						:
 *
 *	Create Data					:	2015/04/21
 *	Author/Corportation			:	Ray
 *
 *	Abstract Description		:	
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1		v1.0			2015/4/21	Ray												Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include "Infrared.h"


/**************************************************************
*	Macro Define Section
**************************************************************/

//debug���Ժ궨�壬���ݱ��ʽa�����ִ��has_bug��no_bug 
#define BUG_DETECT_PRINT(a,has_bug,no_bug) { if(a) \
printf("%s",has_bug); \
else \
printf("%s",no_bug);}


/**************************************************************
*	Struct Define Section
**************************************************************/


/**************************************************************
*	Prototype Declare Section
**************************************************************/


/**************************************************************
*	Global Variable Declare Section
**************************************************************/
u8 Flag_LearnState = 0;
u16 PulseTab[MAX_PULSE_LEN];


/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *  @name						void Infrared_GPIO_Configuration()
 *	@description		������ն�GPIO������
 *	@param					PA.01 ���ⲿ�жϽ��տ�
 *	@return		
 *  @notice
 */
void Infrared_GPIO_Configuration()
{
	GPIO_InitTypeDef GPIO_InitType;
	
	RCC_APB2PeriphClockCmd(INFRARED_RCC_GPIOx,ENABLE);
	GPIO_InitType.GPIO_Mode = GPIO_Mode_IPU;			//��������
	GPIO_InitType.GPIO_Pin = INFRARED_GPIO_Pinx;
	GPIO_InitType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(INFRARED_GPIOx,&GPIO_InitType);
	GPIO_EXTILineConfig(INFRARED_EXTI_GPIOx,INFRARED_EXTI_Line);
}



/**
 *  @name						void Infrared_EXTI_Configuration()
 *	@description		�����߽��ܶ��ⲿ�жϳ�ʼ������
 *	@param			
 *	@return		
 *  @notice
 */
void Infrared_EXTI_Configuration()
{
	EXTI_InitTypeDef EXTI_InitType;
	NVIC_InitTypeDef NVIC_InitType;
	
	EXTI_InitType.EXTI_Line = EXTI_Line1;
	EXTI_InitType.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitType.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitType.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitType);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					//NVIC �ж�����
	NVIC_InitType.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitType.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitType.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitType.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitType);
}



/**
 *  @name						void Infrared_Send()
 *	@description		���ⷢ�䣬���� PulseTab[]�ڵ����ݷ�����
 *	@param			
 *	@return		
 *  @notice
 */
void Infrared_Send()
{
	u8 i;
	
	for(i=0; i<100 && PulseTab[i]!=0xffff; i++)
	{
		if(i%2 == 0)
		{
			TIM_Cmd(TIM2,ENABLE);
			delay_us(PulseTab[i]);
			TIM_Cmd(TIM2,DISABLE);
		}
		else
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
			delay_us(PulseTab[i]);
		}
	}
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}


/**
 *  @name					void EXTI1_IRQHandler()
 *	@description	�ⲿ�ж�1�жϴ���������ڲɼ����Ⲩ��
 *	@param			
 *	@return		
 *  @notice
 */
void EXTI1_IRQHandler()
{
	u16 pulseWidth = 0;
	u8 i = 0;
	Flag_LearnState = 0;
	
	//�ж�ָʾ
	
	while(1)
	{
		if(IR_RDATA)						//�и�������֣���������ź�
		{
			pulseWidth = 0;
			while(IR_RDATA)
			{
				pulseWidth++;
				delay_us(19);
				if(pulseWidth >= 2000)			// >40ms �������¼
					break;
			}
			
			if(pulseWidth<=15 || pulseWidth>=2000)		// >40us || <300us �������¼
				break;
			PulseTab[i] = pulseWidth*20;
			i++;
		}		
		else										//�ز��ź�,ż��λΪ�͵�ƽ���ز���������λΪ�ߵ�ƽ�����У�
		{	
			pulseWidth = 0;
			while(IR_RDATA == 0)			
			{
				pulseWidth++;
				delay_us(19);
			}
			if(pulseWidth<=15 || pulseWidth>=2000) 		// >40ms || <300sus  �������¼
				break;
			PulseTab[i] = pulseWidth*20;
			i++;		
		}
	}
	PulseTab[i] = 0xffff;
	
	Flag_LearnState = 1;
	EXTI_ClearITPendingBit(EXTI_Line1);

	return;
}
	
	



/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */
 

 

 
 
 #ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
          /* User can add his own implementation to report the file name and line number,
             ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
        
          while (1)
          {}
}
#endif
