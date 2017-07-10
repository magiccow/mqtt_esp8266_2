# mqtt_esp8266_2
Published first attempt

Code that accompanies Youtube video: https://youtu.be/TzeVhrungyU

This is to connect the ESP-12E module to Thingspeak, sending data from a slot sensor to a graph online, illustrating 
low cost IoT (Internet of Things) connectivity via WiFi and MQTT.

In the Arduino IDE go to Sketch/Include Library/Manage Libraries and include the "Adafruit MQTT Library".  The code here is based on 
one of the examples that you get with the libary.  Key differences are:

- The Slot class that I created to count slot events (i.e. when something goes in and out of the slot is one 'count')
- SPIFFS library is used to access the WiFi SSID and passphrase that I programmed there (you don't have to use SPIFFS if you dont want to)
- I simplified soem things (like the reconnect logic, and subscribe stuff that is not needed here)

Have fun, let me know how you get on.
Martyn




