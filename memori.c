#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "memori.h"

int limitBawah = 400;
int limitAtas = 900;

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
#define FLASH_PAGE_SIZE			((uint16_t)0x400)
#define BANK1_WRITE_START_ADDR	((uint32_t)0x08008000)
#define BANK1_WRITE_END_ADDR	((uint32_t)0x08008400)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t EraseCounter = 0x00,
		AddressSetP = 0x00, AddressP = 0x04, AddressI = 0x08, AddressD = 0x0B;
uint32_t Data = 0x3210ABCD;
uint32_t NbrOfPage = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;

void FLASH_Program(void)
{
	FLASH_UnlockBank1();																					/* Unlock the Flash Bank1 Program Erase controller */
	NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;							/* Define the number of page to be erased */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);								/* Clear All pending flags */
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)	/* Erase the FLASH pages */
	{
		FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
	}
	while(FLASHStatus == FLASH_COMPLETE)																	/* Program Flash Bank1 */
	{
//		FLASHStatus = FLASH_ProgramWord(AddressSetP, mSetP);
//		FLASHStatus = FLASH_ProgramWord(AddressP, mP);
//		FLASHStatus = FLASH_ProgramWord(AddressI, mI);
//		FLASHStatus = FLASH_ProgramWord(AddressD, mD);
	}
	FLASH_LockBank1();
	while(MemoryProgramStatus != FAILED)																	/* Check the corectness of written data */
	{
//		if((*(__IO uint32_t*) AddressSetP) != mSetP){MemoryProgramStatus = FAILED;}
//		if((*(__IO uint32_t*) AddressP) != mP){MemoryProgramStatus = FAILED;}
//		if((*(__IO uint32_t*) AddressI) != mI){MemoryProgramStatus = FAILED;}
//		if((*(__IO uint32_t*) AddressD) != mD){MemoryProgramStatus = FAILED;}
	}
	if( MemoryProgramStatus == FAILED)
	{
		while(1);
	}
}

void dError(){
	er1 = er0;
	er0 = Sensor.RPM - SetPoint;
	derr = er1 - er0;
}

void KontrolP(){
	CalcKp = ((Kp * (Sensor.RPM - SetPoint)) / 1000);
}

void KontrolI(){
	er1 = er0;
	er0 = Sensor.RPM - SetPoint;
	CalcKi = ((er1-er0) * Ki) / 1000;
}

void KontrolD(){
	er1 = er0;
	er0 = Sensor.RPM - SetPoint;
	CalcKi = ((er1-er0) * 100000 / Kd);
}

void CallKendaliP(){
	KontrolP();
	Servo1 = Servo2 + CalcKp;
	if (Servo1 > limitAtas) {Servo2 = limitAtas;}					//Limit 1700
	else if (Servo2 < limitBawah) {Servo2 = limitBawah;}
	else {Servo2 = Servo1;}
}

void CallKendaliPI(){
	KontrolP();
	KontrolI();
	Servo1 = Servo2 + (CalcKp + CalcKi);
	if (Servo1 > limitAtas) {Servo2 = limitAtas;}					//Limit 1700
	else if (Servo2 < limitBawah) {Servo2 = limitBawah;}
	else {Servo2 = Servo1;}
}

void CallKendaliPD(){
	KontrolP();
	KontrolD();
	Servo1 = Servo2 + (CalcKp + CalcKd);
	if (Servo1 > limitAtas) {Servo2 = limitAtas;}					//Limit 1700
	else if (Servo2 < limitBawah) {Servo2 = limitBawah;}
	else {Servo2 = Servo1;}
}

void CallKendaliPID(){
	KontrolP();
	KontrolI();
	KontrolD();
	Servo1 = Servo2 + (CalcKp + CalcKi + CalcKd);
	if (Servo1 > limitAtas) {Servo2 = limitAtas;}					//Limit 1700
	else if (Servo2 < limitBawah) {Servo2 = limitBawah;}
	else {Servo2 = Servo1;}
}

void Control(){
	if (Serial.Mode == 0) {Servo2 = Servo1;}
	else if (Serial.Mode == 1) {CallKendaliP();}
	else if (Serial.Mode == 2) {CallKendaliPI();}
	else if (Serial.Mode == 3) {CallKendaliPD();}
	else if (Serial.Mode == 4) {CallKendaliPID();}
}
