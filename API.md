#  ArduinoLifx  Library for Arduino

This is a work in progress.

This library is for controlling LIFX WiFi-controlled lighting products. It allows you to send UDP packets carrying LIFX control messages as specified by the https://lan.developer.lifx.com/docs/introduction[LIFX LAN protocol]. In version 0.0.1, it only sends LIFX Light messages, not Device messages or Multizone messages. It provides no API for reading responses yet either. It relies on the WiFiUDP or EthernetUDP libraries.

The Library uses the Arduino WiFiUDP library, but it can work with either MKR1000 and other Arduino boards using the WiFi101 library, or ESP8266 boards using the ESP8266WiFi library.

For more detail on the LIFX protocol, see the https://lan.developer.lifx.com/docs/introduction[LIFX developer documents]

#  Commands

##  ArduinoLifx

### Syntax
````
WiFiUDP Udp;                // instance of UDP library
ArduinoLifx lifx(Udp);      // instance of the library
````

### Parameters
takes a UDP object for transport

### Returns
void

### Notes
Initializes the LIFX packet.


##  begin

### Syntax
````
char targetMac[] = "d0:73:d5:02:c5:3e";   // MAC address of the target
char target[] = "192.168.0.12";           // IP address of the target
IPAddress myIp = WiFi.localIP();          // your address
lifx.begin(myIp, target, targetMac);      // begin

````

### Parameters
targetMac[] - a hexadecimal character string containing the MAC address of the target device (i.e. the light you want to control). The string can be separated by colons or not, as you choose. LIFX bulbs have their MAC address printed on the side as their serial number.

target[] - a  decimal character string containing the IP address of the target device. This must include periods between bytes as delimiters.

myIP - an IPAddress containing the IP address of your board.

### Returns
void

### Notes
Initializes the headers of the LIFX packet.

##  setFlags

### Syntax
````
lifx.setFlags(tagged, ackRequired, resRequired);
````

### Parameters
tagged - a  boolean determining whether this message is tagged for a specific target device, or is a broadcast packet for all the targets on the local network. If tagged is 1, then you must include the MAC address and IP address of the target. If tagged is 0, then you can set the target MAC address to all zeroes, and the target IP address to xx.xx.xx.255, and all the target devices on your network will respond to the commands you send.

ackRequired - a boolean determining whether an acknowledgement from the target is required.


resRequired - a boolean determining whether a response message from the target is required.

### Returns
void

### Notes
For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/header-description[LIFX LAN Protocol Header Description page]

##  setSequenceNumber

### Syntax
````
lifx.setSequenceNumber(sequenceNumber);
````

### Parameters
sequenceNumber - a byte containing the sequence number of this message. If the ackRequired flag is set, then the target device will return the sequence number in its acknowledgement, allowing you to distinguish which messages that your target is responding to.

### Returns
void

### Notes
For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/header-description[LIFX LAN Protocol Header Description page]

##  setSource

### Syntax
````
lifx.setSource(myAddress);
````

### Parameters
myAddress - an IPAddress that's your source IP address.

### Returns
void

### Notes

##  setTarget
### Syntax
````
lifx.setTarget(targetMac[]);
````

### Parameters

targetMac[] - a hexadecimal character string containing the MAC address of the target device (i.e. the light you want to control). The string can be separated by colons or not, as you choose. LIFX bulbs have their MAC address printed on the side as their serial number.

### Returns
void

### Notes

##  setDestination
### Syntax
````
lifx.setDestination(target[]);
````

### Parameters
target[] - a  decimal character string containing the IP address of the target device. This must include periods between bytes as delimiters.


### Returns
void

### Notes

##  getState
### Syntax
````
lifx.getState();
````

### Parameters
none

### Returns
void

### Notes
Asks the target device to return its state. For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page]

##  setColor
### Syntax
````
lifx.setColor(hue, saturation, brightness, colorTemperature, duration);
````

### Parameters
hue - an unsigned int from 0 -360, using standard HSB values (0 = red, 120=green, 240=blue).

