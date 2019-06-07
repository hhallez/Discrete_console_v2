

#ifndef SI7034_H
#define SI7034_H

#ifdef __cplusplus
extern "C" {
#endif
#include "i2c_blocking.h"
#include "math.h"

/* Addresses for Si7021 */
#define SI7034_ADDR         0x70U

#define SI7034_REG_FAST_READ 0x6458U
#define SI7034_REG_NORMAL_READ 0x7CA2U
//static uint16_t humRaw;
//static uint16_t tempRaw;


struct SI7034_data {
    int16_t t;
    int16_t rh;
    uint8_t error;
};


void SI7034_init();
int8_t SI7034_read(struct SI7034_data *data );


#ifdef __cplusplus
}
#endif


#endif //SI7034_H
