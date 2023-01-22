#include <Arduino.h>
#include <FreeRTOS.h>
#include "MouseBridge.h"
#include "Bluetooth.h"
#include "USBHIDMouse.h"
#include "USB.h"

inline int8_t _clamp(int v, int min, int max)
{
    return v > max ? max : v < min ? min
                                   : v;
}
void MouseBridge::notifyHIDCB(uint8_t *pData, size_t length)
{
    logitech_mouse_t *my_mouse;
    my_mouse = (logitech_mouse_t *)pData;
    mouse.move(_clamp(my_mouse->x, -35, 35), _clamp(my_mouse->y, -35, 35), my_mouse->wheel, my_mouse->pan);
    mouse.click(my_mouse->buttons);
}

MouseBridge::MouseBridge()
{
    Serial.print("initialize usb");
    mouse.begin();
    USB.begin();
}