/*
 * Ini adalah sebuah library dengan fungsi mengupload library
 * Sehingga penlis tidak perlu menulis library terlalu sering
 */

#ifndef __aktuator_H
#define __aktuator_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"

uint16_t Servo1, Servo2;
//uint16_t CCR1_Val;
uint16_t Prescaler_Valve, Prescaler_Servo;
uint16_t PeriodeV, PeriodeV2;
uint16_t PeriodeS, PeriodeS2;
int Kontrol;
uint16_t servo_high, servo_low, servo_counter;

void PWM_Config(void);
void Servo_Config(void);
void TIM2_IRQHandler(void);

#endif
