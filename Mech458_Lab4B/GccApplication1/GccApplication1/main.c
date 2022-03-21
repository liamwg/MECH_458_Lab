
//HOW TO UPLOAD A HEX FILE TO MCU WITHOUT PROJECT FILE USING ?COMMAND LINE?


/*
 * Stepper_Test_Code.c
 *
 * Created: 2020-08-20 3:33:28 PM
 * Author : bretd
 * Edited : Talk
 -C"C:\AVRDUDE_Uploader\avrdude.conf" -patmega2560 -cwiring -P\\.\COM4 -D -b115200 -Uflash:w:"$(ProjectDir)Debug\$(TargetName).hex":i
 */ 

#include <stdlib.h>		// the header of the general purpose standard library of C programming language
#include <avr/io.h>		// the header of i/o port
#include <avr/interrupt.h>
#include <inttypes.h>

volatile int current_step = 0;//for the stepper motor, (0-3)
volatile int step_delay = 4; //global variable for i5s speed .
volatile int dir = 1; //this should be either +1 or -1, set in step_what() to give direction for bucket
volatile int dist = 0; //holds absolute value of # of steps to get where needs to go

int spin[4] = {0b00001100, 0b00000110, 0b00000011, 0b00001001};




void mTimer(int count) {
	int i = 0;

	TCCR1B |=_BV(WGM12);		//Set clear timer on compare
	OCR1A = 0x07D0;				//Set timer upper limit to 2000 (1ms @ 2MHz)
	TCNT1 = 0x0000;				//Set timer start to 0
	TIMSK1 |= 0x02;				//Enable interrupt, remove if sei ()

	TIFR1 |=_BV(OCF1A);			//Clear timer flag

	while(i<count)
	{
		while ((TIFR1 & 0x02) != 0x02);	//Check if timer flag set
		
		TIFR1 |=_BV(OCF1A);		//Clear increment
		i++;					//repeat until i = count
		//end if
	}  //end while
}  //end mTimer


void StepperGo(){
	
	current_step += dir;
	
	if(current_step == 4)
	current_step = 0;
	if(current_step == -1)
	current_step = 3;
	
	PORTA = spin[current_step];
	mTimer(step_delay);
}




int main(void) {
	
	//PROGRAM INITIALIZATION 
	TCCR1B |=_BV(CS11);		//Set scaled clock 16MHz/8 -> 2MHz
	DDRA = 0xFF;			//Set port A to output
	
	
	//PROPER STEP DISPLAY PROOF
		
		//go CW 180 degrees	
		
		dir = 1;
		for (int i = 0; i < 1024*2; i++) {
			StepperGo();
		}
		
		dir = -1;
		for (int i = 0; i < 1024*2; i++) {
			StepperGo();
		}
		
		dir = 1;
		for (int i = 0; i < 1024*2; i++) {
			StepperGo();
		}
		
		dir = -1;
		for (int i = 0; i < 1024*2; i++) {
			StepperGo();
		}
		
		dir = 1;
		for (int i = 0; i < 1024*2; i++) {
			StepperGo();
		}
		
		dir = -1;
		for (int i = 0; i < 1024*2; i++) {
			StepperGo();
		}
			
		//PORTA = 0;
		mTimer(2000);
		
		//go CCW 180 degrees
	
}




