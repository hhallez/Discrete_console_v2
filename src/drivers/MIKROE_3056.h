//
// Created by krist on 29/11/2018.
//

#ifndef TWI_SENSOR_MIKROE_3056_H
#define TWI_SENSOR_MIKROE_3056_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "ADS1015.h"




    typedef struct MIKROE_3056_data
    {
        uint16_t CO;
        uint16_t NO2;
        uint16_t NH3;
    }

    MIKROE_3056_data;


    void MIKROE_3056_init();
     void MIKROE_3056_init_v2();
    int8_t MIKROE_3056_read(MIKROE_3056_data * data);
       int8_t MIKROE_3056_read_extra(MIKROE_3056_data * data);

#ifdef __cplusplus
}
#endif
#endif // TWI_SENSOR_MIKROE_3056_H
