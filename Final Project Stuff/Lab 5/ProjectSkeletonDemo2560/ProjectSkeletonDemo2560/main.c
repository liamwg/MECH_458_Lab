// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!

// Open up the document in START -> WinAVR -> AVR LibC -> User Manual -> avr/interrupt.h
// Chapter 15, in Full Manual... THIS HAS A LOT OF IMPORTANT INFO...I have mentioned this at least 3 times!!!

// For those that are still having major problems, I've seen about 1/3 of the class with major problems in
// code structure. If you are still having major problems with your code, it's time to do a VERY quick overhaul.
// I've provided a skeleton structure with an example using two input capture interrupts on PORTDA0 and A3
// Please try this in the debugger.

// Create a watch variable on STATE. To do this right click on the variable STATE and then
// Add Watch 'STATE'. You can see how the variable changes as you click on PINDA0 or PINDA3. Note that the interrupt
// catches a rising edge. You modify this to suit your needs.

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
//#include <util/delay.h>
#include "lcd.h"
#include "LinkedQueue.h" 	/* This is the attached header file, which cleans things up */



//Function Declarations

void mTimer(int count);
void PWM();
void CCW();
void brake();
void CW();
void initialize();
void stepper();


// Global Variable
volatile char STATE;
volatile unsigned int i=0;
volatile unsigned int reflect = 0;
volatile unsigned int ADC_result = 0;
volatile unsigned int ADC_low;
volatile unsigned int ADC_high;
volatile unsigned int ADC_result_flag = 0;
volatile int lowest_ref = 0xFFFF;
volatile const int alum_low = 0;
volatile const int alum_high = 396;
volatile const int steel_low = 397;
volatile const int steel_high = 735;
volatile const int white_low = 736;
volatile const int white_high = 975;
volatile const int black_low = 976;
volatile const int black_high = 1023;
volatile unsigned int alum_count = 0;
volatile unsigned int steel_count =0;
volatile unsigned int white_count = 0;
volatile unsigned int black_count = 0;


volatile int current_step = 0; //current position of stepper
volatile int direction = 1;
const int steps[4] = {0b00110110, 0b00101110, 0b00101101, 0b00110101};
const int black_pos = 200;
const int alum_pos = 50;
const int white_pos = 100;
const int steel_pos = 150;
volatile int position = 0;
volatile int a_delay = 50;
volatile int a_flag = 0;


link *head;			/* The ptr to the head of the queue */
link *tail;			/* The ptr to the tail of the queue */
link *newLink;		/* A ptr to a link aggregate data type (struct) */
link *rtnLink;		/* same as the above */
element eTest;		/* A variable to hold the aggregate data type known as element */


int main(int argc, char *argv[]){
	
	initialize(); //initializes LCD, interrupts

	PWM(); //initialize PWM 
	
	while ((PINA & 0b10000000) != 0x00){ //homing the stepper motor to the black position
		stepper();
	}
	position = 200; //setting the home position of the stepper after homing
	
	LCDClear();
	LCDWriteStringXY(0,0,"i hOmE :)");
	//LCDClear();
	
	CCW();

	
	 
	goto POLLING_STAGE;

	// POLLING STATE
	POLLING_STAGE:
	//PORTC |= 0xF0;	// Indicates this state is active
	
	switch(STATE){
		case (0) :
		goto POLLING_STAGE;
		break;	//not needed but syntax is correct
		case (1) :
		goto MAGNETIC_STAGE;
		break;
		case (2) :
		goto REFLECTIVE_STAGE;
		break;
		case (3) :
		goto BUCKET_STAGE;
		break;
		case (5) :
		goto END;
		default :
		goto POLLING_STAGE;
	}//switch STATE
	

	MAGNETIC_STAGE:
	// Do whatever is necessary HERE
	//PORTC = 0x01; // Just output pretty lights know you made it here
	//Reset the state variable
	STATE = 0;
	goto POLLING_STAGE;

	REFLECTIVE_STAGE:
	
	//PORTC = 0x04; // Just output pretty lights know you made it here
	//Reset the state variable
	STATE = 0;
	goto POLLING_STAGE;
	
	BUCKET_STAGE:
	// Do whatever is necessary HERE
	//PORTC = 0x08;
	//Reset the state variable
	STATE = 0;
	goto POLLING_STAGE;
	
	END:
	// The closing STATE ... how would you get here?
	//PORTC = 0xF0;	// Indicates this state is active
	// Stop everything here...'MAKE SAFE'
	return(0);

}

