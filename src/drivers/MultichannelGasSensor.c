#include "MultichannelGasSensor.h"

/*********************************************************************************************************
** Function name:           begin
** Descriptions:            initialize I2C
*********************************************************************************************************/
void begin_custom(int address)
{
    __version = 1;          // version 1/2
    r0_inited = false;

    i2cAddress = address;
    __version = getVersion();
}

unsigned char getVersion()
{
    if(get_addr_dta2(CMD_READ_EEPROM, ADDR_IS_SET) == 1126)        // get version
    {
        __version = 2;
        printf("version = 2");
        return 2;
    }

    __version = 1;
    printf("version = 1");
    return 1;
}

void begin()
{
    begin_custom(DEFAULT_I2C_ADDR);
}



unsigned int get_addr_dta(unsigned char addr_reg)
{
    uint8_t data[2];

    data[0] = addr_reg;
    int data1 = I2C_write(i2cAddress,data, 1);
    int data2 = I2C_read(i2cAddress, data, 2);

    printf("I2C data1: %d data2: %d \r\n", data1,data2);

    if (data2 == -1){
        return 0;
    }

    unsigned int dta = (data[0] << 8 | data[1]);

    switch(addr_reg)
    {
        case CH_VALUE_NH3:

            if(dta > 0)
            {
                adcValueR0_NH3_Buf = dta;
            }
            else
            {
                dta = adcValueR0_NH3_Buf;
            }

            break;

        case CH_VALUE_CO:

            if(dta > 0)
            {
                adcValueR0_CO_Buf = dta;
            }
            else
            {
                dta = adcValueR0_CO_Buf;
            }

            break;

        case CH_VALUE_NO2:

            if(dta > 0)
            {
                adcValueR0_NO2_Buf = dta;
            }
            else
            {
                dta = adcValueR0_NO2_Buf;
            }

            break;

        default:;
    }
    return dta;
}

unsigned int get_addr_dta2(unsigned char addr_reg, unsigned char __dta)
{
    uint8_t addr = i2cAddress;
    uint8_t data[2];
    data[0] = addr_reg; // temporary use this to send address to conversion register
    data[1] = __dta; // temporary use this to send address to conversion register

    I2C_write(i2cAddress, data, 2);
    I2C_read(addr, data, 2);
    return (data[0] << 8 | data[1]);
}


/*********************************************************************************************************
** Function name:           readData
** Descriptions:            read 4 bytes from I2C slave
*********************************************************************************************************/
int16_t readData(uint8_t cmd)
{
    uint16_t timeout = 0;
    uint8_t buffer[4];
    uint8_t checksum = 0;
    int16_t rtnData = 0;

    //send command
    I2C_write(i2cAddress,cmd, 1);
    //wait for a while
    user_delay_ms(2);
    //get response

    I2C_read(i2cAddress, buffer, 4);
    checksum = (uint8_t)(buffer[0] + buffer[1] + buffer[2]);
    if(checksum != buffer[3])
        return -4;//checksum wrong
    rtnData = ((buffer[1] << 8) + buffer[2]);

    return rtnData;//successful
}

/*********************************************************************************************************
** Function name:           readR0
** Descriptions:            read R0 stored in slave MCU
*********************************************************************************************************/
int16_t readR0(void)
{
    int16_t rtnData = 0;

    rtnData = readData(0x11);

    if(rtnData > 0)
        res0[0] = rtnData;
    else
        return rtnData;         //unsuccessful

    rtnData = readData(0x12);
    if(rtnData > 0)
        res0[1] = rtnData;
    else
        return rtnData;         //unsuccessful

    rtnData = readData(0x13);
    if(rtnData > 0)
        res0[2] = rtnData;
    else
        return rtnData;         //unsuccessful

    return 1;//successful
}

/*********************************************************************************************************
** Function name:           readR
** Descriptions:            read resistance value of each channel from slave MCU
*********************************************************************************************************/
int16_t readR(void)
{
    int16_t rtnData = 0;

    rtnData = readData(0x01);
    if(rtnData >= 0)
        res[0] = rtnData;
    else
        return rtnData;//unsuccessful

    rtnData = readData(0x02);
    if(rtnData >= 0)
        res[1] = rtnData;
    else
        return rtnData;//unsuccessful

    rtnData = readData(0x03);
    if(rtnData >= 0)
        res[2] = rtnData;
    else
        return rtnData;//unsuccessful

    return 0;//successful
}

