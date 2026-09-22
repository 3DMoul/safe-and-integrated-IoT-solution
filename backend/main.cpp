#include "json_reading.hpp"
#include <iostream>

int main() {
    Reading reading = parse_reading(
        R"({"sensorId":"sensor-1","value":23.5,"unit":"C"})"
    );

    std::cout << serialize_reading(reading) << '\n';
}