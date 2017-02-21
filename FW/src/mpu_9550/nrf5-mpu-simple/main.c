 /* 
  * This example is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#include <stdio.h>
#include "boards.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_mpu.h"



/*UART buffer size. */
#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 1

/**
 * @brief UART events handler.
 */
static void uart_events_handler(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}


/**
 * @brief UART initialization.
 * Just the usual way. Nothing special here
 */
static void uart_config(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_events_handler,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}



void mpu_setup(void)
{
    ret_code_t ret_code;
    // Initiate MPU driver
    ret_code = mpu_init();
    APP_ERROR_CHECK(ret_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 19;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    ret_code = mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(ret_code); // Check for errors in return value 
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{    
    uint32_t err_code;
	LEDS_CONFIGURE(LEDS_MASK);
	LEDS_OFF(LEDS_MASK);
    uart_config();
    printf("\033[2J\033[;HMPU simple example. Compiled @ %s\r\n", __TIME__);
    mpu_setup();
       
    

	
    accel_values_t acc_values;
    uint32_t sample_number = 0;
	  gyro_values_t gyro_values;
	  uint8_t address  = 0;
    while(1)
    {
        // Read accelerometer sensor values
       err_code = mpu_read_accel(&acc_values);
			 APP_ERROR_CHECK(err_code);
			
			 err_code = mpu_read_gyro(&gyro_values);
			 APP_ERROR_CHECK(err_code);
			 
			 err_code =  who_am_i(&address);
       APP_ERROR_CHECK(err_code);
        // Clear terminal and print values
			printf("\033[3;1H\r\nZ");
			printf("device address: %x\r\n",address);
      printf("acc sample # %d\r\nX: %06d\r\nY: %06d\r\nZ: %06d\r\n", ++sample_number, acc_values.x, acc_values.y, acc_values.z);
			printf("gyro sample # %d\r\nX: %06d\r\nY: %06d\r\nZ: %06d\r\n", ++sample_number, gyro_values.x, gyro_values.y, gyro_values.z);
			
		nrf_gpio_pin_toggle(LED_1);
        nrf_delay_ms(250);
    }
}

/** @} */

