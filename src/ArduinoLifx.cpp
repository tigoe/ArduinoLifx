#include "Arduino.h"
#include "ArduinoLifx.h"

ArduinoLifx::ArduinoLifx(UDP& udp) {
  this->_udp = &udp;
  memset(_message, 0, LIFX_MAX_PACKET_LENGTH);   // initialize _message with zeroes
  _packetLength = LIFX_HEADER_LENGTH;
  _sequenceNum = 0;
}

void ArduinoLifx::begin(uint32_t sourceIP, const char* targetIP, const char* mac){
  setFlags();
  setSource(sourceIP);
  setTarget(mac);
  setDestination(targetIP);
  this->_udp->begin(LIFX_UDP_PORT);
}

void ArduinoLifx::setFlags(bool tagged, bool ack, bool res) {
  // tagged is bit 5 of byte 3 of the header:
  _message[3] =  (tagged << 5); // tagged is bit 5
  _message[3] = _message[3] | (1 << 4); // addressable is bit 4
  _message[2] = lowByte(LIFX_PROTOCOL_NUMBER);
  _message[3] = _message[3] | highByte(LIFX_PROTOCOL_NUMBER);

  // ack required and res required are bits 1 and 0 of byte 22:
  _message[22] = (ack << 1) | res;
}

void ArduinoLifx::setSequenceNumber(uint8_t sequenceNum) {
  // set the sequence number in the header:
  _message[23] = sequenceNum;
}

void ArduinoLifx::setSource(int32_t sourceAddress) {
  // source, 4 bytes. This should be unique to your source (i.e. this device):
  // goes in bytes 4-7 of the header:
  for (int i = 0; i < 4; i++) {
    uint8_t b = sourceAddress >> (i * 8);
    _message[i+4] = b;
  }
}

// TODO: capture 0x at the beginning of the string, and
// fill the rest of the string with zeroes if needed.
void ArduinoLifx::setTarget(const char* targetAddress) {
  // for all messages other than getService, this should be
  // the MAC address of the target device (i.e. the light):
  // bytes 8 - 13 of the header:
  int bytePos = 13;
  // iterate in steps of two chars, so you can convert
  // each two-char substring to an unsigned long:
  for (int c =strlen(targetAddress)-1; c>=0; c -= 2) {
    // skip the : characters:
    if (targetAddress[c] == ':') {
      c++;
      continue;
    }
    char temp[] = {targetAddress[c-1], targetAddress[c], 0}; // get the temp string
    byte b = strtoul(temp, NULL, 16);     // convert it
    _message[bytePos--] = b;               // add the byte to the array
  }
}

void ArduinoLifx::setDestination(const char* addr) {
  this->_targetIP = addr;
}

void ArduinoLifx::setMessageLength(uint16_t length) {
  // first two bytes are length:
  _message[0] = lowByte(length);
  _message[1] = highByte(length);
}

void ArduinoLifx::setMessageType(int msgType) {
  // message type, 2 bytes, little endian:
  _message[32] = lowByte(msgType);
  _message[33] = highByte(msgType);
}

void ArduinoLifx::finishPacket(int msgType) {
  // finalize details:
  setMessageType(msgType);
  setMessageLength(_packetLength);
  setSequenceNumber(_sequenceNum++);
  sendPacket(msgType, _targetIP, LIFX_UDP_PORT);
  resetPayload();
}

void ArduinoLifx::resetPayload() {
  // reset the payload for next message:
  for (int i=LIFX_HEADER_LENGTH; i<LIFX_MAX_PACKET_LENGTH; i++) {
    _message[i] = 0;
  }
  _packetLength = LIFX_HEADER_LENGTH;
}

void ArduinoLifx::getState() {
  // no payload
  finishPacket(LIFX_GETSTATE);
}

