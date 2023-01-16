#include <Arduino.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include "Bluetooth.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 0;
#endif

static const int led_pin = LED_BUILTIN;

bool ledState = false;
void toggleLED(void *param)
{
  while (1)
  {
    delay(1500);
    digitalWrite(led_pin, HIGH);
    delay(1500);
    digitalWrite(led_pin, LOW);
  }
}

TaskHandle_t LEDTask;

void setup()
{
  Serial.begin(115200);
  Serial.print("starting");
  bluetooth_setup();
  xTaskCreatePinnedToCore(
      &toggleLED,
      "toggle",
      6000,
      NULL,
      10,
      &LEDTask,
      app_cpu);
}

void loop()
{
  bluetooth_loop();
}