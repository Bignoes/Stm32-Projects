/***************************************************************************************
 *	FileName					:		pwm.c
 *	CopyRight					:
 *	ModuleName				:		ѧϰ�ͺ����ز�����ģ��
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
 *	1		v1.0			2015/04/21	Ray											Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include "stm32f10x.h"


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
