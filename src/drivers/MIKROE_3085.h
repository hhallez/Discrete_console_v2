#ifndef MIKROE_3085_H
#define MIKROE_3085_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "si7034.h"

typedef struct SI7034_data MIKROE_3085_data;

void MIKROE_3085_init();
int8_t MIKROE_3085_read(MIKROE_3085_data *data);

#ifdef __cplusplus
}
#endif


#endif //TWI_SENSOR_MIKROE_3085_H
