#include <Arduino.h>
#include <FreeRTOS.h>
#include "MouseBridge.h"
#include "Bluetooth.h"
#include "USBHIDMouse.h"
#include "USB.h"
#include "HIDMouse.h"
#define LSB(v) ((v >> 8) & 0xff)
#define MSB(v) (v & 0xff)

void print_hex(unsigned char *pData, size_t length)
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

    for (size_t i = 1; i < length; i++)
    {
        if (i < length - 1)
        {
            Serial.printf("%d", pData[i]);
            Serial.print(",");
        }
        else
        {
            Serial.printf("%d", pData[i]);
            Serial.println();
        }
    }
}

inline int8_t _clamp(int v, int min, int max)
{
    return v > max ? max : v < min ? min
                                   : v;
}
void MouseBridge::notifyHIDCB(uint8_t *pData, size_t length)
{
    logitech_mouse_t *my_mouse;
    my_mouse = (logitech_mouse_t *)pData;
    print_hex(pData, length - 2);
    Serial.print(my_mouse->x, HEX);
    Serial.print(" ");
    Serial.print(my_mouse->y, HEX);
    Serial.println();
    mouse.move(_clamp(my_mouse->x, -10, 10), _clamp(my_mouse->y, -10, 10), my_mouse->wheel, 0);
    mouse.click(my_mouse->buttons);
}

void MouseBridge::jump()
{
    mouse.click(1);
}

MouseBridge::MouseBridge()
{
    Serial.print("initialize usb");
    mouse.begin();
    USB.begin();
}