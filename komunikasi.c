//#include "stm32f10x.h"
//#include "stm32f10x_gpio.h"
//#include "stm32f10x_rcc.h"
//#include "stm32f10x_usart.h"
#include "komunikasi.h"

#include <stdio.h>
#include "stdio.h"
#include "string.h"
#include "strings.h"
#include "stdlib.h"

void USART_Config()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable GPIOA clock                                                   */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure USART1 Rx (PA10) as input floating                         */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA9) as alternate function push-pull            */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 configured as follow:
			- BaudRate = 115200 baud
			- Word Length = 8 Bits
			- One Stop Bit
			- No parity
			- Hardware flow control disabled (RTS and CTS signals)
			- Receive and transmit enabled
			- USART Clock disabled
			- USART CPOL: Clock is active low
			- USART CPHA: Data is captured on the middle
			- USART LastBit: The clock pulse of the last data bit is not output to the SCLK pin
	 */
//	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	USART_InitStructure.USART_BaudRate            = 9600;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	dataterima=0;
}

char clear[16]="";
void USART1_IRQHandler(void){
	if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
		static uint8_t cnt = 0;
		char t = (USART1->DR & (uint16_t)0x01FF);

		if( (t != '\n') && (cnt < MAX_STRLEN)){
			Serial.received_string[cnt] = t;
			cnt++;
			dataterima=0;
		}
		else if (t=='\n') {
			Serial.received_string[cnt] = '\0';
			strcpy(Serial.dataMasuk, Serial.received_string);
			sprintf(Serial.received_string,"");
			cnt = 0;
			dataterima = 1;
		}
		else{
			cnt = 0;
		}
	}
}

void USART_PutChar(char c)
{
	while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, c);
}

void USART_PutString(char *s)
{
	while (*s)
	{
		USART_PutChar(*s++);
	}
}

void USART_PutStringn(char *s)
{
	while (*s != '\0')
	{
		USART_PutChar(*s++);
	}
	USART_PutChar('\n');
}

void ParsingData(char *dataMasuk)
{
	int flagParsing = 0;
	char charX;
	int countPR=0;
	Serial.state=0;
	while (*dataMasuk != '\0') {
		charX=*dataMasuk++;
	    if (charX=='|')
	    {
			++countPR;
			Serial.state=0;
		}
	    else
	    {
	    	takeData(countPR,charX);
		}
	}
	flagParsing=1;
}

void takeData(int num,char strData)
{
	switch (num) {
		case 0:
			Serial.dataMasuk1[Serial.state] = strData;
			Serial.state++;
			break;
		case 1:
			Serial.dataMasuk2[Serial.state] = strData;
			Serial.state++;
			break;
		default:
			break;
	}
}

void Parsing1(){
	switch (Serial.dataMasuk1[0]) {
		case 'T':													/* 2. Set Point */
			Command2();
			break;
		case 'P':													/* 3. Set kontrol P */
			Command3();
			break;
		case 'I':													/* 4. Set kontrol I */
			Command4();
			break;
		case 'D':													/* 5. Set kontrol D */
			Command5();
			break;
		case 'L':													/* 6. Save PID ke memori FLASH */
			Command6();
			break;
		case 'M':													/* 7. Use mode control (Manual[0], P[1], PI[2], PD[3], PID[4] */
			Command7();
			break;
		case 'S':													/* 8. Use set servo if mode manual */
			Command8();
			break;
		case 'Y':													/* 8. On relay for LOAD */
			Command9();
			break;
		case 'N':													/* 8. Off relay for LOAD */
			Command10();
			break;
		default:
			break;
	}
}

void Realtime(){													/* 1. Baca sensor tegangan, kecepatan, arus */
	USART_PutString("5|");
	sprintf(strsensor, "%d", Sensor.RPM);							// a. Sensor RPM
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Sensor.Voltage);						// b. Sensor tegangan
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Sensor.Current);						// c. Sensor arus
//	sprintf(strsensor, "%d", 0);									// c. Sensor arus
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Sensor.O2);							// d. Sensor lambda
//	sprintf(strsensor, "%d", 0);									// d. Sensor lambda
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Servo2);								// e. Servo
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Serial.Mode);							// f. Kontrol
//	sprintf(strsensor, "%d", 0);
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Kp);									// g. Kontrol P
//	sprintf(strsensor, "%d", 0);
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Ki);									// h. Kontrol I
//	sprintf(strsensor, "%d", 0);
	USART_PutString(strsensor);
	USART_PutString("|");
	sprintf(strsensor, "%d", Kd);									// i. Kontrol D
//	sprintf(strsensor, "%d", 0);
	USART_PutString(strsensor);
	USART_PutString("|");
//	sprintf(strsensor, "%d", 0);									// i. Kontrol D
	sprintf(strsensor, "%d", SetPoint);								// j. Setpoint
	USART_PutString(strsensor);
	USART_PutStringn("|0");											// END
}

void Command2(){													/* 2. Set Point */
	SetPoint = atoi(Serial.dataMasuk2);
}

void Command3(){													/* 3. Set kontrol P */
	Kp = atoi(Serial.dataMasuk2);
}

void Command4(){													/* 4. Set kontrol I */
	Ki = atoi(Serial.dataMasuk2);
}

void Command5(){													/* 5. Set kontrol D */
	Kd = atoi(Serial.dataMasuk2);
}

void Command6(){													/* 6. Save PID ke memori FLASH */
	FLASH_Program();
}

void Command7(){													/* 7. Use mode control (Manual[0], P[1], PI[2], PD[3], PID[4] */
	Serial.Mode = atoi(Serial.dataMasuk2);
	Control();
}

void Command8(){													/* 8. Use set servo if mode manual */
	Servo1 = atoi(Serial.dataMasuk2);
	Control();
}

void Command9(){
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_SET);
	//Lamp = 1;
}

void Command10(){
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_RESET);
//	Lamp = 0;
}

void clean(char *var) {
    int i = 0;
    while(var[i] != '\0') {
        var[i] = '\0';
        i++;
    }
}

void config_Name(char* Artinama){
	USART_PutString("AT+NAME=");
	USART_PutString(Artinama);
	USART_PutString("\r\n");
}
