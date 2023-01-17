#include <NimBLEDevice.h>

void bluetooth_setup();
void bluetooth_loop();
bool connectToServer();
void scanEndedCB(NimBLEScanResults results);