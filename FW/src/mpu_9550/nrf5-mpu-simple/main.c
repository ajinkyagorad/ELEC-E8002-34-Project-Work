 /* 
  * This example is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#include "headers.h"
#include "SEGGER_RTT.h"


#define TIMER_INTERVAL_ACCEL_UPDATE     APP_TIMER_TICKS(5, APP_TIMER_PRESCALER) // 10 ms intervals


extern void on_adv_evt(ble_adv_evt_t ble_adv_evt);
extern void on_ble_evt(ble_evt_t * p_ble_evt);
extern void ble_evt_dispatch(ble_evt_t * p_ble_evt);
extern void sys_evt_dispatch(uint32_t sys_evt);
extern void bsp_event_handler(bsp_event_t event);
extern void ble_stack_init(void);
extern void gap_params_init(void);
extern void advertising_init(void);
extern void advertising_data_update(void);
extern void advertising_start(void);
extern void mpu_setup(void);
extern void uart_config(void);
extern void sig_init(void);
extern uint8_t sig_read_bpm(uint16_t tick);
extern uint8_t sig_calculate_bpm(uint8_t* ad_data);

ble_advdata_t advdata;
ble_advdata_manuf_data_t 				manuf_specific_data;
uint8_t ad_data[5];

bool bpm_update_flag = false;
bool start_btn_flag = false;
APP_TIMER_DEF(m_timer_accel_update_id);
extern accel_values_t accel_values[SAMPLE_SIZE];
extern float buff_x[SAMPLE_SIZE];
extern float buff_y[SAMPLE_SIZE];
extern float buff_z[SAMPLE_SIZE];
uint16_t tick;



/**@brief Function for starting timers.
*/
static void application_timers_start(void)
{
    uint32_t err_code;
    err_code = app_timer_start(m_timer_accel_update_id, TIMER_INTERVAL_ACCEL_UPDATE, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting timers.
*/
static void application_timers_stop(void)
{
    uint32_t err_code;
    err_code = app_timer_stop(m_timer_accel_update_id);
    APP_ERROR_CHECK(err_code);
}


/** 
* @brief timer event
*/
void timer_accel_update_handler(void * p_context)
{
	  if(tick >=SAMPLE_SIZE){
				application_timers_stop();
				bpm_update_flag = true;
		}else{
		    sig_read_bpm(tick++);
		}
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    uint32_t err_code;
    err_code = app_timer_create(&m_timer_accel_update_id, APP_TIMER_MODE_REPEATED, timer_accel_update_handler);
    APP_ERROR_CHECK(err_code);
}



/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void)
{
    uint32_t err_code;

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}









/**@brief Function for application main entry.
 */
int main(void)
{

    // Initialize.
	  uart_config();
    timers_init();
    ble_stack_init();
    gap_params_init();
    advertising_init();
    mpu_setup();
    sig_init();

    tick = 0;
	
    // Start execution.
    application_timers_start();
	  advertising_start();

	  printf(" start... \r\n");


    for (;;)
    {
        if(bpm_update_flag == true)
        {
	
				#ifdef INCLUDE_THIS
					  int ii = 0;
					  printf(" ---------------------------------------------------------\r\n");
						 for(ii = 0; ii < SAMPLE_SIZE; ii++){
							 printf("%d, %5f, %5f, %5f \r\n",ii, buff_x[ii], buff_y[ii], buff_z[ii]);
							 nrf_delay_ms(10);
						 }
						printf(" ---------------------------end----------------------------\r\n");
			
				#endif
					  
					  sig_calculate_bpm(ad_data); 
		
						advertising_data_update();

					  tick = 0;
					  application_timers_start();
            bpm_update_flag = false;
					  start_btn_flag = false;
        }
    }
}

/**
 * @}
 */
