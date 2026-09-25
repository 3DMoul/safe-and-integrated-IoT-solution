## Introduction

This assignment presents a safe and integrated IoT solution, focusing on how connected devices can work together while protecting data, systems, and users. It explores practical design considerations for building a reliable, secure, and effective IoT environment.

Networkcommunication
---
broker: Mosquitto
ip address: 192.168.0.20
Port: 1883
Protokoll: MQTT
Topic: sensors/esp32-c6-01/temperature
Modell: Publish/Subscribe
--
wifi
Wi-Fi-namn (SSID): in wifisecrets.h
Wi-Fi-lösenord: in wifisecrets.h
--
harware
---
microcontroller
model: ESP32-C6
--
sensor
model: mcp9700e/a
unit: tempreture(C)
range: (-50 C) to (50 C)
note:: check with multimeter to se if you get right output





TESTS
---
testing if mqtt works:
i put this in the cmd terminal
-- "C:\Program Files\Mosquitto\mosquitto_sub.exe" -h 192.168.0.20 -p 1883 -t "sensors/esp32-c6-01/temperature" -v --
![alt text](image.png)
it prints out exactly how i want it