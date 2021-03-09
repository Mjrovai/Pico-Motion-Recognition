/* Edge Impulse accelerometer inference 
 * Copyright (c) 2021 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * This file was adapted by Marcelo Rovai to be used as it on a Raspberry Pi Pico 
 * 
 */

/* Includes ---------------------------------------------------------------- */
#include <stdio.h>
#include "pico/stdlib.h"
#include "ei_run_classifier.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

/* Constant defines -------------------------------------------------------- */
#define CONVERT_G_TO_MS2    9.80665f
#define G0 1.65f
#define NSAMP 10

/* Private variables ------------------------------------------------------- */
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

const float conversion_factor = 3.3f / (1 << 12);

const uint LED_PIN = 25;

float readAxisAccelation (int adc_n) {
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

int main()
{
    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
    
    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
        return 1;
    }
    
    while (true){
        
        ei_printf("\nStarting inferencing in 2 seconds...\n");
        sleep_ms(2000);
        gpio_put(LED_PIN, 1);
        ei_printf("Sampling...\n");

        // Allocate a buffer here for the values we'll read from the IMU
        float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

        for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3) {
            // Determine the next tick (and then sleep later)
            uint64_t next_tick = ei_read_timer_us() + (EI_CLASSIFIER_INTERVAL_MS * 1000);
            
            buffer[ix] = readAxisAccelation (0);
            buffer[ix + 1] = readAxisAccelation (1);
            buffer[ix + 2] = readAxisAccelation (2);

            buffer[ix + 0] *= CONVERT_G_TO_MS2;
            buffer[ix + 1] *= CONVERT_G_TO_MS2;
            buffer[ix + 2] *= CONVERT_G_TO_MS2;

            sleep_us(next_tick - ei_read_timer_us());
        }

        // Turn the raw buffer in a signal which we can the classify
        signal_t signal;
        int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
            ei_printf("Failed to create signal from buffer (%d)\n", err);
            return 1;
        }

        // Run the classifier
        ei_impulse_result_t result = { 0 };

        err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", err);
            return 1;
        }

        // print the predictions
        ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
        ei_printf(": \n");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
    #endif
    gpio_put(LED_PIN, 0);
    }

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif
return 0;
}
