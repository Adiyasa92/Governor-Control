/*
 * Ini adalah sebuah library dengan fungsi mengupload library
 * Sehingga penlis tidak perlu menulis library terlalu sering
 */

#ifndef __sensor_H
#define __sensor_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "misc.h"

ErrorStatus HSEStartUpStatus;
uint16_t ADCBuffer[10];

void TimerS_Config(void);
void TimerS_Input(void);
void ADC_Config(void);
void ADC_Read(void);
void RPM_Config(void);

typedef struct
{
	uint16_t RPM;
	uint16_t Suhu1;
	uint16_t Suhu2;
	uint16_t O2;
	uint16_t Voltage;
	uint16_t Current;
	uint16_t Frequency;
}a123;
a123 Sensor;

typedef struct
{
	uint16_t High;
	uint16_t Low;
	uint16_t Total;
}b123;
b123 Pulse;

int count_HIGH, count_LOW;

#endif

/*
 * Sensor baca RPM belum fix!!!
 */












