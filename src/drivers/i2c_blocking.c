#include "i2c_blocking.h"
// set the I2C slave address for all subsequent I2C device transfers
void i2cSetAddress(int address)
{
    if (ioctl(g_i2cFid, I2C_SLAVE, address) < 0) {
        perror("i2cSetAddress");
    }
}

int8_t I2C_write(uint8_t dev_id, uint8_t * data, uint16_t len)
{

    i2cSetAddress(dev_id);
    uint8_t reg[len];

    for (int i=0; i<len; i++)
       reg[i] = data[i];

    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    if (write(g_i2cFid, reg, len) != len) {
        perror("user_i2c_write");
        rslt = -1;
    }


    return rslt;
}

int8_t I2C_read(uint8_t dev_id, uint8_t * data, uint16_t len)
{
    i2cSetAddress(dev_id);

    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    uint8_t da [len];
    if (read(g_i2cFid, da, len)!= len) {
        perror("user_i2c_read_data");
        rslt = -1;
        for (int i = 0; i<len ; i++)
            da[i]=0;
    }
    for(int i=0; i<len;i++){
        data[i] = da[i];
    }

    return rslt;
}


int8_t I2C_blocking_write(uint8_t dev_id, uint8_t reg_addr, uint8_t * reg_data, uint16_t len)
{

    i2cSetAddress(dev_id);
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */


    uint8_t reg[16];
    reg[0]=reg_addr;

    for (int i=1; i<len+1; i++)
       reg[i] = reg_data[i-1];

    if (write(g_i2cFid, reg, len+1) != len+1) {
        perror("user_i2c_write");
        rslt = -1;
        for (int i = 0; i<len ; i++)
            reg_data[i]=0;
    }

    return rslt;
}


int8_t I2C_blocking_read(uint8_t dev_id, uint8_t reg_addr, uint8_t * reg_data, uint16_t len)
{

    i2cSetAddress(dev_id);
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    uint8_t reg[1];
    reg[0]=reg_addr;

    if (write(g_i2cFid, reg, 1) != 1) {
        perror("user_i2c_read_reg");
        rslt = -1;
    }
    if (read(g_i2cFid, reg_data, len) != len) {
        perror("user_i2c_read_data");
        rslt = -1;
        for (int i = 0; i<len ; i++)
            reg_data[i]=0;
    }

    return rslt;
}

void I2C_blocking_init(void)
{
    if (!I2C_init)
    {
        g_i2cFid = open("/dev/i2c-1", O_RDWR);
        if (g_i2cFid < 0) {
            perror("i2cOpen");
            exit(1);
        }
        I2C_init = true;
    }
}
void user_delay_ms(uint32_t ms)
{
    usleep(ms*1000);

}


