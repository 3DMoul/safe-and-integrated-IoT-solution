#pragma once

#include <string>

struct Reading {
    std::string sensor_id;
    double value;
    std::string unit;
};

Reading parse_reading(const std::string& text);

std::string serialize_reading(const Reading& reading);