#include <Arduino.h>
#include <FreeRTOS.h>
#include "MouseBridge.h"
#include "USBHIDMouse.h"

void MouseBridge::notifyHIDCB(uint8_t *pData, size_t length)
{
    hid_mouse_report_t report;
    memset(&report, 0, sizeof(report));
    if (length == 6 || length == 7)
    {
        // BLE Trackball Mouse from Amazon returns 6 bytes per HID report

        report.buttons = pData[0];
        report.x = (int8_t) * (int16_t *)&pData[1];
        report.y = (int8_t) * (int16_t *)&pData[3];
        report.wheel = (int8_t)pData[5];
    }
    else if (length == 5)
    {
        // https://github.com/wakwak-koba/ESP32-NimBLE-Mouse
        // returns 5 bytes per HID report
        memcpy(&report, pData, (sizeof(report) < length) ? sizeof(report) : length);

        // mouse.hid.SendReport(HID_REPORT_ID_MOUSE, &report, sizeof(report))
    }
    mouse.move(report.x, report.y, report.wheel, report.pan);
    mouse.click(report.buttons);
}

MouseBridge::MouseBridge()
{
    Serial.print("initialize usb");
    mouse.begin();
}