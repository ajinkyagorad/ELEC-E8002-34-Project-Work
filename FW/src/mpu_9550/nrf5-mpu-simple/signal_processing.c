#include "headers.h"
#include "SEGGER_RTT.h"
#include <math.h>

#define MIN_TH 1e-3
#define MAX_TH 6e-3
#define MAX_SLOP_SCAN 6
#define MIN_SPECING 40
#define MAX_SPECING 255
 
typedef struct{
  double magnitude;
	double index;
	int8_t type;
}pick_t;

uint8_t bpm;
float buff_x[SAMPLE_SIZE];
float buff_y[SAMPLE_SIZE];
float buff_z[SAMPLE_SIZE];
gyro_values_t accel_values;

/**@brief function initialize module parameters
 */
void sig_init(void){
  bpm=0;
	
	// add hear module parameters which u want to initialize on every reset 
}

/**@brief Function calculates the current beat per minute value 
 */
void sig_read_bpm(uint16_t tick){
	
	mpu_read_gyro(&accel_values);
	buff_x[tick] = ((float)accel_values.x);///16384.0f;
	buff_y[tick] = ((float)accel_values.y);///16384.0f;
	buff_z[tick] = ((float)accel_values.z);///16384.0f;
	

}

// Referenced http://www-users.cs.york.ac.uk/~fisher/mkfilter/
/*void sig_butterworth_filter_4_to_11(float *data, double *result)
{
	float* x;
	double* y;
	x = data;y = result;
	for(int n=0;n<4;n++)y[n]=(double)x[n];
	for(int n=4;n<SAMPLE_SIZE;n++){
		 y[n] = (  1 * (double)x[n- 4])     
					+ (  0 * (double)x[n- 3])     
					+ ( -2 * (double)x[n- 2])     
					+ (  0 * (double)x[n- 1])
					+ (  1 * (double)x[n- 0])

				 + ( -0.7327260304 * y[n- 4])
				 + (  3.0878833813 * y[n- 3])
				 + ( -4.9675926050 * y[n- 2])
				 + (  3.6108197545 * y[n- 1]);
			 }
}
*/
void sig_butterworth_filter_4_to_11(float *X, double *Y)
{
	int j,k;
	double dbuffer[5]={0,0,0,0,0};
	static const double dv0[5] = { 0.0020805671354598072, 0.0,
    -0.0041611342709196144, 0.0, 0.0020805671354598072 };
	static const double dv1[5] = { 1.0, -3.8478114968688084, 5.5721601378375674,
    -3.5994720753697242, 0.87521454825368439 };

		 for (k = 0; k < 4; k++) {
    dbuffer[k + 1] = 0.0;
  }

  for (j = 0; j < SAMPLE_SIZE; j++) {
    for (k = 0; k < 4; k++) {
      dbuffer[k] = dbuffer[k + 1];
    }

    dbuffer[4] = 0.0;
    for (k = 0; k < 5; k++) {
      dbuffer[k] += X[j] * dv0[k];
    }

    for (k = 0; k < 4; k++) {
      dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
    }

    Y[j] = dbuffer[0];
  }
}
void sig_butterworth_filter_0_66_to_2_5(double*X, double *Y)
{
	int k,j;
	double dbuffer[5];
    static const double dv0[5] = { 0.0008023531890466889, 0.0,
    -0.0016047063780933778, 0.0, 0.0008023531890466889 };

  static const double dv1[5] = { 1.0, -3.9151028964225354, 5.7518393939858576,
    -3.7582371566656905, 0.9215032055789143 };

  for (k = 0; k < 4; k++) {
    dbuffer[k + 1] = 0.0;
  }

  for (j = 0; j < 402; j++) {
    for (k = 0; k < 4; k++) {
      dbuffer[k] = dbuffer[k + 1];
    }

    dbuffer[4] = 0.0;
    for (k = 0; k < 5; k++) {
      dbuffer[k] += X[j] * dv0[k];
    }

    for (k = 0; k < 4; k++) {
      dbuffer[k + 1] -= dbuffer[0] * dv1[k + 1];
    }

    Y[j] = dbuffer[0];
  }
}
/*
void sig_butterworth_filter_0_66_to_2_5(double*data, double *result)
{
  //memcpy(result,data,400U *sizeof(result));
	//memset(&result,0,sizeof(result));

	double * x, * y;
	x = data;y = result;
	for(int n=0;n<4;n++)y[n]=(double)x[n];
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
	
	
}*/
void sig_dot_x_acceleration(double *x, double *R){
  int ii = 0;  
	for(ii = 0; ii < SAMPLE_SIZE ; ii++)
	 R[ii] += pow(x[ii],2);
} 

void sig_resultant_acceleration(double *R){
  int ii = 0;  
	for(ii = 0; ii < SAMPLE_SIZE ; ii++){
	 R[ii] = sqrt(R[ii]);
	}
} 
void sig_resultant(double*z,double*wy,double*R){
	int ii = 0;  
	double a=1,w=1;
	for(ii = 0; ii < SAMPLE_SIZE ; ii++)
	 R[ii] = sqrt(a*pow(z[ii],2) + w*pow(wy[ii],2) );
}

double sig_get_max_th(pick_t *picks, uint8_t len){
	
	 double maximum = 0;
	 double cnt_max = 0;
	 uint8_t ii;
   for(ii = 0; ii < len;ii++)
		  if(picks[ii].type == 1){
			  maximum +=picks[ii].magnitude;
				cnt_max++;
			}
		return (double)((maximum/cnt_max)*0.6);
}


