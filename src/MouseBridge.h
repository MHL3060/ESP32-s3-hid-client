#ifndef MOUSE_BRIDGE_H
#define MOUSE_BRIDGE_H

#include "USBHIDMouse.h"

class MouseBridge
{

public:
    MouseBridge();
    void notifyHIDCB(uint8_t *pData, size_t length);

private:
    USBHIDMouse mouse;
};
#endif