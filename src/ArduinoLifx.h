#ifndef ArduinoLifx_h
#define ArduinoLifx_h

#include "Arduino.h"
#include <Udp.h>

#define LIFX_PROTOCOL_NUMBER 1024
#define LIFX_GETSTATE 101
#define LIFX_SETCOLOR 102
#define LIFX_SETWAVEFORM 103
#define LIFX_SETWAVEFORM_OPTIONAL 119
#define LIFX_GETPOWER 116
#define LIFX_SETPOWER 117
#define LIFX_GETINFRARED 120
#define LIFX_SETINFRARED 122
#define LIFX_GETSTATE 101
#define LIFX_SETCOLOR 102
#define LIFX_SETWAVEFORM 103
#define LIFX_SETWAVEFORM_OPTIONAL 119
#define LIFX_GETPOWER 116
#define LIFX_SETPOWER 117
#define LIFX_GETINFRARED 120
#define LIFX_SETINFRARED 122

#define LIFX_UDP_PORT 56700
#define LIFX_HEADER_LENGTH 36
#define LIFX_MAX_PACKET_LENGTH 53


class ArduinoLifx
{
public:
  ArduinoLifx(UDP& udp);
  void begin(uint32_t sourceIP, const char* targetIP, const char* mac);
  void setFlags(bool tagged=1, bool ack=0, bool res=0);
  void setSequenceNumber(uint8_t sequenceNum);
  void setSource(int32_t sourceAddress);
  void setTarget(const char* targetAddress);
  void setDestination(const char* addr);
  void getState();

  void setColor(uint16_t hue,
    uint16_t saturation,
    uint16_t brightness,
    uint16_t kelvin,
    uint32_t duration);

  void setWaveform(uint8_t transient,
    uint16_t hue,
    uint16_t saturation,
    uint16_t brightness,
    uint16_t kelvin,
    uint32_t period,
    float cycles,
    uint32_t skew,
    uint8_t waveform,
    uint8_t setHue,
    uint8_t setSat,
    uint8_t setBri,
    uint8_t setKelvin);

    void getPower();
    void setPower(uint16_t level, uint32_t duration);
    void getInfrared();
    void setInfrared(uint16_t irBrightness);
    void sendPacket(int msgType, const char* addr, int thisPort=LIFX_UDP_PORT);
    void flushPacket();

    private:
      UDP*        _udp;                                   // reference to transport
      uint8_t     _message[LIFX_MAX_PACKET_LENGTH];
      int         _packetLength;
      uint8_t     _sequenceNum;
      const char* _targetIP;
      void setMessageLength(uint16_t length);
      void setMessageType(int msgType);
      void finishPacket(int msgType);
      void resetPayload();
      uint16_t convertHue(int h);
    };
    #endif
