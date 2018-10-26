#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

#define     ADC_CS    12
#define     ADC_DIO   6
#define     ADC_CLK   13

#define  RedPin     0
#define  YelPin     1
#define  GrePin     2
#define  BluPin     3

#define  Relay1	    29
#define  Relay2     28

#define  RelayWater 27
#define  RelayArm   26

int PinGroup(uint a)
{
		if (a == RedPin) {return RedPin;}
		else if (a == YelPin) {return YelPin;}
		else if (a == GrePin) {return GrePin;}
		else if (a == BluPin) {return BluPin;}
		else {return -1;}
}

float setPinVal(float curVol){

	if (curVol >= 3.5 && curVol < 5) {digitalWrite(PinGroup(0), LOW); printf("Extremely Dry ");}
	else {digitalWrite(PinGroup(0), HIGH);}
	if (curVol >= 3.0 && curVol <= 4.0) {digitalWrite(PinGroup(1), LOW); printf("Dry ");}
	else {digitalWrite(PinGroup(1), HIGH);}
	if (curVol >= 2.0 && curVol <= 3.5) {digitalWrite(PinGroup(2), LOW); printf("Wet ");}
	else {digitalWrite(PinGroup(2), HIGH);}
	if (curVol >= 1.0 && curVol <= 2.5) {digitalWrite(PinGroup(3), LOW); printf("Exeremely Wet ");}
	else {digitalWrite(PinGroup(3), HIGH);}
	printf("   ");
		
	return 0;
}

int shouldWater(float curVol){
	if (curVol > 4.0 && curVol < 5){return 10;}
	else if (curVol >= 3.5 && curVol <= 4.0){return 5;}
	else if (curVol < 3.5){return 0;}
	else if (curVol = 5){return -1;}
}

/* 
 * get_ADC_Result snippet written by Jason from www.adeept.com on 2015/06/04
 * E-mail: support@adeept.com
 */

uchar get_ADC_Result(void)
{
	//10:CH0
	//11:CH1
	uchar i;
	uchar dat1=0, dat2=0;
	
	digitalWrite(ADC_CS, 0);

	digitalWrite(ADC_CLK,0);
	digitalWrite(ADC_DIO,1);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,1);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,0);

	digitalWrite(ADC_DIO,1);	delayMicroseconds(2); //CH0 10
	digitalWrite(ADC_CLK,1);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,0);

	digitalWrite(ADC_DIO,0);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,1);
	digitalWrite(ADC_DIO,1);	delayMicroseconds(2);
	digitalWrite(ADC_CLK,0);	
	digitalWrite(ADC_DIO,1);	delayMicroseconds(2);

	for(i=0;i<8;i++)
	{
		digitalWrite(ADC_CLK,1);	delayMicroseconds(2);
		digitalWrite(ADC_CLK,0);	delayMicroseconds(2);

		pinMode(ADC_DIO, INPUT);
		dat1=dat1<<1 | digitalRead(ADC_DIO);
	}

	for(i=0;i<8;i++)
	{
		dat2 = dat2 | ((uchar)(digitalRead(ADC_DIO))<<i);
		digitalWrite(ADC_CLK,1); 	delayMicroseconds(2);
		digitalWrite(ADC_CLK,0);	delayMicroseconds(2);
	}

	digitalWrite(ADC_CS,1);

	pinMode(ADC_DIO, OUTPUT);

	return(dat1==dat2) ? dat1 : 0;
}

void ServoPos(int z) {
	
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 500000000L;

	digitalWrite(RelayArm, LOW);
	for (int g = 1; g <= z; g += 1){
		nanosleep(&tim , &tim2);
	}
	delay(100);
	digitalWrite(RelayArm, HIGH);
}

int main(void)
{
	float ohm;
	uchar val;
	float vol;
	
	if(wiringPiSetup() == -1){
		printf("wiringPi setup failed!\n");
		exit(1);
	}
	
	int PinAmount = 3; //Set amount of LEDs -1
	
	for(int i=0; i <= PinAmount; i = i + 1){ 
		pinMode(PinGroup(i), OUTPUT);
	}	
	
	pinMode(ADC_CS,  OUTPUT);
	pinMode(ADC_CLK, OUTPUT);

	pinMode(Relay1,	 OUTPUT);
	pinMode(Relay2,	 OUTPUT);

	pinMode(RelayWater, OUTPUT);
	pinMode(RelayArm, OUTPUT);

	digitalWrite(Relay1, HIGH);
	digitalWrite(Relay2, HIGH);

	digitalWrite(RelayWater, HIGH);
	digitalWrite(RelayArm, HIGH);
	
	int currentPlant = 1;
	int chkLoop = 0;


	printf("Setup complete! Starting Voltmeter...\n");

	while(1){
		for(chkLoop; chkLoop <= 4; chkLoop++) {
			pinMode(ADC_DIO, OUTPUT);
			val = get_ADC_Result();
			printf("Analog: %d    ", val);

			vol = (5.0 / 255) * val;
			printf("Voltage: ", "%s");
			printf("%.2fV    ", vol);

			printf("Plant: %d   ", currentPlant);

			ohm = ((vol) / (5-vol)*10);
			printf("R2: %f   ", ohm);

			setPinVal(vol);
			printf("\n");
			delay(500);
		}
		
		chkLoop = 0;

		printf("Plant %d: ", currentPlant);

		if (shouldWater(vol) != -1 && shouldWater(vol) != 0) {
			float tempvar = (shouldWater(vol));
			printf("Watering for %f secounds...\n", tempvar/10);
			ServoPos(currentPlant);
			printf("Going to %d \n", currentPlant);
			delay(3500);
			digitalWrite(RelayWater, LOW);
			delay(tempvar*50);
			digitalWrite(RelayWater, HIGH);
		}
		else {printf("Plant does not need watering or is not connected, ignoring...\n");}

		switch(currentPlant) {
			case 1:
				currentPlant++;
				digitalWrite(Relay1, LOW);
				break;
			case 2:
				currentPlant++;
				digitalWrite(Relay2, LOW);
				break;
			case 3:
				currentPlant = 1;
				delay(10000);
				digitalWrite(Relay1, HIGH);
				digitalWrite(Relay2, HIGH);
				break;
		}
		delay(50);
	}

	return 0;
}
