//
// Created by krist on 29/11/2018.
//

#include "MIKROE_3149.h"


int8_t i2c_reg_write(
    void * intf_ptr, uint8_t i2c_addr, uint8_t reg_addr, uint8_t * reg_data, uint16_t length)
{
    I2C_blocking_write(i2c_addr, reg_addr, reg_data, length);
};
int8_t i2c_reg_read(
    void * intf_ptr, uint8_t i2c_addr, uint8_t reg_addr, uint8_t * reg_data, uint16_t length)
{
    I2C_blocking_read(i2c_addr, reg_addr, reg_data, length);
};


void MIKROE_3149_init()
{
    int8_t rslt;
    bma.intf_ptr = NULL; /* To attach your interface device reference */
    bma.delay_ms = user_delay_ms;
    bma.dev_id = BMA400_I2C_ADDRESS_SDO_HIGH;
    bma.read = i2c_reg_read;
    bma.write = i2c_reg_write;
    bma.intf = BMA400_I2C_INTF;
    rslt = bma400_init(&bma);
    rslt = bma400_soft_reset(&bma);

    bma_conf.param.accel.odr = BMA400_ODR_100HZ;
    bma_conf.param.accel.range = BMA400_2G_RANGE;
    bma_conf.param.accel.data_src = BMA400_DATA_SRC_ACCEL_FILT_1;
    rslt = bma400_set_sensor_conf(&bma_conf, 1, &bma);

    rslt = bma400_set_power_mode(BMA400_LOW_POWER_MODE, &bma);
}


int8_t MIKROE_3149_read(struct bma400_sensor_data * data)
{
    int8_t rslt;
    rslt = bma400_get_accel_data(BMA400_DATA_SENSOR_TIME, data, &bma);
    if (rslt !=BMA400_OK){
        data->x = 0;
        data->y = 0;
        data->z = 0;
    }
    return rslt;
}
