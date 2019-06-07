#include "MIKROE_3085.h"


void MIKROE_3085_init(){
    SI7034_init();
};


int8_t MIKROE_3085_read(MIKROE_3085_data *data){
    return SI7034_read(data);
};