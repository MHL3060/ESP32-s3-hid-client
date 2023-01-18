#include "Bluetooth.h"

void ClientCallbacks::onConnect(NimBLEClient *pClient)
{
    Serial.println("Connected");
    /** After connection we should change the parameters if we don't need fast response times.
     *  These settings are 150ms interval, 0 latency, 450ms timout.
     *  Timeout should be a multiple of the interval, minimum is 100ms.
     *  I find a multiple of 3-5 * the interval works best for quick response/reconnect.
     *  Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latency, 60 * 10ms = 600ms timeout
     */
    pClient->updateConnParams(120, 120, 0, 60);
};

void ClientCallbacks::onDisconnect(NimBLEClient *pClient)
{
    Serial.print(pClient->getPeerAddress().toString().c_str());
    Serial.println(" Disconnected - Starting scan");
    NimBLEDevice::getScan()->start(SCAN_TIME, scanEndedCB);
};

/** Called when the peripheral requests a change to the connection parameters.
 *  Return true to accept and apply them or false to reject and keep
 *  the currently used parameters. Default will return true.
 */
bool ClientCallbacks::onConnParamsUpdateRequest(NimBLEClient *pClient, const ble_gap_upd_params *params)
{
    return true;
};

/********************* Security handled here **********************
 ****** Note: these are the same return values as defaults ********/
uint32_t ClientCallbacks::onPassKeyRequest()
{
    Serial.println("Client Passkey Request");
    /** return the passkey to send to the server */
    return 123456;
};

bool ClientCallbacks::onConfirmPIN(uint32_t pass_key)
{
    Serial.print("The passkey YES/NO number: ");
    Serial.println(pass_key);
    /** Return false if passkeys don't match. */
    return true;
};

/** Pairing process complete, we can check the results in ble_gap_conn_desc */
void ClientCallbacks::onAuthenticationComplete(ble_gap_conn_desc *desc)
{
    if (!desc->sec_state.encrypted)
    {
        Serial.println("Encrypt connection failed - disconnecting");
        /** Find the client with the connection handle provided in desc */
        NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
        return;
    }
};