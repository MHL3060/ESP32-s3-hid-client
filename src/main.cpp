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

struct Button
{
  const uint8_t PIN;
  uint32_t numberKeyPressed;
  bool on;
};

bool ledState = false;
unsigned long last_button_time = 0;
unsigned long button_time = 0;

TaskHandle_t LEDTask;
Button button = {18, 0, false};

void toggleLED(void *param)
{
  while (1)
  {
    if (button.on)
    {
      digitalWrite(led_pin, HIGH);
      connectToServer();
    }
    else
    {
      digitalWrite(led_pin, LOW);   
    }
    delay(10);
  }
}

void IRAM_ATTR isr()
{
  button_time = millis();
  if (button_time - last_button_time > 50)
  {
    button.on = !button.on;
    button.numberKeyPressed++;
    last_button_time = button_time;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.print("starting");

  pinMode(button.PIN, INPUT_PULLUP);
  attachInterrupt(button.PIN, isr, FALLING);
  xTaskCreatePinnedToCore(
      &toggleLED,
      "toggle",
      6000,
      NULL,
      10,
      &LEDTask,
      app_cpu);
  bluetooth_setup();
}

void loop()
{
  bluetooth_loop();
}