float readAll(struct MGSData * data){
    float ratio0, ratio1, ratio2;
    if(1 == __version)
    {
        if(!r0_inited)
        {
            if(readR0() >= 0) r0_inited = true;
            else return -1.0f;
        }

        if(readR() < 0)
            return -2.0f;

        ratio0 = (float)res[0] / res0[0];
        ratio1 = (float)res[1] / res0[1];
        ratio2 = (float)res[2] / res0[2];
    }
    else if(2 == __version)
    {
        // how to calc ratio/123

        int A0_0 = get_addr_dta2(6, ADDR_USER_ADC_HN3);
        int A0_1 = get_addr_dta2(6, ADDR_USER_ADC_CO);
        int A0_2 = get_addr_dta2(6, ADDR_USER_ADC_NO2);

        int An_0 = get_addr_dta(CH_VALUE_NH3);
        int An_1 = get_addr_dta(CH_VALUE_CO);
        int An_2 = get_addr_dta(CH_VALUE_NO2);

        ratio0 = (float)An_0/(float)A0_0*(1023.0-A0_0)/(1023.0-An_0);
        ratio1 = (float)An_1/(float)A0_1*(1023.0-A0_1)/(1023.0-An_1);
        ratio2 = (float)An_2/(float)A0_2*(1023.0-A0_2)/(1023.0-An_2);

        data->RAW_CO = An_1;
                data->RAW_NH3 = An_0;
                data->RAW_NO2 = An_2;
                data->REF_CO = A0_1;
                        data->REF_NH3 = A0_0;
                        data->REF_NO2 = A0_2;
                printf("Register: %d, %d, %d\r\n", A0_0,A0_1,A0_2);
                printf("Sensor now:  %d, %d, %d\r\n", An_0,An_1,An_2);


    }
            data->CO = pow(ratio1, -1.179)*4.385;  //mod by jack
            data->NO2 = pow(ratio2, 1.007)/6.855;  //mod by jack
            data->NH3 = pow(ratio0, -1.67)/1.47;  //modi by jack
            data->C3H8 = pow(ratio0, -2.518)*570.164;
 data->C4H10 = pow(ratio0, -2.138)*398.107;
        data->CH4= pow(ratio1, -4.363)*630.957;
        data->H2 = pow(ratio1, -1.8)*0.73;
        data->C2H5OH =pow(ratio1, -1.552)*1.622;


    return 0;
}


/*********************************************************************************************************
** Function name:           readR
** Descriptions:            calculate gas concentration of each channel from slave MCU
** Parameters:
                            gas - gas type
** Returns:
                            float value - concentration of the gas
*********************************************************************************************************/
float calcGas(int gas)
{

    float ratio0, ratio1, ratio2;
    if(1 == __version)
    {
        if(!r0_inited)
        {
            if(readR0() >= 0) r0_inited = true;
            else return -1.0f;
        }

        if(readR() < 0)
            return -2.0f;

        ratio0 = (float)res[0] / res0[0];
        ratio1 = (float)res[1] / res0[1];
        ratio2 = (float)res[2] / res0[2];
    }
    else if(2 == __version)
    {
        // how to calc ratio/123

        int A0_0 = get_addr_dta2(6, ADDR_USER_ADC_HN3);
        int A0_1 = get_addr_dta2(6, ADDR_USER_ADC_CO);
        int A0_2 = get_addr_dta2(6, ADDR_USER_ADC_NO2);

        int An_0 = get_addr_dta(CH_VALUE_NH3);
        int An_1 = get_addr_dta(CH_VALUE_CO);
        int An_2 = get_addr_dta(CH_VALUE_NO2);

        ratio0 = (float)An_0/(float)A0_0*(1023.0-A0_0)/(1023.0-An_0);
        ratio1 = (float)An_1/(float)A0_1*(1023.0-A0_1)/(1023.0-An_1);
        ratio2 = (float)An_2/(float)A0_2*(1023.0-A0_2)/(1023.0-An_2);

    }

    float c = 0;

    switch(gas)
    {
        case CO:
        {
            c = pow(ratio1, -1.179)*4.385;  //mod by jack
            break;
        }
        case NO2:
        {
            c = pow(ratio2, 1.007)/6.855;  //mod by jack
            break;
        }
        case NH3:
        {
            c = pow(ratio0, -1.67)/1.47;  //modi by jack
            break;
        }
        case C3H8:  //add by jack
        {
            c = pow(ratio0, -2.518)*570.164;
            break;
        }
        case C4H10:  //add by jack
        {
            c = pow(ratio0, -2.138)*398.107;
            break;
        }
        case CH4:  //add by jack
        {
            c = pow(ratio1, -4.363)*630.957;
            break;
        }
        case H2:  //add by jack
        {
            c = pow(ratio1, -1.8)*0.73;
            break;
        }
        case C2H5OH:  //add by jack
        {
            c = pow(ratio1, -1.552)*1.622;
            break;
        }
        default:
            break;
    }

    return isnan(c)?-3:c;
}

