#include <Arduino.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include "Bluetooth.h"
#include "MouseBridge.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 0;
#endif

static const int led_pin = LED_BUILTIN;

TaskHandle_t LEDTask;
BluetoothHID *bluetooth;
void setup()
{
  Serial.begin(115200);
  Serial.print("starting");
  MouseBridge bridge;
  bluetooth = new BluetoothHID(&bridge);
}

void loop()
{
  bluetooth->bluetoothLoop();
}