double sig_get_min_th(pick_t *picks, uint8_t len){
	
	 double minimum = 0;
	 double cnt_min = 0;
	 uint8_t ii;
   for(ii = 0; ii < len;ii++)
		  if(picks[ii].type == -1){
			  minimum +=picks[ii].magnitude;
				cnt_min++;
			}
	 return (double)((minimum/cnt_min)*0.6);
}

void sig_print(double *signal, int len){
int ii;
	for(ii = 0; ii < len; ii++){
		printf("%.2f\r\n",(float)signal[ii]);
    nrf_delay_ms(10);
	}
}


uint8_t sig_calculate_bpm(uint8_t* ad_data){
	
	int ii;		
  double Xf[SAMPLE_SIZE];
	double R[SAMPLE_SIZE];
	memset(Xf,0,sizeof(Xf));
  memset(R,0,sizeof(R));
	
	
  sig_butterworth_filter_4_to_11(buff_x, Xf);
	sig_dot_x_acceleration(Xf,R);

  memset(Xf,0,sizeof(Xf));
	sig_butterworth_filter_4_to_11(buff_y, Xf);
	sig_dot_x_acceleration(Xf,R);

  memset(Xf,0,sizeof(Xf));
	sig_butterworth_filter_4_to_11(buff_z, Xf);
	sig_dot_x_acceleration(Xf,R);

	sig_resultant_acceleration(R);
	

	memset(Xf,0,sizeof(Xf));
	double * filtered_signal = Xf;

	sig_butterworth_filter_0_66_to_2_5(R, filtered_signal);
  sig_print(filtered_signal, SAMPLE_SIZE);

	pick_t pick[10]; 
	memset(pick,0,sizeof(pick));
	
	int16_t kk = 0;
	int8_t slop[SAMPLE_SIZE];
	memset(slop,0,sizeof(slop));
	double val_previous = filtered_signal[0], val_current = 0;
 
	for(ii = 0; ii < SAMPLE_SIZE; ii++){
		 if (filtered_signal[ii] <  MIN_TH || filtered_signal[ii] >  MAX_TH ) continue;
    
		 val_current = filtered_signal[ii];
	  
		 slop[ii] = ((val_current-val_previous)>0)?1:(((val_current-val_previous)<0)?-1:0);
    
		 if(slop[ii]==0 || slop[ii]*slop[ii-1] < 0){
			if(ii == 1) continue;
			
			pick[kk].index = ii;
			
			if(slop[ii-1] > 0){
			 pick[kk].magnitude = val_current;
			 pick[kk].type = 1;
			}else if(slop[ii-1] < 0){
			 pick[kk].magnitude = val_current;
			 pick[kk].type = -1;
			}else continue;
			if(++kk >= 8)break;
			
		}
		
		val_previous = val_current;
  }
	
	
	
	uint16_t cnt_slop = 0;
	for(ii = 0; ii < kk; ii++){
			uint16_t index = pick[ii].index;
			int jj;
			for(jj = 0; jj < MAX_SLOP_SCAN; jj++)   
					if(index + jj < SAMPLE_SIZE)if(slop[index+jj] > 0)cnt_slop++;
							
			if(cnt_slop  > 1){
				 for(jj = ii; jj < kk-1; jj++){
						 pick[jj].index = pick[jj+1].index ;
						 pick[jj].magnitude =  pick[jj+1].magnitude;
						 pick[jj].type =  pick[jj].type;
				 }
						 kk = jj-1;
						 cnt_slop = 0;
			 }
	}
							 

	
	  int16_t  current_maxima = 0;
    int16_t  previous_maxima = -1;
    uint16_t count_diff = 0;
    
    int16_t difference[10];

	  memset(difference,0,sizeof(difference));
	  
		
	    for(ii = 0; ii < kk; ii++){
        if(pick[ii].type == 1){
           if(previous_maxima == -1){
               previous_maxima = ii;
               current_maxima = ii;
               continue;
					 }
           
           current_maxima  = ii;
           
           if((pick[current_maxima].index - pick[previous_maxima].index) < MIN_SPECING){
               if(pick[previous_maxima].type ==1 && pick[current_maxima].magnitude > pick[previous_maxima].magnitude && difference[count_diff-1]){
                    difference[count_diff-1] += pick[current_maxima].index - pick[previous_maxima].index;
                    previous_maxima = current_maxima;
               }
             continue;
					 }
					 
					 if((pick[current_maxima].index - pick[previous_maxima].index) > MAX_SPECING){
						 //TODO: not sure about this
						    previous_maxima = current_maxima;
             continue;
					 }
             
           difference[count_diff] = pick[current_maxima].index - pick[previous_maxima].index;
           count_diff = count_diff + 1;
            
           previous_maxima = current_maxima;  
				 }
				 else if (pick[ii].type == -1){
            
             ASSERT(pick[ii].type > 1)
				 }
        
        
			 }
    
			 
		uint8_t cnt = 0;
    double gap = 0;
    for (ii = 0; ii < count_diff; ii++){
			    
          if(difference[ii] > 0){
						 *(ad_data + GAP_OFFSET + cnt) = difference[ii];
             gap += difference[ii]; cnt++; 
						if(cnt > 2)break;
          }
		}
		
    return (count_diff > 0 && gap > 0)?round((cnt*60* 200)/(gap)):0xff;
	
}

