ISR(INT0_vect){//rising edge restart belt
	mTimer(20); //debounce
	if((PIND & 0x01) == 01){
		CCW();
	}
}
ISR(INT1_vect) { //falling edge killswitch
	if((PIND & 0x02) == 0x00){
		PORTB = 0x00;
		LCDClear();
		LCDWriteStringXY(0,0, "Kill Switch Activated");
		while (1){} //infinite loop
	}
}  // the interrupt will be triggered if the ADC is done ========================

/* Set up the External Interrupt 2 Vector */
ISR(INT2_vect){ //when something is in front of OR
	mTimer(20);//debounce
	/* Toggle PORTC bit 2 */
	i++;
	lowest_ref = 0xFFFF;
	LCDWriteStringXY(0,0,"Counter=");
	LCDWriteIntXY(9,0,i,5);
	if(((PIND & 0x04) == 0x04) && (PINB == 0b01110000)){ //active high, check if actually triggered and belt is on
		ADCSRA |= _BV(ADSC); //initialize ADC with one conversion and classify a part		
	}
	
	//mTimer(20);//debounce
	STATE = 2;
}

ISR(INT3_vect){ //when something is in front of EX
	mTimer(20); //debounce
	/* Toggle PORTC bit 3 */
	
	//brake for the first element if brake flag not high
	
	//check is current element and next element are same type
		//
	
	if((PIND & 0x08) == 0x00){ //active low, checks if EX triggered
		
		
		i--; 
		
		LCDClear();
		LCDWriteStringXY(0,0,"Counter=");
		LCDWriteIntXY(9,0,i,5);
		
		//int mat = firstValue(&head);	
		dequeue(&head, &tail, &rtnLink); /* remove the item at the head of the list */
		
		if(rtnLink->e.material == 1){
			if(position != alum_pos){
				CW(); //brake
				mTimer(20);
				PORTB = 0x00; // turn belt off
			while (position != alum_pos){
				stepper_direction(alum_pos); //moving the stepper motor to the indicated position
				stepper();
			}	
			CCW();
			}
		}
		if(rtnLink->e.material == 2){
			if(position != steel_pos){
				CW(); //brake
				mTimer(20);
				PORTB = 0x00; // turn belt off
			while (position != steel_pos){ //moving the stepper motor to the indicated position
				stepper_direction(steel_pos);
				stepper();
			}
			CCW();
			}
		}
		if(rtnLink->e.material == 3){
			if(position != white_pos){
				CW(); //brake
				mTimer(20);
				PORTB = 0x00; // turn belt off
			while (position != white_pos){ //moving the stepper motor to the indicated position
				stepper_direction(white_pos);
				stepper();
			}
			CCW();
			}
		}
		if(rtnLink->e.material == 4){
			if(position != black_pos){
				CW(); //brake
				mTimer(20);
				PORTB = 0x00; // turn belt off
			while (position != black_pos){ //moving the stepper motor to the indicated position
				stepper_direction(black_pos);
				stepper();
			}
			CCW();
			}
		}
		
		LCDWriteIntXY(5,1,rtnLink->e.material,1);
		free(rtnLink);	
		mTimer(50);
		STATE = 3;	

	}
	//while((PIND & 0x08) == 0x00){ //wait until switch is released		
	//}
	//mTimer(20);//debounce
	
}

