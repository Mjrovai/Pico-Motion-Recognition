#include <stdio.h>
#include "pico/stdlib.h"
#include "ei_run_classifier.h"

const uint LED_PIN = 25;
static const float features[] = {-1.2641, 0.9955, 4.3218, -0.9876, 1.0113, 2.7179, -0.7427, 0.5452, 1.5881, -0.5294, 0.4266, 0.6716, 0.0237, 0.3713, -0.9402, 0.7822, 0.3713, -1.4064, 1.0587, 0.7980, -1.5012, 1.0192, 0.9481, -1.0982, 0.5294, 0.8138, -0.5610, -0.2923, 0.2844, 0.5373, -1.0350, 0.0632, 2.1095, -1.1535, 0.1027, 3.8398, -1.4064, 0.1264, 5.5701, -1.6671, 0.8454, 4.7247, -1.3510, 0.6163, 2.8127, -0.9323, 0.1975, 0.9402, -0.4662, 0.2765, -0.3792, 0.2607, 0.4820, -1.2720, 0.8454, 0.8691, -1.3668, 0.9165, 0.9323, -1.1614, 0.6637, 1.1061, -0.7822, 0.3476, 0.8138, -0.1264, -0.3871, 0.3318, 0.8217, -1.1377, -0.2528, 2.8996, -1.5723, -0.0237, 4.9854, -1.5486, 0.4345, 5.3173, -1.3589, 1.0350, 3.8793, -1.1298, 0.6953, 2.1964, -0.8138, 0.5926, 0.8770, -0.3397, 0.4187, -0.5768, 0.5136, 0.1659, -1.7066, 0.9481, 0.6795, -1.7935, 0.8849, 0.7585, -1.5486, 0.6558, 1.1140, -1.0666, 0.1738, 1.0587, -0.5610, -0.7348, 0.2212, 1.1930, -1.5644, 0.3002, 3.3500, -2.1569, 0.0237, 5.7834, -2.2992, 0.9402, 6.0284, -1.6118, 1.4775, 3.5396, -1.1456, 0.8454, 1.6118, -0.6795, 0.8691, -0.6084};


int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_put(LED_PIN, 1);
    sleep_ms(250);
    gpio_put(LED_PIN, 0);
    sleep_ms(250);
    
    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        sleep_ms(1000);
        return -1;
    }

    ei_impulse_result_t result = {0};

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    while (true) 
    
    {
        ei_printf("Edge Impulse standalone inferencing (Raspberry Pico 2040)\n");
        gpio_put(LED_PIN, 0);
        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
        ei_printf("run_classifier returned: %d\n", res);

        if (res != 0) return res;

        // print the predictions
        ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
        ei_printf(": \n");
        ei_printf("[");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("%.5f", result.classification[ix].value);
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf(", ");
    #else
            if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
                ei_printf(", ");
            }
    #endif
        }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("%.3f", result.anomaly);
    #endif
        ei_printf("]\n");

        // human-readable predictions
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
    #endif
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);

    }
return 0;
}
