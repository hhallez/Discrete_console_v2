#ifndef TWI_SENSOR_MIKROE_2467_H
#define TWI_SENSOR_MIKROE_2467_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "bme680.h"
#include "i2c_blocking.h"



typedef struct bme680_field_data MIKROE_2467_data;

void MIKROE_2467_init();
int8_t MIKROE_2467_read(MIKROE_2467_data *data);

#ifdef __cplusplus
}
#endif
#endif //TWI_SENSOR_MIKROE_2467_H
