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

/** Notification / Indication receiving handler callback */
// Notification from 4c:75:25:xx:yy:zz: Service = 0x1812, Characteristic = 0x2a4d, Value = 1,0,0,0,0,
void notifyHIDCB(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
  std::string str = (isNotify == true) ? "Notification" : "Indication";
  str += " from ";
  /** NimBLEAddress and NimBLEUUID have std::string operators */
  str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
  str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
  str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
  str += ", Handle = 0x";
  Serial.print(str.c_str());
  Serial.print(pRemoteCharacteristic->getHandle());
  Serial.print(", Value = ");
  for (size_t i = 0; i < length; i++)
  {
    Serial.print(pData[i], HEX);
    Serial.print(',');
  }
  Serial.print(' ');
  switch (length)
  {
    uint16_t appearance;
  case 5:
    // https://github.com/wakwak-koba/ESP32-NimBLE-Mouse
    // returns 5 bytes per HID report
    Serial.printf("buttons: %02x, x: %d, y: %d, wheel: %d hwheel: %d",
                  pData[0], (int8_t)pData[1], (int8_t)pData[2], (int8_t)pData[3], (int8_t)pData[4]);
    break;
  case 6:
    // BLE Trackball Mouse from Amazon returns 6 bytes per HID report
    {
      trackball_mouse_t *my_mouse;
      my_mouse = (trackball_mouse_t *)pData;
      Serial.printf("buttons: %02x, x: %d, y: %d, wheel: %d",
                    my_mouse->buttons, my_mouse->x, my_mouse->y, my_mouse->wheel);
    }
    break;

  case 7:
    // Logitech BLE Mouse returns 7 bytes per HID report
    {
      logitech_mouse_t *my_mouse;
      my_mouse = (logitech_mouse_t *)pData;
      Serial.printf("buttons: %02x, x: %d, y: %d, wheel: %d, pan: %d",
                    my_mouse->buttons, my_mouse->x, my_mouse->y, my_mouse->wheel, my_mouse->pan);
    }
    break;
  case 9:
    if (appearance == APPEARANCE_HID_MOUSE)
    {
      // MS BLE Mouse returns 9 bytes per HID report

      ms_mouse_t *my_mouse;
      my_mouse = (ms_mouse_t *)pData;
      Serial.printf("buttons: %02x, x: %d, y: %d, wheel: %d, pan: %d",
                    my_mouse->buttons, my_mouse->x, my_mouse->y, my_mouse->wheel, my_mouse->pan);
    }
    else if (appearance == APPEARANCE_HID_GAMEPAD)
    {

      joystick_t *my_joystick;
      my_joystick = (joystick_t *)pData;
      Serial.printf("buttons: %02x, x: %d, y: %d",
                    my_joystick->buttons, my_joystick->x, my_joystick->y);
    }
    break;
  }

  Serial.println();
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
