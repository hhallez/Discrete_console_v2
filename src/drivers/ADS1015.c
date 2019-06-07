#include "ADS1015.h"

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
void ADS1015_writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value)
{
    char cmd[3];
    cmd[0] = (char)reg;
    cmd[1] = (char)(value >> 8);
    cmd[2] = (char)(value & 0xFF);
    I2C_write(i2cAddress, cmd, 3);
}

/**************************************************************************/
/*!
    @brief  Reads 16-bits from the specified register
*/
/**************************************************************************/
uint16_t ADS1015_readRegister(uint8_t i2cAddress, uint8_t reg)
{
uint8_t addr = i2cAddress;
   uint8_t data[2];
    data[0] = reg; // temporary use this to send address to conversion register
    I2C_write(i2cAddress, data, 1);
    I2C_read(addr, data, 2);
    return (data[0] << 8 | data[1]);
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1015 class w/appropriate properties
*/
/**************************************************************************/
void ADS1015_init()
{
    // shift 7 bit address 1 left: read expects 8 bit address, see I2C.h
    m_i2cAddress = ADS1015_ADDRESS << 0;
    m_conversionDelay = ADS1015_CONVERSIONDELAY;
    m_bitShift = 4;
    m_gain = GAIN_ONE; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1015 class w/appropriate properties
*/
/**************************************************************************/
void ADS1115_init()
{
    // shift 7 bit address 1 left: read expects 8 bit address, see I2C.h
    m_i2cAddress = ADS1015_ADDRESS << 0;
    m_conversionDelay = ADS1115_CONVERSIONDELAY;
    m_bitShift = 0;
    m_gain = GAIN_ONE; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}


/**************************************************************************/
/*!
    @brief  Sets the gain and input voltage range
*/
/**************************************************************************/
void ADS1015_setGain(adsGain_t gain) { m_gain = gain; }

/**************************************************************************/
/*!
    @brief  Gets a gain and input voltage range
*/
/**************************************************************************/
adsGain_t ADS1015_getGain() { return m_gain; }

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel
*/
/**************************************************************************/
uint16_t ADS1015_readADC_SingleEnded(uint8_t channel)
{
    if (channel > 3)
    {
        return 0;
    }

    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600(ADS1015) or 250(ADS1115) samples per
                                                        // second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set single-ended input channel
    switch (channel)
    {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    ADS1015_writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    user_delay_ms(m_conversionDelay);

    // Read the conversion results
    // Shift 12-bit results right 4 bits for the ADS1015
    uint16_t rslt = ADS1015_readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
    return rslt;
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t ADS1015_readADC_Differential_0_1()
{
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600(ADS1015) or 250(ADS1115) samples per
                                                        // second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1; // AIN0 = P, AIN1 = N

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    ADS1015_writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    user_delay_ms(m_conversionDelay);

    // Read the conversion results
    uint16_t res = ADS1015_readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
    if (m_bitShift == 0)
    {
        return (int16_t)res;
    }
    else
    {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF)
        {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
        return (int16_t)res;
    }
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
*/
/**************************************************************************/
int16_t ADS1015_readADC_Differential_2_3()
{
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600(ADS1015) or 250(ADS1115) samples per
                                                        // second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= m_gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3; // AIN2 = P, AIN3 = N

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    ADS1015_writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    user_delay_ms(m_conversionDelay);

    // Read the conversion results
    uint16_t res = ADS1015_readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
    if (m_bitShift == 0)
    {
        return (int16_t)res;
    }
    else
    {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF)
        {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
        return (int16_t)res;
    }
}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.

            This will also set the ADC in continuous conversion mode.
*/
/**************************************************************************/
void ADS1015_startComparator_SingleEnded(uint8_t channel, int16_t threshold)
{
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_1CONV |   // Comparator enabled and asserts on 1 match
                      ADS1015_REG_CONFIG_CLAT_LATCH |   // Latching mode
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600(ADS1015) or 250(ADS1115) samples per
                                                        // second (default)
                      ADS1015_REG_CONFIG_MODE_CONTIN |  // Continuous conversion mode
                      ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

    // Set PGA/voltage range
    config |= m_gain;

    // Set single-ended input channel
    switch (channel)
    {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set the high threshold register
    // Shift 12-bit results left 4 bits for the ADS1015
    ADS1015_writeRegister(m_i2cAddress, ADS1015_REG_POINTER_HITHRESH, threshold << m_bitShift);

    // Write config register to the ADC
    ADS1015_writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
}

/**************************************************************************/
/*!
    @brief  In order to clear the comparator, we need to read the
            conversion results.  This function reads the last conversion
            results without changing the config value.
*/
/**************************************************************************/
int16_t ADS1015_getLastConversionResults()
{
    // Wait for the conversion to complete
    user_delay_ms(m_conversionDelay);

    // Read the conversion results
    uint16_t res = ADS1015_readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
    if (m_bitShift == 0)
    {
        return (int16_t)res;
    }
    else
    {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF)
        {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
        return (int16_t)res;
    }
}
