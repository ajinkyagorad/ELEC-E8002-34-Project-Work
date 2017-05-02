#include "headers.h"
#include "SEGGER_RTT.h"
#include <math.h>

#define ACCEL
#ifdef ACCEL
//#define MIN_TH 1000
//#define MIN_TH 2.3e+6
//#define MIN_TH 400
//USE WITH _1ST_STAGE_FILTER_CLH
//#define MIN_TH 300000
//USE WITH _1ST_STAGE_FILTER_CLH
#define MIN_TH 10
#else
#define MIN_TH 0.5
#define MAX_TH 500
#endif
#define SCAN_WINDOW 6
#define MIN_SPECING 70
#define MAX_SPECING 255

extern uint16_t tick;

typedef struct{
  double magnitude;
	double index;
	int8_t type;
}pick_t;

uint8_t bpm;
int16_t buff_x[SAMPLE_SIZE];
int16_t buff_y[SAMPLE_SIZE];
int16_t buff_z[SAMPLE_SIZE];
#ifdef ACCEL
accel_values_t accel_values;
#else
gyro_values_t gyro_values;
#endif
 
/**@brief function initialize module parameters
 */
void sig_init(void){
  bpm=0;
	
	// add hear module parameters which u want to initialize on every reset 
}

/**@brief Function calculates the current beat per minute value 
 */
void sig_read_bpm(uint16_t tick){
	
	#ifdef ACCEL
	mpu_read_accel(&accel_values);
	buff_x[tick] = (accel_values.x);
	buff_y[tick] = (accel_values.y);
	buff_z[tick] = (accel_values.z);
	#else
	mpu_read_gyro(&gyro_values);
	buff_x[tick] = (gyro_values.x);
	buff_y[tick] = (gyro_values.y);
	buff_z[tick] = (gyro_values.z);
	#endif
	

}


unsigned char rtIsInf(double value)
{
  return ((value==1.79769313486231571e+308) ? 1U : 0U);
}

void filter(double *b, double *a, double *x, double *y)
{
  double a1;
  int k;
  double dbuffer[3];
  int j;
  a1 = a[0];
  if ((!((!rtIsInf(a[0])))) || (a[0] == 0.0) || (!(a[0] !=
        1.0))) {
  } else {
    for (k = 0; k < 3; k++) {
      b[k] /= a1;
    }

    for (k = 0; k < 2; k++) {
      a[k + 1] /= a1;
    }

    a[0] = 1.0;
  }

  for (k = 0; k < 2; k++) {
    dbuffer[k + 1] = 0.0;
  }

  for (j = 0; j < SAMPLE_SIZE; j++) {
    for (k = 0; k < 2; k++) {
      dbuffer[k] = dbuffer[k + 1];
    }

    dbuffer[2] = 0.0;
    for (k = 0; k < 3; k++) {
      dbuffer[k] += x[j] * b[k];
    }

    for (k = 0; k < 2; k++) {
      dbuffer[k + 1] -= dbuffer[0] * a[k + 1];
    }

    y[j] = dbuffer[0];
  }
}


#define _1ST_STAGE_FILTER_CLH_4_TO_11
#define _2ND_STAGE_FILTER_CLH_0_66_TO_2_5

#ifdef _1ST_STAGE_FILTER_4_TO_11
 //Referenced http://www-users.cs.york.ac.uk/~fisher/mkfilter/
