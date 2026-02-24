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

void updateLEDS() {
  if (!currentSwitchState) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

// Return true if the module is calibrated (switch LOW)
bool isCalibrated() {
  updateLEDS();

  return !currentSwitchState;
}

void setup() {
  Serial.begin(115200);

  espNowHelper.begin(DEVICE_ID);
  espNowHelper.addPeer(hubAddress);
  espNowHelper.sendModuleConnected(hubAddress);

  // Hardware Initialization
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  currentSwitchState = digitalRead(SWITCH_PIN);
  lastSwitchState = currentSwitchState;

  // Start with both LEDs off
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Send initial calibration status
  espNowHelper.sendModuleUpdated(hubAddress, isCalibrated());
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
      espNowHelper.sendModuleUpdated(hubAddress, isCalibrated());
    }
  }

  lastSwitchState = reading;
}
