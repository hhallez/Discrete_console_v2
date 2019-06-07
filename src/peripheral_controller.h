#ifndef PERIPHERAL_CONTROLLER_H__
#define PERIPHERAL_CONTROLLER__

#include <stdio.h>

#include "drivers/MIKROE_2467.h"
#include "drivers/MIKROE_3056.h"
#include "drivers/MIKROE_3085.h"
#include "drivers/MIKROE_3149.h"


#ifdef __cplusplus
extern "C"
{
#endif

void PER_CTRLR_init_drivers();

#ifdef __cplusplus
}
#endif

#endif // NRF_DRV_TWI_H__
