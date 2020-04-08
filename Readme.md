# BLINDS 433Mhz Controller

This is an example of a blinds 433Mhz based controller. It receives 
commands from the MQTT Broker and transmit control commands to
blinds through a RF 433Mhz transmitter.

The framework requires the presence of a file named "/config.json" 
located in the device SPIFFS flash file system. To do so, please 

1. Update the supplied data/config.json.sample file to your
   network configuration parameters
2. Rename it to data/config.json
3. Kill the PlatformIO Serial Monitor
4. Launch the "Upload File System Image" task of the PlatformIO IDE.