saturation - an unsigned int from 0-65535

brightness - an unsigned int from 0-65535

colorTemperature - an unsigned int from 0-65535 representing the color temperature in degrees kelvin

duration - an unsigned int representing the number of milliseconds for the change

### Returns
void

### Notes
Sets the color and brightness of the light. For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page]

##  setWaveform
### Syntax
````
lifx.setWaveform(transient, hue, saturation, brightness, color, period, cycles, skew, waveform, setHue, setSaturation, setBrightness, setColorTemperature);
````

### Parameters
transient - an int, value 0 or 1, representing whether or not the color persists.

hue - an unsigned int from 0 -360, using standard HSB values (0 = red, 120=green, 240=blue), representing the final hue.

saturation - an unsigned int from 0-65535, representing the final saturation.

brightness - an unsigned int from 0-65535,  representing the final brightness.

colorTemperature - an unsigned int from 0-65535 representing the color temperature in degrees kelvin, representing the final color temperature.

period - unsigned int representing the of a cycle in milliseconds.

cycles - float representing the number of cycles for the effect.

skew - an int representing the waveform Skew. -32768 to 32767] are mapped to  0 to 1.

waveform - a byte representing the waveform type. See the https://lan.developer.lifx.com/v2.0/docs/waveforms[Waveform page]. for more.

Optional parameters:

setHue - a byte as 0 or 1 representing whether the hue should be set from the light's existing value.

setSaturation -  a byte as 0 or 1 representing whether the saturation should be set from the light's existing value.

setBrightness -  a byte as 0 or 1 representing whether the brightness should be set from the light's existing value.

setColorTemperature -  a byte as 0 or 1 representing whether the color temperature should be set from the light's existing value.

### Returns
void

### Notes
Sets a waveform and parameters to be used for an effect. Depending on whether you set the last four parameters, this sends either the SET_WAVEFORM command or SET_WAVEFORM_OPTIONAL command. For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page] and the https://lan.developer.lifx.com/v2.0/docs/waveforms[Waveform page].

##  getPower
### Syntax
````
lifx.getPower();
````

### Parameters
none

### Returns
void

### Notes
Sets the power of a LIFX light. Doesn't return a value, but the target light will send back a UDP packet with the return message. For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page]

##  setPower
### Syntax
````
lifx.setPower(level, duration);
````

### Parameters
level - an unsigned int representing the power level. 0 will turn the light off, any other level will turn it on.

duration - an unsigned int representing the number of milliseconds for the change
### Returns
void

### Notes
Sets the power of a LIFX light.  For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page]

##  getInfrared
### Syntax
````
lifx.getInfrared();
````

### Parameters
none

### Returns
void

### Notes
Gets the infrared brightness of a LIFX light, when that property is available.  Doesn't return a value, but the target light will send back a UDP packet with the return message.  For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page]

##  setInfrared
### Syntax
````
lifx.setInfrared(irBrightness);
````

### Parameters
irBrightness - an unsigned int representing the infrared brightness, when that property is available.

### Returns
void

### Notes
Sets the infrared brightness of a LIFX light, when that property is available. For more on this command and its settings, see the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page]

##  sendPacket
### Syntax
````
lifx.sendPacket(msgType, target[], port);
````

### Parameters
messageType - an integer representing the message type. See the  https://lan.developer.lifx.com/docs/light-messages[LIFX LAN Protocol Light Messages Description page] for a list of the message type values.

target[] - a  decimal character string containing the IP address of the target device. This must include periods between bytes as delimiters.

port - an integer representing the target port. Defaults to 56700

### Returns
void

### Notes
Sends a LIFX packet to the target. This is normally sent by each of the Light messages automatically. It's public so that it can be used for diagnostic purposes only.

##  flushPacket
### Syntax
````
lifx.flushPacket();
````

### Parameters
none

### Returns
void

### Notes
Clears the current LIFX packet values. Public for diagnostic purposes only.
