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
 *	No	version		Data		Revised By			Item			Description
 *	1		v1.0							Ray												Create this file
 *
 ***************************************************************************************/



/**************************************************************
*	Debug switch Section
**************************************************************/


/**************************************************************
*	Include File Section
**************************************************************/
#include "stm32f10x.h"
#include "Infrared.h"
#include "pwm.h"

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
int main(void)
{
	EXTI_InitTypeDef EXTI_InitType;
	
	EXTI_InitType.EXTI_Line = EXTI_Line1;
	EXTI_InitType.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitType.EXTI_Trigger = EXTI_Trigger_Falling;
	
	SystemInit();
	delay_init(72);
	TIM2_PWM_Init(1895,0);
	Infrared_GPIO_Configuration();
	Infrared_EXTI_Configuration();
	Key_GPIO_Configuration();
	
	while(1)
	{
		if(Is_Press_Key() == 1)
		{
			EXTI_InitType.EXTI_LineCmd = DISABLE;
			EXTI_Init(&EXTI_InitType);
			Infrared_Send();
			EXTI_InitType.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitType);
		}
	}


	return 0;
}
/**
 *  @name
 *	@description
 *	@param			
 *	@return		
 *  @notice
 */