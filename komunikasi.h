/*
 * Ini adalah sebuah library dengan fungsi mengupload library
 * Sehingga penlis tidak perlu menulis library terlalu sering
 */

#ifndef __komunikasi_H
#define __komunikasi_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "sensor.h"
#include "memori.h"

/***************************************************************************//**
 * Declare function prototypes
 ******************************************************************************/
void Clock_Config(void);
void USART_Config(void);

#define MAX_STRLEN 100 // this is the maximum string length of our string in characters
 // this will hold the recieved string
int Lamp;

typedef struct
	{
		volatile char received_string[MAX_STRLEN+1];
		char dataMasuk[11];
		char dataMasuk1[4];
		char dataMasuk2[5];
		int status;
		int state;
		int Mode;
	} c123;
	c123 Serial;

	int dataterima;
	char strsensor[10];

#endif
