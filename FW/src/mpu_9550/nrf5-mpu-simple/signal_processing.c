#include "headers.h"
#include <math.h>

typedef struct{
  double magnitude;
	double index;
}pick_t;

uint8_t bpm;
//double buff_x[SAMPLE_SIZE];
//double buff_y[SAMPLE_SIZE];
double buff_z[SAMPLE_SIZE];
//double gyro_x[SAMPLE_SIZE];
double gyro_y[SAMPLE_SIZE];
//double gyro_z[SAMPLE_SIZE];

//double Xf[SAMPLE_SIZE];
//double Yf[SAMPLE_SIZE];
double Zf[SAMPLE_SIZE];
//double Xwf[SAMPLE_SIZE];
double Ywf[SAMPLE_SIZE];
//double Zwf[SAMPLE_SIZE];
double R[SAMPLE_SIZE];
accel_values_t accel_values;
gyro_values_t gyro_values;

/**@brief function initialize module parameters
 */
void sig_init(void){
  bpm=0;
	
	// add hear module parameters which u want to initialize on every reset 
}

/**@brief Function calculates the current beat per minute value 
 */
void sig_read_bpm(uint16_t tick){
 	
	mpu_read_accel(&accel_values);
	mpu_read_gyro(&gyro_values);
	//gyro_x[tick] = gyro_values.x;
	gyro_y[tick] = gyro_values.y;
	//gyro_z[tick] = gyro_values.z;
	
	//buff_x[tick] = accel_values.x;
	//buff_y[tick] = accel_values.y;
	buff_z[tick] = accel_values.z;

}

// Referenced http://www-users.cs.york.ac.uk/~fisher/mkfilter/
void sig_butterworth_filter_4_to_11(double *data, double *result)
{
  //memcpy(result,data,400U *sizeof(result));
	//memset(&result,0,sizeof(result));
	double* x,*y;
	x = data;y = result;
	for(int n=0;n<4;n++)y[n]=x[n];
	for(int n=4;n<SAMPLE_SIZE;n++){
		 y[n] = (  1 * x[n- 4])     
					+ (  0 * x[n- 3])     
					+ ( -2 * x[n- 2])     
					+ (  0 * x[n- 1])
					+ (  1 * x[n- 0])

				 + ( -0.7327260304 * y[n- 4])
				 + (  3.0878833813 * y[n- 3])
				 + ( -4.9675926050 * y[n- 2])
				 + (  3.6108197545 * y[n- 1]);
			 }
}

void sig_butterworth_filter_0_66_to_2_5(double *data, double *result)
{
  //memcpy(result,data,400U *sizeof(result));
	//memset(&result,0,sizeof(result));
	double* x,*y;
	x = data;y = result;
	for(int n=0;n<4;n++)y[n]=x[n];
	for(int n=4;n<SAMPLE_SIZE;n++){
		y[n] = (  1 * x[n- 4])
				 + (  0 * x[n- 3])
				 + ( -2 * x[n- 2])
				 + (  0 * x[n- 1])
				 + (  1 * x[n- 0])

				 + ( -0.9215032056 * y[n- 4])
				 + (  3.7582371567 * y[n- 3])
				 + ( -5.7518393940 * y[n- 2])
				 + (  3.9151028964 * y[n- 1]);
			 }
	
	
}

void sig_resultant_acceleration(double *x,double *y,double *z, double *R){
  int ii = 0;  
	for(ii = 0; ii < SAMPLE_SIZE ; ii++)
	 R[ii] = sqrt(pow(x[ii],2) + pow(y[ii],2) + pow(z[ii],2));
} 
void sig_resultant(double*z,double*wy,double*R){
	int ii = 0;  
	double a=1,w=1;
	for(ii = 0; ii < SAMPLE_SIZE ; ii++)
	 R[ii] = sqrt(a*pow(z[ii],2) + w*pow(wy[ii],2) );
}

uint8_t sig_calculate_bpm(void){
			
	//if(++bpm >= 255) bpm = 0;
  //return bpm;	
	
  
	//memset(Xf,0,sizeof(Xf));
	//memset(Yf,0,sizeof(Zf));
	memset(Zf,0,sizeof(Zf));
	memset(Ywf,0,sizeof(Ywf));
  //sig_butterworth_filter_4_to_11(buff_x, Xf);
	//sig_butterworth_filter_4_to_11(buff_y, Yf);
	sig_butterworth_filter_4_to_11(buff_z, Zf);
	//sig_butterworth_filter_4_to_11(gyro_x, Xf);
	sig_butterworth_filter_4_to_11(gyro_y, Ywf);
	//sig_butterworth_filter_4_to_11(gyro_z, Zf);
	
	//sig_resultant_acceleration(Xf,Yf,Zf, R);
	sig_resultant(Zf,Ywf,R);
	
	double * filtered_signal = Zf;		// using old variable 
  memset(filtered_signal,0,sizeof(Zf));	
	
	sig_butterworth_filter_0_66_to_2_5(R, filtered_signal);
	
	
	int ii,kk = 0, slop[SAMPLE_SIZE];
	pick_t pick[10]; 
	double val_previous = filtered_signal[0], val_current = 0;
	memset(slop,0,sizeof(slop));
	memset(pick,0,sizeof(pick));
	for(ii = 1; ii < SAMPLE_SIZE; ii++){
		val_current = filtered_signal[ii];
		//printf("%.2f\r\n",val_current);
		slop[ii] = ((val_current-val_previous)>0)?1:-1;
    
		if(slop[ii]==-1 && slop[ii-1]==1){
			pick[kk].magnitude = val_previous;
 		  pick[kk].index = ii-1;
			++kk;
			ASSERT(kk < 10);
		}
		
		val_previous = val_current;
  }
	// for debugging (comment out if not required)
	for(int ii=0;ii<SAMPLE_SIZE;ii++){
		printf("%.5f\r\n",filtered_signal[ii]);
		
		nrf_delay_ms(1);
		}
	

	//TODO: check for local peacks hear
	  
	  if(pick[1].index == 0 && pick[0].index == 0)
			return 0;
		else
	    return (uint8_t)(60*200/(pick[1].index -  pick[0].index));
	
}