ISR(ADC_vect){// triggers when ADC is done
	ADC_low = ADCL;
	ADC_high = ADCH << 8;
	ADC_result = ADC_low + ADC_high;
			
	//LCDClear();
	LCDWriteStringXY(0,0,"Counter=");
	LCDWriteIntXY(9,0,i,5);
	
	if(ADC_result < lowest_ref){
		lowest_ref = ADC_result;
		//LCDWriteStringXY(0,1,"Lowest=");
		//LCDWriteIntXY(7,1,lowest_ref,4);
		//mTimer(20);
		
	} //if the result of ADC is the lowest reflectivity value so far, update lowest reflectivity with ADC result
		
	//LCDClear();
	/*LCDWriteStringXY(0,0,"Counter=");
	LCDWriteIntXY(9,0,i,5);
	LCDWriteIntXY(0,1,ADC_low,3);
	LCDWriteIntXY(4,1,ADC_high,3);
	LCDWriteIntXY(8,1,ADC_result,4); 
	*/
	//mTimer(20);

	
	if((PIND & 0x04) == 0x04){ //if OR sensor is still triggered
		ADCSRA |= _BV(ADSC); //initialize ADC with one conversion
	}
	else{
		
	link* newLink;
		
	initLink(&newLink);
		
	if (lowest_ref >= alum_low && lowest_ref <= alum_high){
		//LCDWriteStringXY(0,1,"Aluminum");
		newLink->e.material = 1; //add to queue
		enqueue(&head, &tail, &newLink);
		alum_count++;
		LCDWriteIntXY(0,1,newLink->e.material,1);
	}
	else if (lowest_ref >= steel_low && lowest_ref <= steel_high){
		//LCDWriteStringXY(0,1,"Steel");
		newLink->e.material = 2;
		enqueue(&head, &tail, &newLink);
		steel_count++;
		LCDWriteIntXY(0,1,newLink->e.material,1);
	}
	else if (lowest_ref >= white_low && lowest_ref <= white_high){
		//LCDWriteStringXY(0,1,"White");
		newLink->e.material = 3;
		enqueue(&head, &tail, &newLink);
		white_count++;	
		LCDWriteIntXY(0,1,newLink->e.material,1);
	}
	else if(lowest_ref >= black_low && lowest_ref <= black_high){
		//LCDWriteStringXY(0,1,"Black");
		newLink->e.material = 4;
		enqueue(&head, &tail, &newLink);
		black_count++;
		LCDWriteIntXY(0,1,newLink->e.material,1);
	}//else
	
	}//else
	}//ISR
// If an unexpected interrupt occurs (interrupt is enabled and no handler is installed,
// which usually indicates a bug), then the default action is to reset the device by jumping
// to the reset vector. You can override this by supplying a function named BADISR_vect which
// should be defined with ISR() as such. (The name BADISR_vect is actually an alias for __vector_default.
// The latter must be used inside assembly code in case <avr/interrupt.h> is not included.
ISR(BADISR_vect)
{
	PORTL = 0xFF;
	while(1);
	}



void mTimer(int count){
	/* this system clock is 8MHz, the timer runs on the CPU clock which is a function of the system clock.
	The system clock has been pre-scaled by 2. 
	*/
	
	int j; //keeps track of loop number
	
	j = 0; //initializes loop counter
	
	//Set the Waveform Generation Mode bit description to Clear Timer on Compare Math mode (CTC) only
	
	TCCR1B |=_BV(WGM12);		//Set WGM bits to 0100, page 145
							//note WGM is spread over two register
							
	OCR1A = 0x03E8; //Sets output compare register for 1000 cycles = 1ms
	
	TCNT1 = 0x0000; //Sets initial value of timer counter to 0x0000
	
	//TIMSK1 = TIMSK1 | 0b00000010; //Enable the output compare interrupt (removed because we are using global interrupts)
	
	TIFR1 |= _BV(OCF1A); //clear timer interrupt flag and begin new timing
	
	//Poll the timer to determine when the timer has reached 0x03E8
	
	while(j<count){
		
		if((TIFR1 & 0x02) == 0x02){
			
			TIFR1 |= _BV(OCF1A); // clear interrupt flag by writing a ONE to the bit
			
			j++; //increment loop number
						
		} // end if
	} // end while
	
	return;
	
}// mTimer	

