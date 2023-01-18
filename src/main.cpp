#include <Arduino.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include "Bluetooth.h"
#include "Control.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 0;
#endif

static const int led_pin = LED_BUILTIN;

TaskHandle_t LEDTask;

void setup()
{
  Serial.begin(115200);
  Serial.print("starting");

  bluetooth_setup();
}

void loop()
{
  bluetooth_loop();
}