/*********************************************************************************************************
** Function name:           changeI2cAddr
** Descriptions:            change I2C address of the slave MCU, and this address will be stored in EEPROM of slave MCU
*********************************************************************************************************/
void changeI2cAddr(uint8_t newAddr)
{
    uint8_t cmd[2];
    cmd[0] = 0x23;
    cmd [1] = newAddr;
    I2C_write(i2cAddress, cmd, 2);
    i2cAddress = newAddr;
}

/*********************************************************************************************************
** Function name:           doCalibrate
** Descriptions:            tell slave to do a calibration, it will take about 8s
                            after the calibration, must reread the R0 values
*********************************************************************************************************/
void doCalibrate(void)
{

    uint8_t data[1];
    if(1 == __version)
    {
        START:

        data[0] = 0x22;

        I2C_write(i2cAddress, data, 1);

        if(readR0() > 0)
        {
            for(int i=0; i<3; i++)
            {
                printf("%d",res0[i]);
                printf("\t");
            }
        }
        else
        {
            user_delay_ms(5000);

            for(int i=0; i<3; i++)
            {
                printf("%d",res0[i]);
                printf("\t");

            }
            goto START;
        }
    }
    else if(2 == __version)
    {
        unsigned int i, a0, a1, a2;
        while(1)
        {
            a0 = get_addr_dta(CH_VALUE_NH3);
            a1 = get_addr_dta(CH_VALUE_CO);
            a2 = get_addr_dta(CH_VALUE_NO2);

            printf("%d",a0);
            printf("\t");
            printf("%d",a1);
            printf("\t");
            printf("%d",a2);
            printf("\t");



            int cnt = 0;
            for(i=0; i<20; i++)
            {
                if((a0 - get_addr_dta(CH_VALUE_NH3)) > 2 || (get_addr_dta(CH_VALUE_NH3) - a0) > 2)cnt++;
                if((a1 - get_addr_dta(CH_VALUE_CO)) > 2 || (get_addr_dta(CH_VALUE_CO) - a1) > 2)cnt++;
                if((a2 - get_addr_dta(CH_VALUE_NO2)) > 2 || (get_addr_dta(CH_VALUE_NO2) - a2) > 2)cnt++;

                if(cnt>5)
                {
                    break;
                }
                user_delay_ms(1000);
            }
            printf("%d", cnt);
            fflush(stdout);
            if(cnt <= 5)break;
            user_delay_ms(200);
        }

        printf("write user adc value: ");
        printf("%i",a0);printf("\t");
        printf("%i",a1);printf("\t");
        printf("%i",a2);printf("\t");

        unsigned char tmp[7];

        tmp[0] = 7;

        tmp[1] = a0>>8;
        tmp[2] = a0&0xff;

        tmp[3] = a1>>8;
        tmp[4] = a1&0xff;

        tmp[5] = a2>>8;
        tmp[6] = a2&0xff;

        I2C_write(i2cAddress, tmp, 7);
    }
}

/*********************************************************************************************************
** Function name:           powerOn
** Descriptions:            power on sensor heater
*********************************************************************************************************/
void powerOn(void) {

    if (__version == 1) {
        uint8_t data[1];
    data[0] = 0x21;

    I2C_write(i2cAddress, data, 1);

}else if(__version == 2)
    {
        dta_test[0] = 11;
        dta_test[1] = 1;
        I2C_write(i2cAddress, dta_test, 2);
    }
}

/*********************************************************************************************************
** Function name:           powerOff
** Descriptions:            power off sensor heater
*********************************************************************************************************/
void powerOff(void) {
    if (__version == 1) {
    uint8_t data[1];
    data[0] = 0x20;

    I2C_write(i2cAddress, data, 1);
}else if(__version == 2)
    {
        dta_test[0] = 11;
        dta_test[1] = 0;
        I2C_write(i2cAddress, dta_test, 2);
    }
}