void ArduinoLifx::setColor(uint16_t hue, uint16_t saturation,
  uint16_t brightness, uint16_t kelvin, uint32_t duration) {
    uint16_t newHue = convertHue(hue);
    // payload: reserved, HSBK, duration:
    _message[_packetLength++] = 0;   // reserved byte
    _message[_packetLength++] = lowByte(newHue);
    _message[_packetLength++] = highByte(newHue);
    _message[_packetLength++] = lowByte(saturation);
    _message[_packetLength++] = highByte(saturation);
    _message[_packetLength++] = lowByte(brightness);
    _message[_packetLength++] = highByte(brightness);
    _message[_packetLength++] = lowByte(kelvin);
    _message[_packetLength++] = highByte(kelvin);
    // duration is 32 bits, convert to bytes:
    for (int i = 0; i < 4; i++) {
      uint8_t b = duration >> (i * 8);
      _message[_packetLength++] = b;
    }

    finishPacket(LIFX_SETCOLOR);
  }


  void ArduinoLifx::setWaveform(uint8_t transient, uint16_t hue,
    uint16_t saturation, uint16_t brightness, uint16_t kelvin,
    uint32_t period, float cycles, uint32_t skew, uint8_t waveform,
    uint8_t setHue, uint8_t setSat, uint8_t setBri, uint8_t setKelvin) {
      // payload: reserved, transient, period, cycles, skew, waveform
      // optional payload: also included hue, sat, bri, kelvin
      // payload: reserved, HSBK, duration
      uint16_t newHue = convertHue(hue);
      _message[_packetLength++] = 0;
      _message[_packetLength++] = transient;
      _message[_packetLength++] = lowByte(newHue);
      _message[_packetLength++] = highByte(newHue);
      _message[_packetLength++] = lowByte(saturation);
      _message[_packetLength++] = highByte(saturation);
      _message[_packetLength++] = lowByte(brightness);
      _message[_packetLength++] = highByte(brightness);
      _message[_packetLength++] = lowByte(kelvin);
      _message[_packetLength++] = highByte(kelvin);
      _message[_packetLength++] = lowByte(period);
      _message[_packetLength] = highByte(period);
      // convert float to bytes:
      unsigned int cyclesConverted = *((int*)&cycles);

      for (int i = 0; i < 4; i++) {
        uint8_t b = cyclesConverted >> (i * 8) & 0xFF;
        _message[_packetLength++] = b;
      }
      _message[_packetLength++] = lowByte(skew);
      _message[_packetLength++] = highByte(skew);
      _message[_packetLength] = waveform;


      if (setHue || setSat || setBri || setKelvin) {
        _packetLength++;              // because we terminated with waveform, above
        _message[_packetLength++] = setHue;
        _message[_packetLength++] = setSat;
        _message[_packetLength++] = setBri;
        _message[_packetLength] = setKelvin;
        // if any of the options are set, this is a setWaveformOptional msg:
        finishPacket(LIFX_SETWAVEFORM_OPTIONAL);
      } else {
        finishPacket(LIFX_SETWAVEFORM);
      }
    }

    void ArduinoLifx::getPower() {
      // no payload
      finishPacket(LIFX_GETPOWER);
    }

    void ArduinoLifx::setPower(uint16_t level, uint32_t duration) {
      // payload: level, duration
      // can only be on or off, so:
      if (level > 0) level = 0xffff;
      _message[_packetLength++] = lowByte(level);
      _message[_packetLength++] = highByte(level);

      for (int i = 0; i < 4; i++) {
        uint8_t b = duration >> (i * 8);
        _message[_packetLength++] = b;
      }
      _packetLength--;    // terminate with the right length

      finishPacket(LIFX_SETPOWER);
    }

    void ArduinoLifx::getInfrared() {
      // no payload
      finishPacket(LIFX_GETINFRARED);
    }

    void ArduinoLifx::setInfrared(uint16_t irBrightness) {
      // payload: IR brightness
      _message[_packetLength++] = lowByte(irBrightness);
      _message[_packetLength] = highByte(irBrightness);     // terminate

      finishPacket(LIFX_SETINFRARED);
    }

    uint16_t ArduinoLifx::convertHue(int h) {
      return (h * 65535 / 360);
    }

    void ArduinoLifx::sendPacket(int msgType, const char* addr, int thisPort) {
      // start a new packet:
      if (this->_udp != 0) {       // make sure there's a UDP transport
      this->_udp->beginPacket(addr, thisPort);          // start packet
      // NOTE: don't include the whole message, only as long as the
      // current packet length.
      this->_udp->write(_message, _packetLength);  // add payload to it
      this->_udp->endPacket();                          // finish and send packet
    }
  }
