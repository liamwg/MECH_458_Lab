/* Solution Set for the LinkedQueue.c */
/* 	
	Course		: UVic Mechatronics 458
	Milestone	: 3
	Title		: Data structures for MCUs and the Linked Queue Library

	Name 1:	Liam Welgan-Gunn		Student ID: V00889715
	Name 2:	Marcus Lau				Student ID: V00884155
	
	Description: You can change the following after you read it.  Lab3 Demo
	
	This main routine will only serve as a testing routine for now. At some point you can comment out
	The main routine, and can use the following library of functions in your other applications

	To do this...make sure both the .C file and the .H file are in the same directory as the .C file
	with the MAIN routine (this will make it more convenient)
*/

/* include libraries */
#include <stdlib.h>
#include <avr/io.h>
#include "LinkedQueue.h" 	/* This is the attached header file, which cleans things up */
							/* Make sure you read it!!! */
/* global variables */
/* Avoid using these */

/* main routine 
   You need to add the mtimer function to this project.    */

int main(){	
	
	link *head;			/* The ptr to the head of the queue */
	link *tail;			/* The ptr to the tail of the queue */
	link *newLink;		/* A ptr to a link aggregate data type (struct) */
	link *rtnLink;		/* same as the above */
	element eTest;		/* A variable to hold the aggregate data type known as element */

	//TIMER CODE
	CLKPR = 0x80;
	CLKPR = 0x01;
	TCCR1B |= _BV(CS11);

	DDRA = 0x00; // sets Port A to input
	DDRC = 0xFF; // sets Port C to output

while (1){

	rtnLink = NULL;
	newLink = NULL;

	setup(&head, &tail);

	while((PINA & 0x04) == 0x04); //check for switch being pressed
	
	mTimer(20); // debounce timer
	
	initLink(&newLink);
	newLink->e.stage = (PINA & 0x03);
	enqueue(&head, &tail, &newLink);
	while((PINA & 0x04) == 0x00); //check for switch being not pressed

	
	//getting second input
	while((PINA & 0x04) == 0x04); //check for switch being pressed
		
	mTimer(20); // debounce timer
		
	initLink(&newLink);
	newLink->e.stage = (PINA & 0x03);
	enqueue(&head, &tail, &newLink);
	while((PINA & 0x04) == 0x00); //check for switch being not pressed
	
	//getting third input
	while((PINA & 0x04) == 0x04); //check for switch being pressed
	
	mTimer(20); // debounce timer
	
	initLink(&newLink);
	newLink->e.stage = (PINA & 0x03);
	enqueue(&head, &tail, &newLink);
	while((PINA & 0x04) == 0x00); //check for switch being not pressed
	
	//getting fourth input
	while((PINA & 0x04) == 0x04); //check for switch being pressed
	
	mTimer(20); // debounce timer
	
	initLink(&newLink);
	newLink->e.stage = (PINA & 0x03);
	enqueue(&head, &tail, &newLink);
	while((PINA & 0x04) == 0x00); //check for switch being not pressed
	//done collecting values
	
	//dequeue first link
	dequeue(&head, &rtnLink); /* remove the item at the head of the list */
	free(rtnLink);
	
	dequeue(&head, &rtnLink); /* remove the item at the head of the list */
	PORTC |= rtnLink->e.stage;
	free(rtnLink);
	mTimer(1000);
		
	dequeue(&head, &rtnLink); /* remove the item at the head of the list */
	PORTC |= (rtnLink->e.stage) <<2 ;
	free(rtnLink);
	mTimer(1000);
			
	dequeue(&head, &rtnLink); /* remove the item at the head of the list */
	PORTC |= (rtnLink->e.stage) << 4;
	free(rtnLink);
	mTimer(1000);
	
	mTimer(5000);
	PORTC = 0x00;	

}

return (0);

}/* main */

/*initLink(&newLink);
newLink->e.itemCode = 3;
newLink->e.stage = 4;
enqueue(&head, &tail, &newLink);
PORTC = head->e.itemCode; */ 


/**************************************************************************************/
/***************************** SUBROUTINES ********************************************/
/**************************************************************************************/

int debug (char input){
	switch (input){
		case (0x01):
			PORTC = 0b00000001;
			break;
		case (0x02):
			PORTC = 0b00000010;
			break;
		case (0x04):
			PORTC = 0b00000100;
			break;
		case (0x08):
			PORTC = 0b00001000;
			break;
		default:
			PORTC = 0b00000000;
			break;
	}
}

void mTimer(int count){
	/* this system clock is 8MHz, the timer runs on the CPU clock which is a function of the system clock.
	The system clock has been pre-scaled by 2. 
	*/
	
	int i; //keeps track of loop number
	
	i = 0; //initializes loop counter
	
	//Set the Waveform Generation Mode bit description to Clear Timer on Compare Math mode (CTC) only
	
	TCCR1B |=_BV(WGM12);		//Set WGM bits to 0100, page 145
							//note WGM is spread over two register
							
	OCR1A = 0x03E8; //Sets output compare register for 1000 cycles = 1ms
	
	TCNT1 = 0x0000; //Sets initial value of timer counter to 0x0000
	
	TIMSK1 = TIMSK1 | 0b00000010; //Enable the output compare interrupt
	
	TIFR1 |= _BV(OCF1A); //clear timer interrupt flag and begin new timing
	
	//Poll the timer to determine when the timer has reached 0x03E8
	
	while(i<count){
		
		if((TIFR1 & 0x02) == 0x02){
			
			TIFR1 |= _BV(OCF1A); // clear interrupt flag by writing a ONE to the bit
			
			i++; //increment loop number
						
		} // end if
	} // end while
	
	return;
	
}// mTimer	


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
void dequeue(link **h, link **deQueuedLink){
	/* ENTER YOUR CODE HERE */
	*deQueuedLink = *h;	// Will set to NULL if Head points to NULL
	/* Ensure it is not an empty queue */
	if (*h != NULL){
		*h = (*h)->next;
	}/*if*/
	
	return;
}/*dequeue*/




/**************************************************************************************
* DESC: Peeks at the first element in the list
* INPUT: The head pointer
* RETURNS: The element contained within the queue
*/
/* This simply allows you to peek at the head element of the queue and returns a NULL pointer if empty */
element firstValue(link **h){
	return((*h)->e);
}/*firstValue*/





/**************************************************************************************
* DESC: deallocates (frees) all the memory consumed by the Queue
* INPUT: the pointers to the head and the tail
*/
/* This clears the queue */
void clearQueue(link **h, link **t){

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





/**************************************************************************************
* DESC: Obtains the number of links in the queue
* INPUT: The head and tail pointer
* RETURNS: An integer with the number of links in the queue
*/
/* returns the size of the queue*/


