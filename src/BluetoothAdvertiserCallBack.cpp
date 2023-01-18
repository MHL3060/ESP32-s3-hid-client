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
        /** Ready to connect now */
        shouldProcessed = true;
    }
};
void AdvertisedDeviceCallbacks::removeCandidate()
{
    shouldProcessed = false;
    candidateDevice = NULL;
}

bool AdvertisedDeviceCallbacks::shouldConnect()
{
    return shouldProcessed;
}

NimBLEAdvertisedDevice *AdvertisedDeviceCallbacks::getCandidatedadDevice()
{
    return candidateDevice;
}