void sig_butterworth_filter_4_to_11(double *data, double *result)
{
	double* x;
	double* y;
	x = data;y = result;
	for(int n=0;n<2;n++)y[n]=x[n];
	for(int n=2;n<SAMPLE_SIZE;n++){
		y[n] = (  1 * x[n- 2])
     + (  2 * x[n- 1])
     + (  1 * x[n- 0])

     + ( -0.6135227659 * y[n- 2])
     + (  1.5181325407 * y[n- 1]);
	}
	
	memcpy(x, y, SAMPLE_SIZE * sizeof(double));
	
	x = data;y = result;
  for(int n=0;n<2;n++)y[n]=x[n];
	for(int n=2;n<SAMPLE_SIZE;n++){
		y[n] = (  1 * x[n- 2])
     + ( -2 * x[n- 1])
     + (  1 * x[n- 0])

     + ( -0.8371816513 * y[n- 2])
     + (  1.8226949252 * y[n- 1]);
	}
}
#elif defined(_1ST_STAGE_FILTER_4_TO_22)
void sig_butterworth_filter_4_to_22(double *data, double *result)
{
	double* x;
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

#elif defined(_1ST_STAGE_FILTER_CLH_4_TO_11)

void sig_butterworth_filter_4_to_11(double *data, double *result)
{
  double dv0[3];
  double dv1[3];
  int i0;
  static const double dv2[3] = { 0.9149691441130825, -1.829938288226165,
    0.9149691441130825 };

  static const double dv3[3] = { 1.0, -1.8226949251963078, 0.8371816512560224 };


  static const double dv4[3] = { 0.023847556298910971, 0.047695112597821943,
    0.023847556298910971 };

  static const double dv5[3] = { 1.0, -1.518132540717628, 0.61352276591327193 };

  for (i0 = 0; i0 < 3; i0++) {
    dv0[i0] = dv2[i0];
    dv1[i0] = dv3[i0];
  }

  filter(dv0, dv1, data, result);
  for (i0 = 0; i0 < 3; i0++) {
    dv0[i0] = dv4[i0];
    dv1[i0] = dv5[i0];
  }

  memcpy(data, result, SAMPLE_SIZE * sizeof(double));
  filter(dv0, dv1, data, result);
}
#endif


#ifdef _2ND_STAGE_FILTER_0_66_TO_2_5
void sig_butterworth_filter_0_66_to_2_5(double*data, double *result)
{
		  double* x;
			double* y;
			x = data;y = result;
			for(int n=0;n<2;n++)y[n]=x[n];
			for(int n=2;n<SAMPLE_SIZE;n++){
			y[n] = (  1 * x[n- 2])
					 + (  2 * x[n- 1])
					 + (  1 * x[n- 0])

					 + ( -0.8948743446 * y[n- 2])
					 + (  1.8890330794 * y[n- 1]);
			}
			
			memcpy(x, y, SAMPLE_SIZE * sizeof(double));
			
			x = data;y = result;
			for(int n=0;n<2;n++)y[n]=x[n];
			for(int n=2;n<SAMPLE_SIZE;n++){
			y[n] = (  1 * x[n- 2])
					 + ( -2 * x[n- 1])
					 + (  1 * x[n- 0])

					 + ( -0.9780305085 * y[n- 2])
					 + (  1.9777864838 * y[n- 1]);
			}
}

#elif defined(_2ND_STAGE_FILTER_CLH_0_66_TO_2_5)

void sig_butterworth_filter_0_66_to_2_5(double *data, double *result)
{
  double dv0[3];
  double dv1[3];
  int i0;
  static const double dv2[3] = { 0.98544543731661882, -1.9708908746332376,
    0.98544543731661882 };

  static const double dv3[3] = { 1.0, -1.9706790281185969, 0.97110272114787866 };


  static const double dv4[3] = { 0.0014603163055277313, 0.0029206326110554626,
    0.0014603163055277313 };

  static const double dv5[3] = { 1.0, -1.8890330793945247, 0.89487434461663562 };

  for (i0 = 0; i0 < 3; i0++) {
    dv0[i0] = dv2[i0];
    dv1[i0] = dv3[i0];
  }

  filter(dv0, dv1, data, result);
  for (i0 = 0; i0 < 3; i0++) {
    dv0[i0] = dv4[i0];
    dv1[i0] = dv5[i0];
  }

  memcpy(data, result, SAMPLE_SIZE * sizeof(double));
  filter(dv0, dv1, data, result);
}

#endif
void sig_dot_x_acceleration(double *x, double *R){
  int ii = 0;  
	for(ii = 0; ii < SAMPLE_SIZE ; ii++)
	 R[ii] += x[ii]*x[ii];
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
    nrf_delay_ms(5);
	}
}
void sig_print_all(double * R){
int ii;
	for(ii = 0;ii<SAMPLE_SIZE;ii++){
		//printf("%d, %d,%d,%d,%.5f\r\n",0,(int)buff_x[ii], (int)buff_y[ii],(int)buff_z[ii],R[ii]);
		printf("%.5f\r\n",R[ii]);
		nrf_delay_ms(5);
	}
}
double sig_get_mean(double* vector,uint16_t SIZE)
{
    uint16_t cc;
    long double result=0;

    for(cc = 0;cc<SIZE;cc++)
        result += *(vector+cc);
    return  result/SIZE;
}

