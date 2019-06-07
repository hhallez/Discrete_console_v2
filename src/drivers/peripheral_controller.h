#ifndef PERIPHERAL_CONTROLLER__
#define PERIPHERAL_CONTROLLER__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "i2c_blocking.h"
#include "MIKROE_2467.h"
#include "MIKROE_3056.h"
#include "MIKROE_3085.h"
#include "MIKROE_3149.h"
#include "MultichannelGasSensor.h"


void PER_CTRLR_init_drivers();
int PCTRL_read_env(MIKROE_2467_data * data);

int PCTRL_read_env2(MIKROE_3085_data * data);

int PCTRL_read_air(MIKROE_3056_data * data);

int PCTRL_read_acc(MIKROE_3149_data * data);


#ifdef __cplusplus
}
#endif

#endif // NRF_DRV_TWI_H__
