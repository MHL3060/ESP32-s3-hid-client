#ifndef MOUSE_BRIDGE_H
#define MOUSE_BRIDGE_H

#include "USBHIDMouse.h"

class MouseBridge
{

public:
    MouseBridge();
    void notifyHIDCB(uint8_t *pData, size_t length);
    void jump();

private:
    USBHIDMouse mouse;
    void move(int16_t x, int16_t y, int8_t wheel);
};
#endif