void aTimer(int count){
	/* this system clock is 8MHz, the timer runs on the CPU clock which is a function of the system clock.
	The system clock has been pre-scaled by 2. 
	*/
	
	LCDWriteStringXY(0,0,"In aTimer");
	
	int k = 0; //keeps track of loop number
		
	//Set the Waveform Generation Mode bit description to Clear Timer on Compare Math mode (CTC) only
	
	TCCR2B |=_BV(WGM22);		
							
	OCR2A = 10; //Sets output compare register for 100 cycles = 0.1ms
	
	TCNT2 = 0x0000; //Sets initial value of timer counter to 0x0000
	
	TCCR2B |=_BV(CS21);	//Set scaled clock 16MHz/8 -> 2MHz
		
	TIFR2 |= _BV(OCF2A); //clear timer interrupt flag and begin new timing
	
	//Poll the timer to determine when the timer has reached 0x03E8
	LCDClear();
	while(k<count){
		
		LCDWriteIntXY(0,1,count,4);
		if((TIFR2 & 0x02) == 0x02){
			TIFR2 |= _BV(OCF2A); // clear interrupt flag by writing a ONE to the bit
			k++; //increment loop number
			LCDWriteIntXY(0,0,k,4);
		} // end if
	} // end while
	
	return;
	
}// mTimer	

void PWM(){
	TCCR0A = 0b10000011; //set first two bits to 10, clear OC0A on compare match, set last two bits to WGM mode 3, bits 011 (bits are separated into 2 registers)

	TCCR0B = 0b00000011; // set bit 3 to WGM mode 3 (first bit of 011), set last 3 bits to 011 to prescale clock by 64

	//TIMSK0 = 0b00000010; // set bit 1 to 1, enables OCIE0A

	OCR0A = 0x40; // set OCRA to something
	//use 0x28 for 25%
	// use 0x50 for 50%
	//use 0xA0 for 100% of 50%
}

void CCW(){
	PORTB = 0b01110000;
}

void brake(){
	PORTB = 0b00110000;
}

void CW(){
	PORTB = 0b00111000;
}

/**************************************************************************************
* DESC: initializes the linked queue to 'NULL' status
* INPUT: the head and tail pointers by reference
*/

void setup(link **h,link **t){
	*h = NULL;		/* Point the head to NOTHING (NULL) */
	*t = NULL;		/* Point the tail to NOTHING (NULL) */
	return;
	}/*setup*/




/**************************************************************************************
* DESC: This initializes a link and returns the pointer to the new link or NULL if error
* INPUT: the head and tail pointers by reference
*/
void initLink(link **newLink){
	//link *l;
	*newLink = (link*)malloc(sizeof(link));
	(*newLink)->next = NULL;
	return;
	}/*initLink*/




/****************************************************************************************
*  DESC: Accepts as input a new link by reference, and assigns the head and tail
*  of the queue accordingly
*  INPUT: the head and tail pointers, and a pointer to the new link that was created
*/
/* will put an item at the tail of the queue */


void enqueue(link **h, link **t, link **nL){

	if (*t != NULL){
		/* Not an empty queue */
		(*t)->next = *nL;
		*t = *nL; //(*t)->next;
		}/*if*/
	else{
		/* It's an empty Queue */
		//(*h)->next = *nL;
		//should be this
		*h = *nL;
		*t = *nL;
		}/* else */
		return;
}/*enqueue*/




/**************************************************************************************
* DESC : Removes the link from the head of the list and assigns it to deQueuedLink
* INPUT: The head and tail pointers, and a ptr 'deQueuedLink'
* 		 which the removed link will be assigned to
*/
/* This will remove the link and element within the link from the head of the queue */
void dequeue(link **h, link **t, link **deQueuedLink){
	/* ENTER YOUR CODE HERE */
	*deQueuedLink = *h;	// Will set to NULL if Head points to NULL
	/* Ensure it is not an empty queue */
	if (*h != NULL){
		*h = (*h)->next;
	}/*if*/
	if (*h == NULL){
		*t = NULL; //since the queue is empty, tail should point to null
	}
							
return;
}/*dequeue*/




/**************************************************************************************
* DESC: Peeks at the first element in the list
* INPUT: The head pointer
* RETURNS: The element contained within the queue
*/
/* This simply allows you to peek at the head element of the queue and returns a NULL pointer if empty */

int firstValue(link **h){
	return((*h)->e.material);
}/*firstValue*/





void clearQueue(link **h, link **t){
	/**************************************************************************************
	* DESC: deallocates (frees) all the memory consumed by the Queue
	* INPUT: the pointers to the head and the tail
	*/
	/* This clears the queue */
	link *temp;

	while (*h != NULL){
		temp = *h;
		*h=(*h)->next;
		free(temp);
	}/*while*/
										
	/* Last but not least set the tail to NULL */
	*t = NULL;

	return;
}/*clearQueue*/





