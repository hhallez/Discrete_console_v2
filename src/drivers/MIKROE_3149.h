//
// Created by krist on 29/11/2018.
//

#ifndef TWI_SENSOR_MIKROE_3149_H
#define TWI_SENSOR_MIKROE_3149_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include "bma400.h"
#include "i2c_blocking.h"



    typedef struct bma400_sensor_data MIKROE_3149_data;

    static struct bma400_dev bma;
    static struct bma400_sensor_conf bma_conf;
    static struct bma400_sensor_data bma_data;
    int8_t i2c_reg_write(
        void * intf_ptr, uint8_t i2c_addr, uint8_t reg_addr, uint8_t * reg_data, uint16_t length);
    int8_t i2c_reg_read(
        void * intf_ptr, uint8_t i2c_addr, uint8_t reg_addr, uint8_t * reg_data, uint16_t length);

        int8_t MIKROE_3149_read(struct bma400_sensor_data * data);
    void MIKROE_3149_init();


#ifdef __cplusplus
}
#endif
#endif // TWI_SENSOR_MIKROE_3149_H
