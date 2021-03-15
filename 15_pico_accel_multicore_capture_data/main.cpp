#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define SENSOR_BUFFER_SIZE 3
#define NSAMP 10
#define G0 1.65f
#define CONVERT_G_TO_MS2    9.80665f
#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

const float conversion_factor = 3.3f / (1 << 12);

float buffer[SENSOR_BUFFER_SIZE] = { 0 };

// Read accelerometer value by axis
uint32_t readAxisAcceleration (int adc_n) {
    adc_select_input(adc_n);
    unsigned int axis_raw = 0;
    for (int i=0;i<NSAMP;i++){
        axis_raw = axis_raw + adc_read();
        sleep_ms(1);
    }
    axis_raw = axis_raw/NSAMP;
    //float axis_g = (axis_raw*conversion_factor)-G0;
    return axis_raw;
}
 

// Core 1 Main Code
void second_core_code() {
        
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
    
	while(1)
	{
		for (int i = 0; i < SENSOR_BUFFER_SIZE; i++){
            uint32_t val = readAxisAcceleration (i);
            multicore_fifo_push_blocking(val);
        }
        sleep_ms(INTERVAL_MS);
	}
}


// Core 0 Main Code
int main(void){
    stdio_init_all();

    multicore_launch_core1(second_core_code); // Start core 1

    // Primary Core 0 Loop
    while (1) {
        for (int i = 0; i < SENSOR_BUFFER_SIZE; i++){
            uint32_t axis_raw = multicore_fifo_pop_blocking();
            float axis_g = (axis_raw*conversion_factor)-G0;
            printf("%f \t", (axis_g * CONVERT_G_TO_MS2)); 
        }
        printf("\n"); 
    }
}