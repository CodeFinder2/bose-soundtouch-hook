# Bose Soundtouch Software Architecture
This summarizes findings about the Bose Soundtouch software architecture based on analysis on a Soundtouch 10 (ST10) speaker.

## Binaries

### STSCertified
- Relevant for notification as it asks the cloud if a notification is "allowed"
- Seems to talk to `BoseApp` via Protobuf on port 30030


### BoseApp
- Seems to listen to requests on 127.0.0.1:30030 based on protobuf format
- Most probably some sort of star topology where many other binaries (STSCertified, TPDA, Bluetooth, more?) communicate with this central app

### TPDA
...

### Bluetooth
...