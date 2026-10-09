## Introduction

This assignment presents a safe and integrated IoT solution, focusing on how connected devices can work together while protecting data, systems, and users. It explores practical design considerations for building a reliable, secure, and effective IoT environment.
----////////////////////////////////////////////////////////////----

--///////////////////////////--
--//  startup instructions //--
--///////////////////////////--
------

--//software requirements//--
ESP-IDF installed and configured for ESP32-C6.
Mosquitto installed as a Windows service.
MSYS2 UCRT64 with g++, the Mosquitto development library, and the JSON dependencies.
---

Check if Mosquitto is running
|-
Get-Service -Name mosquitto
|-

If Mosquitto is not running open up adminastrator powershell and write this
|-
Start-Service -Name mosquitto
|-
---

Start with build and flash to the esp this should open up monitoring for the esp code on terminal 1
This command will compile the esp32 firmware, upload the firmware to your esp32c6 and then display esp32 
|-
cd esp32
idf.py build flash monitor
|-
---

compile backend
|-
g++ backend/api.cpp backend/mqtt_subscriber.cpp backend/json_reading.cpp `
  -Ibackend `
  -I/ucrt64/include `
  -L/ucrt64/lib `
  -lmosquitto `
  -lws2_32 `
  -pthread `
  -o backend/backend.exe
|-

to start backend functions like api and mqtt subscriber write this in terminal 2 this can be done form the project root directory.
|-
.\backend\backend.exe
|-
---

to check for api latest reading and health open terminal 3

for health
|-
curl.exe http://127.0.0.1:8085/health 
|-

for latest reading 
|-
curl.exe http://127.0.0.1:8085/api/readings/latest 
|-

----////////////////////////////////////////////////////////////----


--///////////////////////////--
--//      Background       //--
--///////////////////////////--

--// visual representation of the project code //--
safe-and-integrated-IoT-solution/
│
├── esp32/
│   ├── main/
│   │   └── ... ESP32 firmware code
│   └── wifisecrets.h
│
├── backend/
│   ├── api.cpp
│   ├── mqtt_subscriber.cpp
│   ├── mqtt_subscriber.hpp
│   ├── json_reading.cpp
│   ├── json_reading.hpp
│   ├── httplib.h
│   ├── nlohmann/
│
└── README.md
--// visual representation of the code in effect //--

MCP9700A Temperature Sensor
          |
          | Analog voltage
          v
       ESP32-C6
          |
          | Reads ADC
          | Calculates temperature
          | Creates JSON
          |
          | Wi-Fi / MQTT
          v
    Mosquitto Broker
    192.168.0.20:1883
          |
          | MQTT subscription
          v
   mqtt_subscriber.cpp
          |
          | Receives JSON payload
          v
    json_reading.cpp
          |
          | Parses and validates JSON
          v
    Latest valid reading
    (Shared backend state)
          |
          | Accessed by api.cpp
          v
        REST API
    127.0.0.1:8085
          |
          | HTTP GET
          v
       API Client
    (curl / browser)

--//JSON formating//--

{
"sensorId":"Temperature.01",
"value":13.5,
"unit":"C"
}


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
--// JSON/XML data //--
--///////////////////--
------

----////////////////////////////////////////////////////////////----

--///////////////////--
--//      API      //--
--///////////////////--
------


GET /health
curl.exe http://127.0.0.1:8085/health

this endpint is checking if the api is running. if it is runnning it outputs "ok". 

GET /api/readings/latest
curl.exe http://127.0.0.1:8085/api/readings/latest



POST /api/readings	
curl.exe -X POST http://127.0.0.1:8085/api/readings `
  -H "Content-Type: application/json" `
  -d '{\"sensorId\":\"Temperature.01\",\"value\":23.5,\"unit\":\"C\"}'

This is to send a reading manualy

this is a valid JSON reading
![alt text](valid reading.png)

Invalid JSON syntax
![alt text](Invalid syntax.png)

Invalid field value or type
![alt text](Invalid field type.png)


----////////////////////////////////////////////////////////////----

--///////////////////--
--//   Security    //--
--///////////////////--
------

----////////////////////////////////////////////////////////////----

--////////////////////////////////--
--//    Logging & monitoring    //--
--////////////////////////////////--
------

----////////////////////////////////////////////////////////////----

--///////////////////--
--//     Wi-Fi     //--
--///////////////////--
------

Wi-Fi name (SSID): in wifisecrets.h
Wi-Fi password: in wifisecrets.h

--//Wi-Fi connection//--
The ESP32-C6 uses the Wi-Fi name (SSID) and password stored in wifisecrets.h. When Wi-Fi starts, the WIFI_EVENT_STA_START event triggers esp_wifi_connect() to connect to the network. Once connected and assigned an IP address, IP_EVENT_STA_GOT_IP is triggered. The program then sets WIFI_CONNECTED_BIT, allowing the program to continue with MQTT and sensor readings.

--//Wi-Fi recovery//--
If the Wi-Fi connection is lost, WIFI_EVENT_STA_DISCONNECTED is triggered this happens automatically by the ESP32-C6 Wi-Fi system. The program automatically calls esp_wifi_connect() to attempt reconnection. This allows the ESP32-C6 to reconnect without restarting the device.

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
![alt text](MQTT connection test.png)
it prints out exactly how i want it

---

--//Test that the sensor responds to temperature changes//--

I start the program and let it run for a while, then I blow hot air toward the sensor
Normal:
![alt text](normal test.png)
Blowing hot air
![alt text](blowing hot air test.png)

---

--//Test Wi-Fi recovery//--

Here i disable the internet acces and then turn it on
disabled:
![alt text](disabled wifi.png)
enabled:
![alt text](enabled wifi.png)

---

--//Test MQTT from another machine//--

this is from my laptop and the program is on my stationary:
![alt text](another machine MQTT test.png)

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
![alt text](JSON check.png)
//this is only for the testing this is not the final JSON formating
---

--//Testing that parse_reading() and serialize_reading() works//--

#include "json_reading.hpp"
#include <iostream>

int main() {
    Reading reading = parse_reading(
        R"({"sensorId":"sensor-1","value":23.5,"unit":"C"})"
    );

    std::cout << serialize_reading(reading) << '\n';
}

this code is a placeholder just to test the functions

I used this command to compile and make into exe --|| g++ main.cpp json_reading.cpp -I. -o app.exe ||--
Then i ran the exe with this command --|| .\app.exe ||--                                                  
It gave the expected output.
{                                                                                                             
    "sensorId":"sensor-1",                                                                                                         
    "value": 23.5
    "unit": "C",            
}

---
--//    Deliberate error test    //--

    1. MQTT connection failure
    Ran  -| Stop-Service -Name mosquitto |- in Administrator PowerShell its start sending signals that it cant connect and it stops sending new readings to the api/backend
    ![alt text](MQTT connection failure 1.png)
    Here i ran  -| start-Service -Name mosquitto |- in Administrator PowerShell it starts working again like normal
    ![alt text](MQTT connection failure 2.png)

    - Expected result: What did you expect the ESP32 and backend to do when the broker stopped?
    -- Answer: i expected that the esp32 can still read the temperature sensor. And it will still try to publish MQTT messages. but without Mosquitto it will not be able to deliver those messages to the broker.

    - Reason: Why did stopping Mosquitto interrupt communication?
    -- Answer: Mosquitto is a middleman between the esp32 and the backend. so when i stopped Mosquitto there was no way for the readings to get to the backend.

    - Recovery: Did the ESP32 and backend reconnect automatically when you restarted Mosquitto, or did you need to restart either program? 
    -- Answer: the ESP32 and backend reconnect automatically and i did not have to restart the program.
    
    2. Invalid JSON syntax test
    I send and intentionaly wrong reading
    -|  
        & "C:\Program Files\Mosquitto\mosquitto_pub.exe" `
        -h 192.168.0.20 `
        -p 1883 `
        -t "sensors/esp32-c6-01/temperature" `
        -m '{"sensorId":"Temperature.01","value":"NOT_A_NUMBER","unit":"C"}'
    |-
    ![alt text](Invalid JSON syntax test.png)

    - Did the backend print Invalid sensor data or another error?
    -- Answer: the backend printed this error {sensorId:Temperature.01,value:NOT_A_NUMBER,unit:C}
                                            Invalid sensor data: [json.exception.parse_error.101] parse error at line 1, column 2: syntax error while parsing object key - invalid literal; last read: '{s'; expected string literal
    - Why is "NOT_A_NUMBER" invalid for the value field?
    -- Answer: Because the missing quotation marks makes the message invalid. and even if it was valid it would get a error because its expecting a number not a string.
    - Did the backend continue running after receiving the invalid message?
    -- Answer: Yes
    - Did valid readings from the ESP32 continue arriving afterward?
    -- Answer: Yes
    - Was the invalid value prevented from replacing the latest valid reading?
    -- Answer: Yes


--//

----////////////////////////////////////////////////////////////----