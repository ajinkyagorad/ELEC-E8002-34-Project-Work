#include "headers.h"
uint8_t bpm;


/**@brief function initialize module parameters
 */
void sig_init(void){
  bpm=0;
	// add hear module parameters which u want to initialize on every reset 
}

/**@brief Function calculates the current beat per minute value 
 */
uint8_t sig_read_bpm(void){
 	
	if(++bpm >= 255) bpm = 0;
  return bpm;	

}


//add your functions in this file
