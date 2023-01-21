#include <Arduino.h>
#include <FreeRTOS.h>
#include "MouseBridge.h"
#include "Bluetooth.h"
#include "USBHIDMouse.h"
#include "USB.h"

void MouseBridge::notifyHIDCB(uint8_t *pData, size_t length)
{
    hid_mouse_report_t report;
    memset(&report, 0, sizeof(report));
    if (length == 6)
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
    if (length == 7)
    {

        logitech_mouse_t *my_mouse;
        my_mouse = (logitech_mouse_t *)pData;

        report.x = (int8_t) * (int16_t *)&pData[2];
        report.y = (int8_t) * (int16_t *)&pData[3];
        report.buttons = my_mouse->buttons;
        report.wheel = my_mouse->wheel;
        report.pan = my_mouse->pan;

        Serial.printf("buttons: %02x, x: %d, y: %d, wheel: %d, pan: %d (x: %d, y: %d)",
                      report.buttons, report.x, report.y, report.wheel, report.pan, (int8_t) * (int16_t *)&pData[2], (int8_t) * (int16_t *)&pData[3]);

        mouse.move(report.x, report.y, report.wheel, report.pan);
        mouse.click(report.buttons);
    }
    printf("length=%d\n", length);
}

MouseBridge::MouseBridge()
{
    Serial.print("initialize usb");
    mouse.begin();
    USB.begin();
}