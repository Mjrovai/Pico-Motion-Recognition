#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"

#define NSAMP 10

#define G0 1.65f
#define CONVERT_G_TO_MS2    9.80665f
#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

const float conversion_factor = 3.3f / (1 << 12);

float get_axis (int adc_n) {
    adc_select_input(adc_n);
    unsigned int axis_raw = 0;
    for (int i=0;i<NSAMP;i++){
        axis_raw = axis_raw + adc_read();
        sleep_ms(1);
    }
    axis_raw = axis_raw/NSAMP;
    float axis_g = (axis_raw*conversion_factor)-G0;
    return axis_g;
}

int main() {

    stdio_init_all();
    
    adc_init();

    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

    while (1) {

        printf("%f \t", (get_axis (0) * CONVERT_G_TO_MS2));
        printf("%f \t", (get_axis (1) * CONVERT_G_TO_MS2));
        printf("%f \n", (get_axis (2) * CONVERT_G_TO_MS2));
        
        sleep_ms(INTERVAL_MS);
    }
}