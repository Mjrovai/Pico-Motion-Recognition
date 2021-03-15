# Pico-Motion-Recognition
<img src='https://github.com/Mjrovai/Pico-Motion-Recognition/blob/main/Portada1.png'/>
<p>This Repository has the code used on the 2 parts tutorial <a href='https://www.hackster.io/mjrobot/tinyml-motion-recognition-using-raspberry-pi-pico-6b6071'>TinyML - Motion Recognition Using Raspberry Pi Pico</a> <p>
  
- The first part is to explore the Raspberry Pi Pico (RP2040), its main components and how to program it, using its C/C++ SDK (Software Development Kit). 

- In the second part, the Pico is programmed to capture "gesture data" used to train a tinyML model, using Edge Impulse Studio. Once developed and tested, the model is deployed and used for real inference on the same device.

- The project can be cloned on <a href='https://studio.edgeimpulse.com/public/20571/latest'>Edge Impulse Studio - Project</a>

- Here a quick view of the <a href='https://youtu.be/KZtT5GOwNDk'>project running at YouTube</a>

- Alternativelly, the code on <a href='https://github.com/Mjrovai/Pico-Motion-Recognition/tree/main/15_pico_accel_multicore_capture_data'>15_pico_accel_multicore_capture_data</a>, show how to caputre data continuosly on Core 0, when the data is sent to Serial by Core 1<img src='https://github.com/Mjrovai/Pico-Motion-Recognition/blob/main/accel_multicore_data_capture.png'/>

I recommend, visiting the great tutorial <a href='https://www.hackster.io/dmitrywat/machine-learning-inference-on-raspberry-pico-2040-e6e874'>Machine Learning Inference on Raspberry Pico 2040</a> by <a href='https://www.hackster.io/dmitrywat'>Dmitry Maslov</a>, which inspire me on this project. 

