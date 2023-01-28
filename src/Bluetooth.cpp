
/*
 * This program is based on https://github.com/h2zero/NimBLE-Arduino/tree/master/examples/NimBLE_Client.
 */

// Install NimBLE-Arduino by h2zero using the IDE library manager.
#include <NimBLEDevice.h>
#include "Bluetooth.h"
#include "MouseBridge.h"

BluetoothHID::BluetoothHID()
{

    /** Initialize NimBLE, no device name spcified as we are not advertising */
    NimBLEDevice::init("");

    /** Set the IO capabilities of the device, each option will trigger a different pairing method.
     *  BLE_HS_IO_KEYBOARD_ONLY    - Passkey pairing
     *  BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
     *  BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
     */
    // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY); // use passkey
    // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison

    /** 2 different ways to set security - both calls achieve the same result.
     *  no bonding, no man in the middle protection, secure connections.
     *
     *  These are the default values, only shown here for demonstration.
     */
    NimBLEDevice::setSecurityAuth(true, true, true);
    // NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

    /** Optional: set the transmit power, default is 3db */
    NimBLEDevice::setPower(ESP_PWR_LVL_P3); /** +9db */
    /** create new scan */
    NimBLEScan *pScan = NimBLEDevice::getScan();

    pAdvertisedDeviceCallBack = new AdvertisedDeviceCallbacks();
    /** create a callback that gets called when advertisers are found */
    pScan->setAdvertisedDeviceCallbacks(pAdvertisedDeviceCallBack);

    /** Set scan interval (how often) and window (how long) in milliseconds */
    pScan->setInterval(45);
    pScan->setWindow(15);

    /** Active scan will gather scan response data from advertisers
     *  but will use more energy from both devices
     */
    pScan->setActiveScan(true);
    /** Start scanning for advertisers for the scan time specified (in seconds) 0 = forever
     *  Optional callback for when scanning stops.
     */
    pScan->start(SCAN_TIME, scanEndedCB);
}

void BluetoothHID::bluetoothLoop()
{
    /** Loop here until we find a device we want to connect to */
    if (!pAdvertisedDeviceCallBack->shouldConnect())
    {
        return;
    }
    NimBLEAdvertisedDevice *pCandidateDevice = pAdvertisedDeviceCallBack->getCandidatedadDevice();
    pAdvertisedDeviceCallBack->removeCandidate();

    /** Found a device we want to connect to, do it now */
    if (connectToServer(pCandidateDevice))
    {
        Serial.println("Success! we should now be getting notifications!");
    }
    else
    {
        Serial.println("Failed to connect, starting scan");
        NimBLEDevice::getScan()->start(SCAN_TIME, scanEndedCB);
    }
}

/** Create a single global instance of the callback class to be used by all clients */

/** Handles the provisioning of clients and connects / interfaces with the server */
bool BluetoothHID::connectToServer(NimBLEAdvertisedDevice *advDevice)
{
    NimBLEClient *pClient = nullptr;
    bool reconnected = false;

    Serial.printf("Client List Size: %d\r\n", NimBLEDevice::getClientListSize());
    /** Check if we have a client we should reuse first **/
    if (NimBLEDevice::getClientListSize())
    {
        /** Special case when we already know this device, we send false as the
         *  second argument in connect() to prevent refreshing the service database.
         *  This saves considerable time and power.
         */
        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
        if (pClient)
        {
            Serial.println("Reconnect try");
            if (!pClient->connect(advDevice, false))
            {
                Serial.println("Reconnect failed");
                return false;
            }
            Serial.println("Reconnected client");
            reconnected = true;
        }
        /** We don't already have a client that knows this device,
         *  we will check for a client that is disconnected that we can use.
         */
        else
        {
            Serial.print("disconnect client");
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    /** No client to reuse? Create a new one. */
    if (!pClient)
    {
        if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS)
        {
            Serial.println("Max clients reached - no more connections available");
            return false;
        }

        pClient = NimBLEDevice::createClient();

        Serial.println("New client created");

        pClient->setClientCallbacks(&clientCB, false);
        /** Set initial connection parameters: These settings are 15ms interval, 0 latency, 120ms timout.
         *  These settings are safe for 3 clients to connect reliably, can go faster if you have less
         *  connections. Timeout should be a multiple of the interval, minimum is 100ms.
         *  Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latency, 51 * 10ms = 510ms timeout
         */
        pClient->setConnectionParams(12, 12, 0, 51);
        /** Set how long we are willing to wait for the connection to complete (seconds), default is 30. */
        pClient->setConnectTimeout(5);

        if (!pClient->connect(advDevice))
        {
            /** Created a client but failed to connect, don't need to keep it as it has no data */
            NimBLEDevice::deleteClient(pClient);
            Serial.println("Failed to connect, deleted client");
            return false;
        }
    }

    if (!pClient->isConnected())
    {
        if (!pClient->connect(advDevice))
        {
            Serial.println("Failed to connect");
            return false;
        }
    }

    Serial.print("Connected to: ");
    Serial.println(pClient->getPeerAddress().toString().c_str());
    Serial.print("RSSI: ");
    Serial.println(pClient->getRssi());

    /** Now we can read/write/subscribe the charateristics of the services we are interested in */
    NimBLERemoteService *pSvc = nullptr;
    pSvc = pClient->getService(HID_SERVICE);
    if (!handleService(pSvc, reconnected))
    {
        /** Disconnect if subscribe failed */
        Serial.println("subscribe notification failed");
        pClient->disconnect();
    }
    Serial.println("Done with this device!");
    return true;
}

bool BluetoothHID::handleService(NimBLERemoteService *pSvc, boolean reconnected)
{

    NimBLERemoteCharacteristic *pChr = nullptr;
    NimBLERemoteDescriptor *pDsc = nullptr;

    if (pSvc)
    { /** make sure it's not null */
        if (!reconnected)
        {

            // This returns the HID report descriptor like this
            // HID_REPORT_MAP 0x2a4b Value: 5,1,9,2,A1,1,9,1,A1,0,5,9,19,1,29,5,15,0,25,1,75,1,
            // Copy and paste the value digits to http://eleccelerator.com/usbdescreqparser/
            // to see the decoded report descriptor.
            pChr = pSvc->getCharacteristic(HID_REPORT_MAP);
            if (pChr)
            { /** make sure it's not null */

                // Serial.print("HID_REPORT_MAP ");
                if (pChr->canRead())
                {
                    std::string value = pChr->readValue();
                    Serial.print(pChr->getUUID().toString().c_str());
                    /*Serial.print(" Value: ");
                    uint8_t *p = (uint8_t *)value.data();
                    for (size_t i = 0; i < value.length(); i++)
                    {
                        Serial.print(p[i], HEX);
                        Serial.print(',');
                    }
                    Serial.println();
                    */
                }
            }
            else
            {
                Serial.println("HID REPORT MAP char not found.");
            }
        }

        // Subscribe to characteristics HID_REPORT_DATA.
        // One real device reports 2 with the same UUID but
        // different handles. Using getCharacteristic() results
        // in subscribing to only one.
        std::vector<NimBLERemoteCharacteristic *> *charvector;
        charvector = pSvc->getCharacteristics(true);
        for (auto &it : *charvector)
        {
            if (it->getUUID() == NimBLEUUID(HID_REPORT_DATA))
            {
                Serial.println(it->toString().c_str());
                if (it->canNotify())
                {

                    if (!it->subscribe(true, notifyHIDCB))
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    return false;
}
