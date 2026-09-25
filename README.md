## Introduction

This assignment presents a safe and integrated IoT solution, focusing on how connected devices can work together while protecting data, systems, and users. It explores practical design considerations for building a reliable, secure, and effective IoT environment.
----////////////////////////////////////////////////////////////----

--///////////////////////////--
--// Network communication //--
--///////////////////////////--
------

broker: Mosquitto
ip address: 192.168.0.20
Port: 1883
Protocol: MQTT
Topic: sensors/esp32-c6-01/temperature
Modell: Publish/Subscribe


----////////////////////////////////////////////////////////////----


--///////////////////--
--//     Wi-Fi     //--
--///////////////////--
------

Wi-Fi name (SSID): in wifisecrets.h
Wi-Fi password: in wifisecrets.h


----////////////////////////////////////////////////////////////----


--///////////////////--
--//    hardware    //--
--///////////////////--
------

--//microcontroller//--
model: ESP32-C6

---

--//sensor//--
model: MCP9700E/A
unit: temperature(C)
range: (-50 C) to (50 C)
note: check with multimeter to se if you get right output


----////////////////////////////////////////////////////////////----


--/////////////////--
--//    TESTS    //--
--/////////////////--
------

--//Test the MQTT connection//--
I put this in the command terminal
--|| "C:\Program Files\Mosquitto\mosquitto_sub.exe" -h 192.168.0.20 -p 1883 -t "sensors/esp32-c6-01/temperature" -v ||--
![alt text](image.png)
it prints out exactly how i want it

---

--//Test that the sensor responds to temperature changes//--
I start the program and let it run for a while, then I blow hot air toward the sensor
Normal:
![alt text](image-1.png)
Blowing hot air
![alt text](image-2.png)

---

--//Test Wi-Fi recovery//--
Here i disable the internet acces and then turn it on
disabled:
![alt text](image-3.png)
enabled:
![alt text](image-4.png)

---

--//Test MQTT from another machine//--
this is from my laptop and the program is on my stationary:
![alt text](image-5.png)

---

--//Check the JSON//--
looking at how the JSON payload looks like 
it should look like this:

Topic: sensors/esp32-c6-01/temperature
Payload:
{
"device_id": "Temperature.01",
"temperature": 13.50,
"voltage": 0.635
}

it looks like this
![alt text](image-6.png)

----////////////////////////////////////////////////////////////----