/**************************************************************************************
* DESC: Checks to see whether the queue is empty or not
* INPUT: The head pointer
* RETURNS: 1:if the queue is empty, and 0:if the queue is NOT empty
*/
/* Check to see if the queue is empty */
char isEmpty(link **h){
	/* ENTER YOUR CODE HERE */
	return(*h == NULL);
}/*isEmpty*/


void stepper(){
	if (current_step == 4){
		current_step = 0;
	}
	if (current_step == -1){
		current_step = 3;
	}
	PORTA = steps[current_step];
	
	current_step += direction;
	position += direction;
	if (position == 201){
		position = 1;
	}
	if (position == 0){
		position = 200;
	}
	
	
	if(a_flag == 1){
		aTimer(a_delay);
		} else{
		mTimer(20);
	}
	
	
}

void stepper_direction(int required_pos){
	
	a_flag = 1;
	
	int distance = required_pos - position;
	
	if (distance >= 100)
	distance -= 200;
	
	if (distance < -100)
	distance += 200;
	
	direction = (distance)/ abs(distance);
	LCDWriteIntXY(10,1,direction,2);
	
	if (abs(distance) > 4 && (a_delay > 10) ) //accelerating
	a_delay -= 3; 
	if (abs(distance) < 8 && (a_delay < 80) ) //braking
	a_delay += 5;
	
}




/**************************************************************************************
* DESC: Obtains the number of links in the queue
* INPUT: The head and tail pointer
* RETURNS: An integer with the number of links in the queue
*/
/* returns the size of the queue*/


void initialize(){
	//Timer Code
	CLKPR = 0x80;
	CLKPR = 0x01;		//  sets system clock to 8MHz
	TCCR1B |= _BV(CS11);
	
	//Initialize LCD module
	InitLCD(LS_BLINK|LS_ULINE);
	
	LCDClear();
	LCDWriteStringXY(0,0,"Counter=00000");

	STATE = 0;
		
	rtnLink = NULL;
	newLink = NULL;
	
	setup(&head, &tail); //set up linked list
	
	//Setting inputs and outputs
	DDRD = 0x00;		// use PORTD for interrupts (pins 18-21)
	DDRC = 0xFF;		// output for LCD
	DDRB = 0xFF;		// output for DC motor
	DDRL = 0xFF;		// output for LED display
	DDRA = 0b00111111;	// output for stepper motor

	cli();		// Disables all interrupts
	
	//set up interrupts
	
	EIMSK |= _BV(INT0); //enable INT0
	EICRA |= _BV(ISC01) | _BV(ISC00); //rising edge interrupt
	
	EIMSK |= _BV(INT1); //enable INT1
	EICRA |= _BV(ISC11);  //falling edge interrupt
	
	EIMSK |= _BV(INT2); //enable INT2
	EICRA |= _BV(ISC21) | _BV(ISC20); //rising edge interrupt
	
	EIMSK |= _BV(INT3); //enable INT3
	EICRA |= _BV(ISC31); //falling edge interrupt
	
	ADCSRA |= _BV(ADEN) | _BV(ADIE);   // enable ADC and interrupt of ADC
	ADCSRA |= _BV(ADPS2) | _BV(ADPS0);//  //prescale by 1/32
	ADMUX |=_BV(REFS0);  // sets voltage reference selection 

	//External interupt control register
	//	EICRA &= ~_BV(ISC21) & ~_BV(ISC20); /* These lines would undo the above two lines */
	//	EICRA &= ~_BV(ISC31) & ~_BV(ISC30); /* Nice little trick */


	// See page 112 - EIFR External Interrupt Flags...notice how they reset on their own in 'C'...not in assembly
	//EIMSK |= 0x0C;

	// Enable all interrupts
	sei();	// Note this sets the Global Enable for all interrupts
}
//use pina7 for hall effect sensor
//have to reconfigure adc code (acquisition)
//dont use auto trigger mode
//only do adc when part is in front of sensor (start when it enters and stop when it leaves)