void display_eeprom()
{
    if(__version == 1)
    {
        printf("ERROR: display_eeprom() is NOT support by V1 firmware.");
        return ;
    }

    printf("ADDR_IS_SET = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_IS_SET));
    printf("ADDR_FACTORY_ADC_NH3 = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_FACTORY_ADC_NH3));
    printf("ADDR_FACTORY_ADC_CO = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_FACTORY_ADC_CO));
    printf("ADDR_FACTORY_ADC_NO2 = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_FACTORY_ADC_NO2));
    printf("ADDR_USER_ADC_HN3 = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_USER_ADC_HN3));
    printf("ADDR_USER_ADC_CO = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_USER_ADC_CO));
    printf("ADDR_USER_ADC_NO2 = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_USER_ADC_NO2));
    printf("ADDR_I2C_ADDRESS = "); printf("%d",get_addr_dta2(CMD_READ_EEPROM, ADDR_I2C_ADDRESS));
}

float getR0(unsigned char ch)         // 0:CH3, 1:CO, 2:NO2
{
    if(__version == 1)
    {
        printf("ERROR: getR0() is NOT support by V1 firmware.");
        return -1;
    }

    int a = 0;
    switch(ch)
    {
        case 0:         // CH3
            a = get_addr_dta2(CMD_READ_EEPROM, ADDR_USER_ADC_HN3);
            printf("a_ch3 = ");
            printf(a);
            break;

        case 1:         // CO
            a = get_addr_dta2(CMD_READ_EEPROM, ADDR_USER_ADC_CO);
            printf("a_co = ");
            printf(a);
            break;

        case 2:         // NO2
            a = get_addr_dta2(CMD_READ_EEPROM, ADDR_USER_ADC_NO2);
            printf("a_no2 = ");
            printf(a);
            break;

        default:;
    }

    float r = (float) (56.0 * (float)a / (1023.0 - (float)a));
    return r;
}

float getRs(unsigned char ch)         // 0:CH3, 1:CO, 2:NO2
{

    if(__version == 1)
    {
        printf("ERROR: getRs() is NOT support by V1 firmware.");
        return -1;
    }

    int a = 0;
    switch(ch)
    {
        case 0:         // NH3
            a = get_addr_dta(1);
            break;

        case 1:         // CO
            a = get_addr_dta(2);
            break;

        case 2:         // NO2
            a = get_addr_dta(3);
            break;

        default:;
    }

    float r = (float) (56.0 * (float)a / (1023.0 - (float)a));
    return r;
}

// 1. change i2c address to 0x04
// 2. change adc value of R0 to default
void factory_setting()
{

    unsigned char tmp[7];

    unsigned char error = 0;
    unsigned char address = 0;

    for(address = 1; address < 127; address++ )
    {
        if (error == 0)
        {
            // change i2c to 0x04

            printf("I2C address is: 0x");
            printf("%X",address);
            printf("Change I2C address to 0x04");

            dta_test[0] = CMD_CHANGE_I2C;
            dta_test[1] = 0x04;
            I2C_write(address, dta_test, 2);

            i2cAddress = 0x04;
            user_delay_ms(100);
            getVersion();
            break;
        }
    }

    unsigned int a0 = get_addr_dta2(CMD_READ_EEPROM, ADDR_FACTORY_ADC_NH3);
    unsigned int a1 = get_addr_dta2(CMD_READ_EEPROM, ADDR_FACTORY_ADC_CO);
    unsigned int a2 = get_addr_dta2(CMD_READ_EEPROM, ADDR_FACTORY_ADC_NO2);

    tmp[0] = 7;
    tmp[1] = a0>>8;
    tmp[2] = a0&0xff;
    tmp[3] = a1>>8;
    tmp[4] = a1&0xff;

    tmp[5] = a2>>8;
    tmp[6] = a2&0xff;
    user_delay_ms(100);
    I2C_write(i2cAddress, tmp, 7);
    user_delay_ms(100);
}

void change_i2c_address(unsigned char addr)
{
    dta_test[0] = CMD_CHANGE_I2C;
    dta_test[1] = addr;
    I2C_write(i2cAddress, dta_test, 2);


    printf("FUNCTION: CHANGE I2C ADDRESS: 0X");
    printf("%X", i2cAddress);
    printf(" > 0x");
    printf("%X", addr);

    i2cAddress = addr;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
