//
// Created by krist on 29/11/2018.
//

#include "MIKROE_2467.h"


static struct bme680_dev gas_sensor;
uint16_t meas_period;


void MIKROE_2467_init()
{
    gas_sensor.dev_id = BME680_I2C_ADDR_SECONDARY;
    gas_sensor.intf = BME680_I2C_INTF;
    gas_sensor.read = I2C_blocking_read;
    gas_sensor.write = I2C_blocking_write;
    gas_sensor.delay_ms = user_delay_ms;
    /* amb_temp can be set to 25 prior to configuring the gas sensor
     * or by performing a few temperature readings without operating the gas sensor.
     */
    gas_sensor.amb_temp = 25;


    int8_t rslt = BME680_OK;
    rslt = bme680_init(&gas_sensor);
    uint8_t set_required_settings;

    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_DISABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 150;  /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL |
                            BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings, &gas_sensor);

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(&gas_sensor);
    bme680_get_profile_dur(&meas_period, &gas_sensor);

    user_delay_ms(meas_period); /* Delay till the measurement is ready */
};


int8_t MIKROE_2467_read(struct bme680_field_data * data)
{
    int8_t rslt;
    rslt = bme680_get_sensor_data(data, &gas_sensor);

    if (rslt != BME680_OK){
        data->humidity = 0;
        data->pressure = 0;
        data->temperature = 0;

    }
    /* Trigger the next measurement if you would like to read data out continuously */
    if (gas_sensor.power_mode == BME680_FORCED_MODE)
    {
        rslt = bme680_set_sensor_mode(&gas_sensor);
    }

    return rslt;
};
