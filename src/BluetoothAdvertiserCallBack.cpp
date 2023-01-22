#include "Bluetooth.h"

void AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice *advertisedDevice)
{
    if ((advertisedDevice->getAdvType() == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD) ||
        (advertisedDevice->getAdvType() == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD) ||
        (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(NimBLEUUID(HID_SERVICE))))
    {
        Serial.printf("onResult: AdvType= %d\r\n", advertisedDevice->getAdvType());
        Serial.print("Advertised HID Device found: ");
        Serial.println(advertisedDevice->toString().c_str());

        /** stop scan before connecting */
        NimBLEDevice::getScan()->stop();
        /** Save the device reference in a global for the client to use*/
        candidateDevice = advertisedDevice;
    }
    else
    {
        Serial.printf("onResult: AdvType= %d\r\n", advertisedDevice->getAdvType());
        Serial.println(advertisedDevice->toString().c_str());
    }
};
void AdvertisedDeviceCallbacks::removeCandidate()
{
    candidateDevice = NULL;
}

bool AdvertisedDeviceCallbacks::shouldConnect()
{
    return candidateDevice != NULL;
}

NimBLEAdvertisedDevice *AdvertisedDeviceCallbacks::getCandidatedadDevice()
{
    return candidateDevice;
}

AdvertisedDeviceCallbacks::~AdvertisedDeviceCallbacks()
{
    candidateDevice = NULL;
}
