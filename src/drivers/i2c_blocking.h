#ifndef I2C_BLOCKING_H__
#define I2C_BLOCKING_H__
/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>



int g_i2cFid;
bool I2C_init;
int8_t I2C_blocking_write(uint8_t dev_id, uint8_t reg_addr, uint8_t * data, uint16_t len);
int8_t I2C_blocking_read(uint8_t dev_id, uint8_t reg_addr, uint8_t * data, uint16_t len);
int8_t I2C_read(uint8_t dev_id, uint8_t * data, uint16_t len);
int8_t I2C_write(uint8_t dev_id, uint8_t * data, uint16_t len);
void I2C_blocking_init();
void I2C_blocking_handler();
void user_delay_ms(uint32_t ms);
void i2cSetAddress(int address);

#ifdef __cplusplus
}
#endif

#endif // NRF_DRV_TWI_H__
