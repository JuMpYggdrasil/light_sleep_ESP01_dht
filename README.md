# light_sleep_ESP01_dht
   I'm using ESP-01S or ESP-01 to connect to DHT11 (well-known temp & humid sensor,pin 2).  
   ESP8266 gets the data and sends it to the server via MQTT protocol.  
   It uses Li-po 2000 mAh to supply power.  
   To reduce power consumption I put the ESP in light sleep mode and removed the LED on the board.  
     
     ##WORK FLOW  
     --wake up  
     --connect wifi   
     --connect mqtt broker  
     --read datas from sensor  
     --send datas to server  
     --sleep     
     
   credit https://www.mischianti.org/2019/11/21/wemos-d1-mini-esp8266-the-three-type-of-sleep-mode-to-manage-energy-savings-part-4/
