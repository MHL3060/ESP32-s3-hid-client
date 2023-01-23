#ifndef MOUSE_BRIDGE_H
#define MOUSE_BRIDGE_H

#include "USBHIDMouse.h"

class MouseBridge
{

public:
    MouseBridge();
    void notifyHIDCB(uint8_t *pData, size_t length);
    void jump();
    void move(int8_t x, int8_t y);

private:
    USBHIDMouse mouse;
};
#endif