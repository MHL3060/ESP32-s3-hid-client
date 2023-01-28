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
    for (size_t i = 3; i < length - 2; i++)
    {

        Serial.print(pData[i], HEX);
        Serial.print(",");
    }
    Serial.println();
}

inline int8_t _clamp(int v, int min, int max)
{
    return v > max ? max : v < min ? min
                                   : v;
}

typedef struct ble_mouse_t
{
    uint8_t buttons;
    int8_t unused;
    int8_t xRlative;
    int8_t yLeastXMost;
    int8_t yRelative;
    int8_t wheel;
    int8_t pan;

} ble_mouse;

void MouseBridge::notifyHIDCB(uint8_t *pData, size_t length)
{
    // is byte messed up?
    print_hex(pData, length);
    ble_mouse_t *my_mouse = (ble_mouse_t *)pData;

    uint8_t yLxM = my_mouse->yLeastXMost;
    int16_t x = ((yLxM & 0xF) << 8) + my_mouse->xRlative;
    int16_t y = ((yLxM >> 4) + my_mouse->yRelative << 4);
    // Serial.printf("yL%x, yR=%x combine=%x\n", yLxM >> 4, my_mouse->yRelative, (my_mouse->yRelative << 4 ) + );

    // Serial.printf("x=%d y=%d  shiftx=%d (%x) shifty=%d (%x)\n", my_mouse->x, my_mouse->y, my_mouse->x >> 8, my_mouse->x >> 8, my_mouse->y >> 8, my_mouse->y >> 8);
    //  mouse.move(my_mouse->x & 0xFF, my_mouse->y & 0xFF, my_mouse->wheel);
    /*
        auto msbx = my_mouse->x >> 8;
    auto xleft = msbx > 0 ? msbx : 0;
    auto msby = my_mouse->y >> 8;
    auto yleft = msby > 0 ? msby : 0;
    if (xleft != 0 || yleft != 0)
    {
        Serial.printf("xleft=%x yleft=%x", xleft, yleft);
        mouse.move(my_mouse->x - 0xFF, my_mouse->y - 0xFF, 0);
    }
    Mouse motion reports contain data with an X-translation and a Y-translation. These are transmitted as 12-bit signed integers.
     format used for mouse reports is the following byte array, where LSB/MSB is the least/most significant bit: [8 LSB (X), 4 LSB (Y) |  MSB(X),4 8 MSB(Y)].
    Therefore, FB0F00 denotes an X-translation of FFB = -5 (two’s complement format), which means a movement of 5 pixels to the left, and a Y-translation of 000 = 0.
    https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.5.1/nrf/samples/bluetooth/peripheral_hids_mouse/README.html#peripheral-hids-mouse
    */
    mouse.move(x, y, my_mouse->wheel);
    mouse.click(my_mouse->buttons);
    Serial.println();
}

void MouseBridge::move(int16_t x, int16_t y, int8_t wheel)
{
    int16_t xRemain = x;
    int16_t yRemain = y;
    bool xInc = x < 0;
    bool yInc = y < 0;

    while (xRemain != 0 || yRemain != 0)
    {

        int xNextMove = xRemain;
        int yNextMove = yRemain;
        if (xInc)
        {
            if (xRemain < -127)
            {
                xNextMove = -127;
                xRemain = xRemain + 127;
            }
            else
            {
                xRemain = 0;
            }
        }
        else
        {
            if (xRemain > 127)
            {
                xNextMove = 127;
                xRemain = xRemain - 127;
            }
            else
            {
                xRemain = 0;
            }
        }
        // y
        if (yInc)
        {
            if (yRemain < -127)
            {
                yNextMove = -127;
                yRemain = yRemain + 127;
            }
            else
            {
                yRemain = 0;
            }
        }
        else
        {
            if (yRemain > 127)
            {
                yNextMove = 127;
                yRemain = yRemain - 127;
            }
            else
            {
                yRemain = 0;
            }
        }
        // Serial.printf("x=%d y=%d", xNextMove, yNextMove);
        mouse.move(xNextMove, yNextMove, 0);
    }
    mouse.move(0, 0, wheel);
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