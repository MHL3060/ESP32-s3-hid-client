#ifndef BLUETOOTH_HID_H
#define BLUETOOTH_HID_H

#include <NimBLEDevice.h>

#define SCAN_TIME 0 /** 0 = scan forever */

#define APPEARANCE_HID_GENERIC 0x3C0
#define APPEARANCE_HID_KEYBOARD 0x3C1
#define APPEARANCE_HID_MOUSE 0x3C2
#define APPEARANCE_HID_JOYSTICK 0x3C3
#define APPEARANCE_HID_GAMEPAD 0x3C4
#define APPEARANCE_HID_DIGITIZER_TABLET 0x3C5
#define APPEARANCE_HID_CARD_READER 0x3C6
#define APPEARANCE_HID_DIGITAL_PEN 0x3C7
#define APPEARANCE_HID_BARCODE_SCANNER 0x3C8
#define APPEARANCE_HID_TOUCHPAD 0x3C9
#define APPEARANCE_HID_PRESENTATION_REMOTE 0x3CA

const char HID_SERVICE[] = "1812";
const char HID_INFORMATION[] = "2A4A";
const char HID_REPORT_MAP[] = "2A4B";
const char HID_CONTROL_POINT[] = "2A4C";
const char HID_REPORT_DATA[] = "2A4D";

static uint8_t connectPhys = BLE_GAP_LE_PHY_CODED_MASK | BLE_GAP_LE_PHY_1M_MASK | BLE_GAP_LE_PHY_2M_MASK;

/** Callback to process the results of the last scan or restart it */
void scanEndedCB(NimBLEScanResults results);

class ClientCallbacks : public NimBLEClientCallbacks
{
public:
    void onConnect(NimBLEClient *pClient);
    void onDisconnect(NimBLEClient *pClient);
    bool onConnParamsUpdateRequest(NimBLEClient *pClient, const ble_gap_upd_params *params);
    uint32_t onPassKeyRequest();
    bool onConfirmPIN(uint32_t pass_key);
    void onAuthenticationComplete(ble_gap_conn_desc *desc);
};

class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
public:
    void onResult(NimBLEAdvertisedDevice *advertisedDevice);
    bool shouldConnect();
    NimBLEAdvertisedDevice *getCandidatedadDevice();
    void removeCandidate();
    virtual ~AdvertisedDeviceCallbacks();

private:
    NimBLEAdvertisedDevice *candidateDevice;
    bool shouldProcessed;
};

void bluetoothSetup();

void bluetoothLoop();

#endif