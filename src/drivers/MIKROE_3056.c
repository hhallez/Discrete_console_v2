//
// Created by krist on 29/11/2018.
//

#include "MIKROE_3056.h"


void MIKROE_3056_init() { ADS1015_init(); }
void MIKROE_3056_init_v2() { ADS1115_init(); }

int8_t MIKROE_3056_read(MIKROE_3056_data * data)
{
    data->CO = ADS1015_readADC_SingleEnded(2);
    data->NH3 = ADS1015_readADC_SingleEnded(1);
    data->NO2 = ADS1015_readADC_SingleEnded(0);
}

int8_t MIKROE_3056_read_extra(MIKROE_3056_data * data)
{
    data->CO = ADS1015_readADC_SingleEnded(2);
    data->NO2 = ADS1015_readADC_SingleEnded(1);
    data->NH3 = ADS1015_readADC_SingleEnded(0);
}
