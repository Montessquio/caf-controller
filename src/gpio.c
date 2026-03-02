#include "efr32bg22c224f512im40.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "system_efr32bg22.h"
#include <stdint.h>

#include "gpio.h"

void init_gpio(void) {
    CMU->CLKEN0_SET = 1<<26;
    GPIO->P_SET[gpioPortB].MODEL = 4;
}
