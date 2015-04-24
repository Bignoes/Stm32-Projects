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
 *	2		v2.0			2015/4/23	Ray												Sucessfully control the air conditioner
 *	3		v2.1			2015/4/24 Ray												Packaging the Infrared Module
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

#ifdef INFRARED_RECEIVE
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
 *	@description		�����߽��ܶ��ⲿ�жϳ�ʼ�����ã�����Ϊ ��·1��ʹ��PA.1��Ϊ�ⲿ�жϵ������
 *	@param					
 *	@return		
 *  @notice					���ı��ⲿ�жϵ�����ˣ��˺����ڵ���·����Ҳ��ı�s
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
 *  @name					void EXTI1_IRQHandler()
 *	@description	�ⲿ�ж�1�жϴ���������ڲɼ����Ⲩ��
 *	@param			
 *	@return				ȫ�ֱ���Flag_LearnState�������ڷ����Ƿ���ѧϰ������
 *  @notice
 */
void EXTI1_IRQHandler()
{
	u16 pulseWidth = 0;
	u16 i = 0;
	
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
			
			if(pulseWidth<=15 || pulseWidth>=2000)		// >40ms || <300us �������¼
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
	PulseTab[i++] = pulseWidth;
	PulseTab[i] = 0xffff;
	
	Flag_LearnState = 1;
	EXTI_ClearITPendingBit(EXTI_Line1);

	return;
}

#endif




#ifdef INFRARED_SEND
/**
 *  @name						void Infrared_Send()
 *	@description		���ⷢ�䣬���� PulseTab[]�ڵ����ݷ�����
 *	@param			
 *	@return		
 *  @notice
 */
void Infrared_Send()
{
	u16 i;
	
	EXTI->IMR &= ~(0x00000002);			//���жϣ����ⷢ�͵ĺ����߱��Լ�����
	for(i=0; i<MAX_PULSE_LEN && PulseTab[i]!=0xffff; i++)
	{
		if(i%2 == 0)
		{
			TIM_Cmd(TIM2,ENABLE);
			delay_us(PulseTab[i]);
			TIM_Cmd(TIM2,DISABLE);
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
		}
		else
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
			delay_us(PulseTab[i]);
		}
	}
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	
	EXTI->IMR |= (0x00000002);		//���ж�
}


/**
 *  @name					void TIM2_PWM_Init(u16 arr,u16 psc)
 *	@description	��ʼ����ʱ��2�����ã�����ʱ��2����PWM���ƣ�PWM�����Ϊ PA.0
 *	@param				arr --	u16,��ʱ����װֵ
									psc --	u16,��ʱ����Ƶֵ							
 *	@return		
 *  @notice				PWMƵ�� = 72M/((arr+1)*(psc+1)),�����������ⷢ����ز�����Ҫ����38kHz�ķ�������ȡarr = 1895,psc = 0��
 */
void TIM2_PWM_Init(u16 arr,u16 psc)
{
	/* ��ʼ���ṹ�嶨�� */
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef 	TIM_OCInitStructure;
	/* ʹ����Ӧ�˿ڵ�ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIO����ʱ��
	
	/* GPIOA.0��ʼ�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			// TIM2 CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// PA.0 �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	/* TIM2 ��ʼ��*/
	TIM_TimeBaseInitStructure.TIM_Period = arr;	 //��һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//��ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;  //ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	/* ��ʱ��TIM2 Ch1 PWMģʽ��ʼ�� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  //ѡ��ʱ��ģʽ:TIM PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	//TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;	  //ռ�ձ� 50%
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/3;	  //ռ�ձ�1:3
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	/* ʹ��TIM2��CCR1�ϵ�Ԥװ�ؼĴ��� */
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	/* ʹ�ܶ�ʱ�� */
//	TIM_Cmd(TIM2,ENABLE);
}

#endif

	

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
