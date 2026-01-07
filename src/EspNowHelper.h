#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Message types
#define MSG_TYPE_CONNECT 0
#define MSG_TYPE_STATUS 1
#define MSG_TYPE_DISCONNECT 2

typedef struct DeviceMessage {
    uint8_t id;
    uint8_t messageType;  // MSG_TYPE_CONNECT, MSG_TYPE_STATUS, MSG_TYPE_DISCONNECT
    bool isCalibrated;
} DeviceMessage;

class EspNowHelper {
  public:
    EspNowHelper();

    void begin(uint8_t* hubMacAddress, uint8_t deviceId);
    void sendConnected();
    void updateCalibration(bool newStatus);

  private:
    uint8_t* receiverAddress;
    DeviceMessage message;

    void sendCalibrationStatus();
    static void handleDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
};
