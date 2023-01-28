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
MouseBridge *mouseBridge;

void scanEndedCB(NimBLEScanResults results)
{
  Serial.println("Scan Ended");
}

void printHex(unsigned char *pData, size_t length)
{
  for (size_t i = 1; i < length; i++)
  {
    if (i < length - 1)
    {
      Serial.print(pData[i], HEX);
      Serial.print(",");
    }
    else
    {
      Serial.print(pData[i], HEX);
      Serial.println();
    }
  }
}

/** Notification / Indication receiving handler callback */
// Notification from 4c:75:25:xx:yy:zz: Service = 0x1812, Characteristic = 0x2a4d, Value = 1,0,0,0,0,
void notifyHIDCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
  mouseBridge->notifyHIDCB(pData, length);
}

void setup()
{
  Serial.begin(115200);
  Serial.print("starting");
  mouseBridge = new MouseBridge();
  bluetooth = new BluetoothHID();
}

void loop()
{
  bluetooth->bluetoothLoop();
}
