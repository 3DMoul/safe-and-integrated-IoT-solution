#pragma once

#include <mutex>
#include <optional>
#include "json_reading.hpp"

void start_mqtt_subscriber(std::optional<Reading>& latest, std::mutex& mutex);