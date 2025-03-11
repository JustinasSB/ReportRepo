#pragma once
#include "esphome.h"
#include <SparkFun_SCD4x_Arduino_Library.h>

class SCD4xSensor : public PollingComponent {
 public:
  Sensor *co2_sensor = new Sensor();
  Sensor *temperature_sensor = new Sensor();
  Sensor *humidity_sensor = new Sensor();

  SCD4xSensor() : PollingComponent(60000) {}

  Sensor *get_co2_sensor() { return co2_sensor; }
  Sensor *get_temperature_sensor() { return temperature_sensor; }
  Sensor *get_humidity_sensor() { return humidity_sensor; }

  void setup() override {
    if (!scd4x.begin()) {
      ESP_LOGE("scd4x", "SCD4x sensor not detected!");
    } else {
      ESP_LOGI("scd4x", "SCD4x sensor initialized");
    }
  }

  void update() override {
    if (scd4x.readMeasurement()) {
      float co2 = scd4x.getCO2();
      float temp = scd4x.getTemperature();
      float hum = scd4x.getHumidity();
      co2_sensor->publish_state(co2);
      temperature_sensor->publish_state(temp);
      humidity_sensor->publish_state(hum);
    } else {
      ESP_LOGW("scd4x", "Failed to read measurement");
    }
  }

 private:
  SCD4x scd4x;
};
