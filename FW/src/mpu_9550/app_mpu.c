 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */
#include "headers.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_mpu.h"
#include "nrf_gpio.h"
#include "nrf_drv_mpu.h"
#include "nrf_error.h"
#include "nrf_drv_config.h"


void mpu_setup(void)
{
    ret_code_t ret_code;
    // Initiate MPU driver
    ret_code = mpu_init();
    APP_ERROR_CHECK(ret_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 4;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    ret_code = mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(ret_code); // Check for errors in return value 
}




uint32_t mpu_config(mpu_config_t * config)
{
    uint8_t *data;
    data = (uint8_t*)config;
    return nrf_drv_mpu_write_registers(MPU_REG_SMPLRT_DIV, data, 4);
}



uint32_t mpu_int_cfg_pin(mpu_int_pin_cfg_t *cfg)
{
    uint8_t *data;
    data = (uint8_t*)cfg;
    return nrf_drv_mpu_write_single_register(MPU_REG_INT_PIN_CFG, *data);
    
}



uint32_t mpu_int_enable(mpu_int_enable_t *cfg)
{
    uint8_t *data;
    data = (uint8_t*)cfg;
    return nrf_drv_mpu_write_single_register(MPU_REG_INT_ENABLE, *data);
}


#define AK8963_ADDRESS   0x0C
#define WHO_AM_I_AK8963  0x00 // should return 0x48
#define INFO             0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL1      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_CNTL2      0x0B
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L      0x14
#define YG_OFFSET_H      0x15
#define YG_OFFSET_L      0x16
#define ZG_OFFSET_H      0x17
#define ZG_OFFSET_L      0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E
#define WOM_THR          0x1F

uint32_t mpu_init(void)
{
    uint32_t err_code;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
	err_code = nrf_drv_mpu_init();
    if(err_code != NRF_SUCCESS) return err_code;

    uint8_t reset_value = 7; // Resets gyro, accelerometer and temperature sensor signal paths.
    err_code = nrf_drv_mpu_write_single_register(MPU_REG_SIGNAL_PATH_RESET, reset_value);
    if(err_code != NRF_SUCCESS) return err_code;

    // Chose  PLL with X axis gyroscope reference as clock source
    err_code = nrf_drv_mpu_write_single_register(MPU_REG_PWR_MGMT_1, 1);
    if(err_code != NRF_SUCCESS) return err_code;

	
		  uint8_t MPU_Init_Data[16][2] = {
        {0x80, MPU_REG_PWR_MGMT_1},     // Reset Device
        {0x01, MPU_REG_PWR_MGMT_1},     // Clock Source
        {0x00, MPU_REG_PWR_MGMT_2},     // Enable Acc & Gyro
        {CONFIG, MPU_REG_CONFIG},         // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
        {0x00, MPU_REG_GYRO_CONFIG},    // +-250dps
        {0x00, MPU_REG_ACCEL_CONFIG},   // +-2G
        {0x01, MPU_REG_ACCEL_CONFIG_2}, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
        {0x30, MPU_REG_INT_PIN_CFG},    //
        //{0x40, MPU_REG_I2C_MST_CTRL},   // I2C Speed 348 kHz
        //{0x20, MPU_REG_USER_CTRL},      // Enable AUX
        {0x20, MPU_REG_USER_CTRL},       // I2C Master mode
        {0x0D, MPU_REG_I2C_MST_CTRL}, //  I2C configuration multi-master  IIC 400KHz
        
        //{AK8963_I2C_ADDR, MPU_REG_I2C_SLV0_ADDR},  //Set the I2C slave addres of AK8963 and set for write.
        //{0x09, MPU_REG_I2C_SLV4_CTRL},
        //{0x81, MPU_REG_I2C_MST_DELAY_CTRL}, //Enable I2C delay
 
        {AK8963_CNTL2, MPU_REG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
        {0x01, MPU_REG_I2C_SLV0_DO}, // Reset AK8963
        {0x81, MPU_REG_I2C_SLV0_CTRL},  //Enable I2C and set 1 byte
 
        {AK8963_CNTL1, MPU_REG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
        {0x12, MPU_REG_I2C_SLV0_DO}, // Register value to continuous measurement in 16bit
        {0x81, MPU_REG_I2C_SLV0_CTRL}  //Enable I2C and set 1 byte
        
    };
			
		 for(int i=0; i<16; i++) {
        nrf_drv_mpu_write_single_register(MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
        
    }
	
    return NRF_SUCCESS;
}



uint32_t mpu_read_accel(accel_values_t * accel_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = nrf_drv_mpu_read_registers(MPU_REG_ACCEL_XOUT_H, raw_values, 6);
    if(err_code != NRF_SUCCESS) return err_code;

    // Reorganize read sensor values and put them into value struct
    uint8_t *data;
    data = (uint8_t*)accel_values;
    for(uint8_t i = 0; i<6; i++)
    {
        *data = raw_values[5-i];
        data++;
    }
    return NRF_SUCCESS;
}



uint32_t mpu_read_gyro(gyro_values_t * gyro_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = nrf_drv_mpu_read_registers(MPU_REG_GYRO_XOUT_H, raw_values, 6);
    if(err_code != NRF_SUCCESS) return err_code;

    // Reorganize read sensor values and put them into value struct
    uint8_t *data;
    data = (uint8_t*)gyro_values;
    for(uint8_t i = 0; i<6; i++)
    {
        *data = raw_values[5-i];
        data++;
    }
    return NRF_SUCCESS;
}



uint32_t mpu_read_temp(temp_value_t * temperature)
{
    uint32_t err_code;
    uint8_t raw_values[2];
    err_code = nrf_drv_mpu_read_registers(MPU_REG_TEMP_OUT_H, raw_values, 2);
    if(err_code != NRF_SUCCESS) return err_code;

    *temperature = (temp_value_t)(raw_values[0] << 8) + raw_values[1];

    return NRF_SUCCESS;
}


uint32_t who_am_i(uint8_t * address)
{
    uint32_t err_code;
    err_code = nrf_drv_mpu_read_registers(MPU_REG_WHO_AM_I, address, 1);
    if(err_code != NRF_SUCCESS) return err_code;

    return NRF_SUCCESS;
}

uint32_t mpu_read_int_source(uint8_t * int_source)
{
    return nrf_drv_mpu_read_registers(MPU_REG_INT_STATUS, int_source, 1);
}


// Function does not work on MPU60x0 and MPU9255
#if defined(MPU9150)
uint32_t mpu_config_ff_detection(uint16_t mg, uint8_t duration)
{
    uint32_t err_code;
    uint8_t threshold = (uint8_t)(mg/MPU_MG_PR_LSB_FF_THR);
    if(threshold > 255) return MPU_BAD_PARAMETER;

    err_code = nrf_drv_mpu_write_single_register(MPU_REG_FF_THR, threshold);
    if(err_code != NRF_SUCCESS) return err_code;

    return nrf_drv_mpu_write_single_register(MPU_REG_FF_DUR, duration);
}
#endif // defined(MPU9150)



/*********************************************************************************************************************
 * FUNCTIONS FOR MAGNETOMETER.
 * MPU9150 has an AK8975C and MPU9255 an AK8963 internal magnetometer. Their register maps
 * are similar, but AK8963 has adjustable resoultion (14 and 16 bits) while AK8975C has 13 bit resolution fixed. 
 */

#if defined(MPU9150) || defined(MPU9255) && (TWI_COUNT >= 1) // Magnetometer only works with TWI so check if TWI is enabled

uint32_t mpu_magnetometer_init(mpu_magn_config_t * p_magnetometer_conf)
{	
	uint32_t err_code;
	
	// Read out MPU configuration register
	mpu_int_pin_cfg_t bypass_config;
	err_code = nrf_drv_mpu_read_registers(MPU_REG_INT_PIN_CFG, (uint8_t *)&bypass_config, 1);
	
	// Set I2C bypass enable bit to be able to communicate with magnetometer via I2C
	bypass_config.i2c_bypass_en = 1;
	// Write config value back to MPU config register
	err_code = mpu_int_cfg_pin(&bypass_config);
	if (err_code != NRF_SUCCESS) return err_code;
	
	// Write magnetometer config data	
	uint8_t *data;
    data = (uint8_t*)p_magnetometer_conf;	
    return nrf_drv_mpu_write_magnetometer_register(MPU_AK89XX_REG_CNTL, *data);
}

uint32_t mpu_read_magnetometer(magn_values_t * p_magnetometer_values, mpu_magn_read_status_t * p_read_status)
{
	uint32_t err_code;
	err_code = nrf_drv_mpu_read_magnetometer_registers(MPU_AK89XX_REG_HXL, (uint8_t *)p_magnetometer_values, 6);
	if(err_code != NRF_SUCCESS) return err_code;
        
	/* Quote from datasheet: MPU_AK89XX_REG_ST2 register has a role as data reading end register, also. When any of measurement data register is read
	in continuous measurement mode or external trigger measurement mode, it means data reading start and
	taken as data reading until ST2 register is read. Therefore, when any of measurement data is read, be
	sure to read ST2 register at the end. */
	if(p_read_status == NULL)
	{
		// If p_read_status equals NULL perform dummy read
		uint8_t status_2_reg;
		err_code = nrf_drv_mpu_read_magnetometer_registers(MPU_AK89XX_REG_ST2, &status_2_reg, 1);
	}
	else
	{
		// If p_read_status NOT equals NULL read and return value of MPU_AK89XX_REG_ST2
		err_code = nrf_drv_mpu_read_magnetometer_registers(MPU_AK89XX_REG_ST2, (uint8_t *)p_read_status, 1);
	}
	return err_code;
}

// Test function for development purposes
uint32_t mpu_read_magnetometer_test(uint8_t reg, uint8_t * registers, uint8_t len)
{
    return nrf_drv_mpu_read_magnetometer_registers(reg, registers, len);
}

#endif // defined(MPU9150) || defined(MPU9255) && (TWI_COUNT >= 1) 

/**
  @}
*/
