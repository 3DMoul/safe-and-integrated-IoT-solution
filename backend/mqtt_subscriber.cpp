#include <mosquitto.h>
#include "mqtt_subscriber.hpp"
#include <iostream>
#include <string>

#include "json_reading.hpp"

static const char* MQTT_HOST = "192.168.0.20";
static const int MQTT_PORT = 1883;
static const char* MQTT_TOPIC = "sensors/esp32-c6-01/temperature";

void on_connect(struct mosquitto* mosq, void* userdata, int result) {
    if (result == 0) {
        std::cout << "Connected to MQTT broker\n";

        int rc = mosquitto_subscribe(
            mosq,
            nullptr,
            MQTT_TOPIC,
            1
        );

        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr
                << "Subscribe failed: "
                << mosquitto_strerror(rc)
                << '\n';
        }
    } 
    else {
        std::cerr
            << "MQTT connection failed: "
            << mosquitto_connack_string(result)
            << '\n';
    }
}

void on_message(struct mosquitto*, void*, const struct mosquitto_message* message) {
    if (!message || !message->payload) {
        return;
    }

    std::string payload(
        static_cast<const char*>(message->payload),
        message->payloadlen
    );

    std::cout << "\nMQTT message received:\n"
              << payload << '\n';

    try {
        Reading reading = parse_reading(payload);

        std::cout
            << "Valid reading:\n"
            << "Sensor: " << reading.sensor_id << '\n'
            << "Value: " << reading.value << " C\n";

    } catch (const std::exception& error) {
        std::cerr
            << "Invalid sensor data: "
            << error.what()
            << '\n';
    }
}

void start_mqtt_subscriber() {
    mosquitto_lib_init();

    struct mosquitto* mosq =
        mosquitto_new(
            "iot-backend",
            true,
            nullptr
        );

    if (!mosq) {
        std::cerr << "Could not create MQTT client\n";
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    std::cout
        << "Connecting to MQTT broker "
        << MQTT_HOST << ':' << MQTT_PORT
        << "...\n";

    int rc = mosquitto_connect(
        mosq,
        MQTT_HOST,
        MQTT_PORT,
        60
    );

    if (rc != MOSQ_ERR_SUCCESS) {
        std::cerr
            << "Connection error: "
            << mosquitto_strerror(rc)
            << '\n';

        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    rc = mosquitto_loop_forever(
        mosq,
        -1,
        1
    );

    if (rc != MOSQ_ERR_SUCCESS) {
        std::cerr
            << "MQTT loop error: "
            << mosquitto_strerror(rc)
            << '\n';
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

}