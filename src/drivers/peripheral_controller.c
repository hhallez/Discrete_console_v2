#include "peripheral_controller.h"


void PER_CTRLR_init_drivers()
{
    I2C_blocking_init();
    MIKROE_2467_init(); // Environnement temp -> Temp, Rel. Hum, Pressure, (VOCS)
    //MIKROE_3056_init(); // Air quality -> NH3 (and more)

    MIKROE_3056_init_v2();

    MIKROE_3085_init(); // Temp and Humidity
    MIKROE_3149_init(); // Accelerometer x, y, z

    printf("other inits done");

    begin();
    powerOn();
   //doCalibrate();

}

int PCTRL_read_env(MIKROE_2467_data * data) { return MIKROE_2467_read(data); }

int PCTRL_read_env2(MIKROE_3085_data * data) { return MIKROE_3085_read(data); }

int PCTRL_read_air(MIKROE_3056_data * data) { return MIKROE_3056_read_extra(data); }

int PCTRL_read_acc(MIKROE_3149_data * data) { return MIKROE_3149_read(data); }