double sig_get_mean_wr(int16_t* data, double* vector,uint16_t SIZE)
{
    uint16_t cc;
    long double result=0;

    for(cc = 0;cc<SIZE;cc++){
				vector[cc] = (double)data[cc];
        result += *(vector+cc);
		}
    return  result/SIZE;
}
  double Xf[SAMPLE_SIZE];
	double R[SAMPLE_SIZE];
uint8_t sig_calculate_bpm(uint8_t* ad_data){
	
	int ii;		

	memset(Xf,0,sizeof(Xf));
  memset(R,0,sizeof(R));
	
	
	double mean = sig_get_mean_wr(buff_x,R,SAMPLE_SIZE);
	for(ii=0;ii < SAMPLE_SIZE; ii++)
		R[ii] = R[ii]- mean;
	#if defined( _1ST_STAGE_FILTER_4_TO_11) || defined(_1ST_STAGE_FILTER_CLH_4_TO_11)
  sig_butterworth_filter_4_to_11(R, Xf);
	#elif defined(_1ST_STAGE_FILTER_4_TO_22)
	sig_butterworth_filter_4_to_22(R, Xf);
	#else
	#error "choose filter ";
	#endif
	memset(R,0,sizeof(R));
	sig_dot_x_acceleration(Xf,R);
  memset(Xf,0,sizeof(Xf));
	
	mean = sig_get_mean_wr(buff_y,R,SAMPLE_SIZE);
	for(ii=0;ii < SAMPLE_SIZE; ii++)
		R[ii] = R[ii]- mean;
	#if defined( _1ST_STAGE_FILTER_4_TO_11) || defined(_1ST_STAGE_FILTER_CLH_4_TO_11)
  sig_butterworth_filter_4_to_11(R, Xf);
	#elif defined(_1ST_STAGE_FILTER_4_TO_22)
	sig_butterworth_filter_4_to_22(R, Xf);
	#else
	#error "choose filter ";
	#endif
	memset(R,0,sizeof(R));
	sig_dot_x_acceleration(Xf,R);
  memset(Xf,0,sizeof(Xf));
	
  mean = sig_get_mean_wr(buff_z,R,SAMPLE_SIZE);
	for(ii=0;ii < SAMPLE_SIZE; ii++)
		R[ii] = R[ii]- mean;
	#if defined( _1ST_STAGE_FILTER_4_TO_11) || defined(_1ST_STAGE_FILTER_CLH_4_TO_11)
  sig_butterworth_filter_4_to_11(R, Xf);
	#elif defined(_1ST_STAGE_FILTER_4_TO_22)
	sig_butterworth_filter_4_to_22(R, Xf);
	#else
	#error "choose filter ";
	#endif
	memset(R,0,sizeof(R));
	sig_dot_x_acceleration(Xf,R);
  memset(Xf,0,sizeof(Xf));

	sig_resultant_acceleration(R);


	memset(Xf,0,sizeof(Xf));
	double * filtered_signal = Xf;
	mean = sig_get_mean(R,SAMPLE_SIZE);
	for(ii=0;ii < SAMPLE_SIZE; ii++)
		R[ii] = R[ii] - mean;
	sig_butterworth_filter_0_66_to_2_5(R, filtered_signal);
  sig_print_all(filtered_signal);
  
		
	pick_t pick[10]; 
	memset(pick,0,sizeof(pick));
	
	int16_t kk = 0;
	int8_t slop[SAMPLE_SIZE];
	memset(slop,0,sizeof(slop));
	double val_previous = filtered_signal[0], val_current = 0, previous_index = 0;
 
	for(ii = 0; ii < SAMPLE_SIZE; ii++){
		 if (filtered_signal[ii] <  MIN_TH) continue;
     if ((ii - previous_index)!=1){
		      previous_index = ii;
			    val_previous   = filtered_signal[ii]; 
			    continue;
		 }
		 previous_index = ii;
		 val_current 		= filtered_signal[ii];
	  
		 slop[ii] = ((val_current-val_previous)>0)?1:(((val_current-val_previous)<0)?-1:0);
    
		 if( slop[ii]<=0 && slop[ii-1] > 0){
			if(ii == 1) continue;
			
			pick[kk].index = ii;
			pick[kk].magnitude = val_current;
			pick[kk].type = 1;
			 
			if(++kk >= 6)break;
			
		}
		
		val_previous = val_current;
  }
	
	
	

	for(ii = 0; ii < kk; ii++){
		  uint16_t cnt_slop_m = 0;
			uint16_t cnt_slop_p = 0;
			uint16_t index = pick[ii].index;
			int jj;
			for(jj = 0; jj < SCAN_WINDOW; jj++){   
					if(index + jj < SAMPLE_SIZE)if(slop[index+jj] > 0)cnt_slop_m++;
					if(index - jj >= 0)if(slop[index-jj] < 0)cnt_slop_p++;
			}
							
			if(cnt_slop_m  > 3 
				|| cnt_slop_p  > 3
				||((ii<kk-1)
			  &&(pick[ii].magnitude <  pick[ii+1].magnitude)
			  &&((pick[ii+1].index - pick[ii].index) < MIN_SPECING ))
			  ){
				 for(jj = ii; jj < kk-1; jj++){
						 pick[jj].index = pick[jj+1].index ;
						 pick[jj].magnitude =  pick[jj+1].magnitude;
						 pick[jj].type =  pick[jj].type;
				 }
						 kk = jj-1;
			 }
	}
							 

	
	  int16_t  current_maxima = 0;
    int16_t  previous_maxima = -1;
    uint16_t count_diff = 0;
    
    int16_t difference[5];
	  double amp = 0;
	  double cnt_amp = 0;

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
								    amp -= pick[previous_maxima].magnitude;
								    amp += pick[current_maxima].magnitude;
                    previous_maxima = current_maxima;
               }
             continue;
					 }
					 
				      amp += pick[current_maxima].magnitude;
              cnt_amp ++;					 
             
           difference[count_diff] = pick[current_maxima].index - pick[previous_maxima].index;
           count_diff = count_diff + 1;
            
           previous_maxima = current_maxima;  
				 }
				 else if (pick[ii].type == -1){
            
             ASSERT(pick[ii].type > 0)
				 }
        
        
			 }
    
	  *(ad_data + STRENGTH_OFFSET) = (uint8_t)(amp/cnt_amp);
			 
		uint8_t cnt = 0;
    double gap = 0;
    for (ii = 0; ii < count_diff; ii++){
			    
          if(difference[ii] > 0){
						 *(ad_data + GAP_OFFSET + cnt) = difference[ii];
             gap += difference[ii]; cnt++; 
						if(cnt > 2)break;
          }
		}
    if(difference[count_diff-1] < 70)
			ad_data[0] = 0;
		else
		  ad_data[0] = difference[count_diff-1];
	
		if(difference[count_diff-2] < 70)
			ad_data[1] = 0;
		else
		  ad_data[1] = difference[count_diff-2];
		
		if(difference[count_diff-3] < 70)
			ad_data[2] = 0;
		else
		  ad_data[2] = difference[count_diff-3];	
		
		if((ad_data[0]==0 && ad_data[1]== 0) 
			|| (ad_data[1] == 0 && ad_data[2] == 0))
					memset(ad_data,0,5);

	
    return (count_diff > 0 && gap > 0)?round((cnt*60* 200)/(gap)):0xff;
	
}
