#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#include "komunikasi.h"
#include "sensor.h"
#include "aktuator.h"
#include "memori.h"

int main(void)
{

	SetPoint = 400;
	er0 = 0;
	er1 = 0;
	ADC_Config();
	RPM_Config();
	USART_Config();
	config_Name("KERE");
	Servo_Config();
	Valve2_Config();
	Lamp = 0;
	Servo2 = 780;
	USART_PutStringn("Governor Controller!\n\r");
	while(1)
	{
		ADC_Read();
		if (dataterima==1) {
			clean(Serial.dataMasuk1);
			clean(Serial.dataMasuk2);
			ParsingData(Serial.dataMasuk);
			Parsing1();
			dataterima=0;
		}
		Realtime(); 												// Program real time
		Control();													// Masuk ke Program Kontrol
		int delay;
//		for(delay=0;delay<0x00020000;++delay){}
//		for(delay=0;delay<0x00040000;++delay){}
//		for(delay=0;delay<0x00050000;++delay){}
		for(delay=0;delay<0x00060000;++delay){}

	}
}

