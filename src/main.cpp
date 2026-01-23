#include <Arduino.h>
#include <shared_hardware_config.h>

#include "EspNowHelper.h"
#include "hardware_config.h"

#define DEBOUNCE_DELAY 50

uint8_t hubAddress[] = HUB_MAC_ADDRESS;
EspNowHelper espNowHelper;

bool lastSwitchState = LOW;
bool currentSwitchState = LOW;
unsigned long lastDebounceTime = 0;

// Return true if the module is calibrated (switch LOW)
bool isCalibrated() {
  return !currentSwitchState;
}

void setup() {
  Serial.begin(115200);

  espNowHelper.begin(hubAddress, DEVICE_ID);
  espNowHelper.sendModuleConnected();

  // Hardware Initialization
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  currentSwitchState = digitalRead(SWITCH_PIN);
  lastSwitchState = currentSwitchState;

  // Send initial calibration status
  espNowHelper.sendModuleUpdated(isCalibrated());
}

void loop() {
  bool reading = digitalRead(SWITCH_PIN);

  // Debounce logic
  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != currentSwitchState) {
      currentSwitchState = reading;

      // Update calibration status
      espNowHelper.sendModuleUpdated(isCalibrated());
    }
  }

  lastSwitchState = reading;
}
