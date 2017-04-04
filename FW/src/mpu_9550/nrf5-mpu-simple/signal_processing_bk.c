#include "headers.h"
#include <math.h>

typedef struct{
  double magnitude;
	double index;
}pick_t;

uint8_t bpm;
double buff_x[SAMPLE_SIZE];
double buff_y[SAMPLE_SIZE];
double buff_z[SAMPLE_SIZE];
accel_values_t accel_values;


/**@brief function initialize module parameters
 */
void sig_init(void){
  bpm=0;
	
	// add hear module parameters which u want to initialize on every reset 
}

/**@brief Function calculates the current beat per minute value 
 */
uint8_t sig_read_bpm(uint16_t tick){
 	
	mpu_read_accel(&accel_values);
	buff_x[tick] = accel_values.x;
	buff_y[tick] = accel_values.y;
	buff_z[tick] = accel_values.z;

}

/**@brief Function calculates the current beat per minute value 
 */

void sig_butterworth_filter_4_to_11(double *data, double *result)
{
  memcpy(result,data,400U *sizeof(result));
}


/**@brief Function calculates the current beat per minute value 
 */

void sig_butterworth_filter_0_66_to_2_5(double *data, double *result)
{
  memcpy(result,data,400U *sizeof(result));
}

/**@brief Function calculates the current beat per minute value 
 */
void sig_resultant_acceleration(double *x,double *y,double *z, double *R){
  int ii = 0;  
	for(ii = 0; ii < SAMPLE_SIZE ; ii++)
	 R[ii] = sqrt(pow(z[ii],2) + pow(z[ii],2) + pow(z[ii],2));
} 


uint8_t sig_calculate_bpm(){
			
	if(++bpm >= 255) bpm = 0;
  return bpm;	
	
  double Xf[SAMPLE_SIZE];
	double Yf[SAMPLE_SIZE];
	double Zf[SAMPLE_SIZE];
	double R[SAMPLE_SIZE];
	memset(Xf,0,sizeof(Xf));
	memset(Yf,0,sizeof(Zf));
	memset(Zf,0,sizeof(Zf));

  sig_butterworth_filter_4_to_11(buff_x, Xf);
	sig_butterworth_filter_4_to_11(buff_y, Yf);
	sig_butterworth_filter_4_to_11(buff_z, Zf);
	
	sig_resultant_acceleration(Xf,Yf,Zf, R);
	
	double * filtered_signal = Xf;
  memset(filtered_signal,0,sizeof(Xf));	
	
	sig_butterworth_filter_0_66_to_2_5(R, filtered_signal);
	
	
	int ii,kk = 0, slop[SAMPLE_SIZE];
	pick_t pick[10]; 
	double val_previous = filtered_signal[0], val_current = 0;
	memset(slop,0,sizeof(slop));
	memset(pick,0,sizeof(pick));
	for(ii = 1; ii < SAMPLE_SIZE; ii++){
		val_current = filtered_signal[ii];
		
		slop[ii] = ((val_current-val_previous)>0)?1:-1;
    
		if(slop[ii]==-1 && slop[ii-1]==1){
			pick[kk].magnitude = val_previous;
 		  pick[kk].index = ii-1;
			++kk;
			ASSERT(kk < 10);
		}
		
		val_previous = val_current;
  }
	

	//TODO: check for local peacks hear
	  
	  if(pick[1].index == 0 && pick[0].index == 0)
			return -1;
		else
	    return (uint8_t)(60*200/(pick[1].index -  pick[0].index));
	
}








