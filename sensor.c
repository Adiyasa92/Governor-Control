#include "sensor.h"

void ADC_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_DeInit();																			/* RCC system reset(for debug purpose) */
	RCC_HSEConfig(RCC_HSE_ON);																/* Enable HSE */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();												/* Wait till HSE is ready */
	if (HSEStartUpStatus == SUCCESS)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);													/* HCLK = SYSCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1);														/* PCLK2 = HCLK */
		RCC_PCLK1Config(RCC_HCLK_Div2);														/* PCLK1 = HCLK/2 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div4);													/* ADCCLK = PCLK2/4 */
		*(vu32 *)0x40022000 = 0x01;															/* Flash 2 wait state */
		RCC_PLLConfig(0x00010000, RCC_PLLMul_9);											/* PLLCLK = 8MHz * 9 = 56 MHz */
		RCC_PLLCmd(ENABLE);																	/* Enable PLL */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);								/* Wait till PLL is ready */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);											/* Select PLL as system clock source */
		while (RCC_GetSYSCLKSource() != 0x08);												/* Wait till PLL is used as system clock source */
	}

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA , ENABLE);			/* Enable ADC1 and GPIOC clock */
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_DMA1 , ENABLE ) ;
	DMA_InitStructure.DMA_BufferSize = 5;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCBuffer;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd ( DMA1_Channel1 , ENABLE ) ;
	RCC_ADCCLKConfig ( RCC_PCLK2_Div6 ) ;

	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1 , ENABLE) ;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel = 5;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_7Cycles5);
	ADC_Cmd ( ADC1 , ENABLE ) ;
	ADC_DMACmd ( ADC1 , ENABLE ) ;
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd ( ADC1 , ENABLE ) ;

}
int statuspwm=0,countxy=0;
int nilaipwm=0;

void ADC_Read(void){
	Sensor.Suhu1 = ADCBuffer[0];
	Sensor.Suhu2 = ADCBuffer[1];
	Sensor.O2 = ADCBuffer[2];
	Sensor.Voltage = ADCBuffer[3];
	Sensor.Current = ADCBuffer[4];
	Pulse.Total = Pulse.High + Pulse.Low;												/* Hasil Pulse.Total hanya jumlah ticks */
	if (Pulse.Total>250 && Pulse.Total<650 ) {
//		Sensor.Frequency = 20000/Pulse.Total;											/* Frekuensi 50Hz di dapat dari pembacaan 400 */
		Sensor.RPM = Pulse.Total;
	}
//	Sensor.RPM = Pulse.Total;
	Pulse.Total = 0;
	//Sensor.Frequency = 20000/Pulse.Total;												/* Frekuensi 50Hz di dapat dari pembacaan 400 */
	//Sensor.RPM = Sensor.Frequency * 60;													/* Frekuensi 50Hz di dapat dari pembacaan 400 */

//	if (Pulse.Total>300 && Pulse.Total<600 ) {
//		Sensor.Frequency = 20000/Pulse.Total;											/* Frekuensi 50Hz di dapat dari pembacaan 400 */
//	}																					/* Setpoint = 400 */
}

// Use Timer 3
void RPM_Config(void)
{
	/* Init GPIO for input RPM */
	TIM_TimeBaseInitTypeDef  TIM_TBSRpm;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);								/* Configuration for PORTA Timer 3 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;												/* Configuration for input PA.6 and PA.7 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;												/* Configuration Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//setting timer 2 interrupt to 1hz ((2400000/24000)*1000)s
	TIM_TBSRpm.TIM_Prescaler = 720;
	TIM_TBSRpm.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TBSRpm.TIM_Period = 5-1;
	TIM_TBSRpm.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TBSRpm);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Regular);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
//		Pulse.Total = 0;
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==1)									/* Modifikasi Pin yang digunakan antara pin 6 atau 7 */
		{
			if (count_HIGH==0) {

				Pulse.Low = count_LOW;
				count_LOW=0;
			}
			count_HIGH++;
		}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0)
		{
			if (count_LOW==0) {
				Pulse.High = count_HIGH;
				count_HIGH=0;
			}
			count_LOW++;
		}
	}
}
