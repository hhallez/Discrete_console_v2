
#include "si7034.h"

void SI7034_init(){

};

float calculateT(uint16_t rawValue) {
  return 100 * (-45 + 175 * (rawValue / pow(2, 16)));
}

float calculateRH(uint16_t rawValue) {
  return 100 * 100 * (rawValue / pow(2, 16));
}

int8_t SI7034_read(struct SI7034_data *result) {
  int8_t rslt = 0;
  uint8_t cmd[2];
  cmd[0] = (SI7034_REG_FAST_READ >> 8) & 0xFF;
  cmd[1] = SI7034_REG_FAST_READ & 0xFF;

  if (I2C_write(SI7034_ADDR, cmd, sizeof(cmd)) == -1) {
    result->error = 0x01;
    rslt = -1;
    result->t = 0;
    result->rh = 0;
  result->error = 0x02;
  } else {
      usleep(2000);
    uint8_t m_sample[6];
    if (I2C_read(SI7034_ADDR, m_sample, sizeof(m_sample)) != -1) {
      result->t = calculateT(m_sample[0] << 8 | m_sample[1]);
      result->rh = calculateRH(m_sample[3] << 8 | m_sample[4]);
      result->error = 0x00;
    } else{
        result->t = 0;
        result->rh = 0;
      result->error = 0x02;
    rslt = -1;
    }
  }
  return rslt;
/*
  uint8_t m_sample[6];
  I2C_write(SI7034_ADDR, cmd, sizeof(cmd));

  I2C_read(SI7034_ADDR, m_sample, sizeof(m_sample));
  result->t = calculateT(m_sample[0] << 8 | m_sample[1]);
  result->rh = calculateRH(m_sample[3] << 8 | m_sample[3]);
  result->error = 0x00;
  return rslt;
  */
};
