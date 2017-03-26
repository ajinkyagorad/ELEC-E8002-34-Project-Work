 /* 
  * This example is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#include "headers.h"
#define BPM_OFFSET 	1
#define STRENGTH_OFFSET 	2
#define TIMER_INTERVAL_ACCEL_UPDATE     APP_TIMER_TICKS(10, APP_TIMER_PRESCALER) // 10 ms intervals


extern void on_adv_evt(ble_adv_evt_t ble_adv_evt);
extern void on_ble_evt(ble_evt_t * p_ble_evt);
extern void ble_evt_dispatch(ble_evt_t * p_ble_evt);
extern void sys_evt_dispatch(uint32_t sys_evt);
extern void ble_stack_init(void);
extern void gap_params_init(void);
extern void advertising_init(void);
extern void advertising_data_update(void);
extern void advertising_start(void);
extern void mpu_setup(void);
extern void uart_config(void);
extern void sig_init(void);
extern uint8_t sig_read_bpm(void);

ble_advdata_t advdata;
ble_advdata_manuf_data_t 				manuf_specific_data;
uint8_t ad_data[3];

bool start_accel_update_flag = false;
APP_TIMER_DEF(m_timer_accel_update_id);





/** 
* @brief timer event
*/
void timer_accel_update_handler(void * p_context)
{
    start_accel_update_flag = true;
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

/**@brief Function for starting timers.
*/
static void application_timers_start(void)
{
    uint32_t err_code;
    err_code = app_timer_start(m_timer_accel_update_id, TIMER_INTERVAL_ACCEL_UPDATE, NULL);
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

    LEDS_CONFIGURE(LEDS_MASK);
	  LEDS_OFF(LEDS_MASK);
    uart_config();


    // Initialize.
    timers_init();
    ble_stack_init();
    gap_params_init();
    advertising_init();
    mpu_setup();
    sig_init();

    // Start execution.
    application_timers_start();
	  advertising_start();
	
    for (;;)
    {
        if(start_accel_update_flag == true)
        {
            
					  ad_data[BPM_OFFSET] = sig_read_bpm(); 
					
					  advertising_data_update();

            start_accel_update_flag = false;
        }
    }
}

/**
 * @}
 */
