#include "aktuator.h"

void Valve2_Config(void){
	GPIO_InitTypeDef GPIO_InitStructureC;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructureC.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructureC.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructureC);
}

// Timer 1
void Valve_Config(void)
{
//	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE );

	// Setting RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);										/* TIM3 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO , ENABLE);					/* GPIOA and GPIOB clock enable */

	// Init GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	// Init Timer 1
	TIM_OCInitTypeDef  TIM_OCISValve;
	TIM_TimeBaseInitTypeDef  TIM_TBSValve;
	PeriodeV = 20000;
	PeriodeV2 = PeriodeV/1000; 																	/* Set PWM */
	Prescaler_Valve = (uint16_t) (SystemCoreClock / 1000000) - 1;
	/* Time base configuration */
	TIM_TBSValve.TIM_Period = PeriodeV;
	TIM_TBSValve.TIM_Prescaler = Prescaler_Valve;
	TIM_TBSValve.TIM_ClockDivision = 0;
	TIM_TBSValve.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TBSValve.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TBSValve);														/* Base use timer 2 */
	TIM_PrescalerConfig(TIM1,1,TIM_PSCReloadMode_Update);

	TIM_OCISValve.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCISValve.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCISValve.TIM_OutputState = TIM_OutputState_Enable;								/* PWM1 Mode configuration: Channel1 */
	TIM_OCISValve.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCISValve.TIM_Pulse = 500;														/* Set 0 degree */
	//  TIM_OCInitStructure.TIM_Pulse = TimerPeriod>>1;
	TIM_OCISValve.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCISValve.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCISValve.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCISValve.TIM_OCNIdleState = TIM_OCNIdleState_Reset;// IdleState_Reset;

	TIM_OC1Init(TIM1, &TIM_OCISValve);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM1, &TIM_OCISValve);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_UpdateRequestConfig(TIM1, TIM_UpdateSource_Regular);

	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	/* Automatic Output enable, Break, dead time and lock configuration*/
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	TIM_BDTRInitStructure.TIM_DeadTime = 100;//11
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);


	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
	TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);

//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	/* Enable and configure RCC global IRQ channel */
//	NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);


	NVIC_InitTypeDef NVIC_InitStructure1;
	NVIC_InitStructure1.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);
}

void TIM1_CC_IRQHandler(void){

	if(TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET){
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);

		Servo1 = Kontrol * PeriodeV2;
		Servo2 = Kontrol * PeriodeV2;
		TIM1->CCR1=5000;
		TIM1->CCR2=5000;
	}
}

// Servo config menggunakan Timer 2
void Servo_Config(void)
{

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE );

	// Setting RCC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);										/* TIM3 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO , ENABLE);					/* GPIOA and GPIOB clock enable */

	// Init GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Init Timer 2
	TIM_OCInitTypeDef  TIM_OCISServo;
	TIM_TimeBaseInitTypeDef  TIM_TBSServo;
	PeriodeS = 20000;
	PeriodeS2 = PeriodeS/1000; 																	/* If use 1000 then range from 50 to 1000 */
	Prescaler_Servo = (uint16_t) (SystemCoreClock / 1000000) - 1;
	/* Time base configuration */
	TIM_TBSServo.TIM_Period = PeriodeS;
	TIM_TBSServo.TIM_Prescaler = Prescaler_Servo;
	TIM_TBSServo.TIM_ClockDivision = 0;
	TIM_TBSServo.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TBSServo);														/* Base use timer 2 */

	TIM_OCISServo.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCISServo.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCISServo.TIM_OutputState = TIM_OutputState_Enable;								/* PWM1 Mode configuration: Channel1 */
	TIM_OCISServo.TIM_Pulse = 5000;														/* Set 0 degree */
	TIM_OC3Init(TIM2, &TIM_OCISServo);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM2, &TIM_OCISServo);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular);


	TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM2, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructuree2;
	NVIC_InitStructuree2.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructuree2.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructuree2.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructuree2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructuree2);

}

void TIM2_IRQHandler(void){\
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		TIM2->CCR3 = Servo2;
		TIM2->CCR4 = Servo2;
	}
}
