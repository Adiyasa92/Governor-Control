#ifndef __memori_H
#define __memori_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "aktuator.h"
#include "sensor.h"
#include "komunikasi.h"

void FLASH_Program(void);

uint16_t Kp, Ki, Kd;
uint16_t SetPoint;											// SetPoint
int16_t CalcKp, CalcKi, CalcKd;							// Hasil perhitungan kendali PID
uint16_t mSetP, mP, mI, mD;									// Memori kendali PID
//uint16_t e0,e1;												// Error
int16_t er0, er1;											// Error
int16_t derr;


#endif
