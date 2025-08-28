# wCK_Series Arduino Library

## Overview

The **wCK_Series** library provides an easy-to-use interface for controlling wCK servo modules via Arduino. It supports position, speed, torque, and configuration commands, as well as reading feedback from the servos. The library is designed for modular robotics and automation projects using wCK servos.

## Table of Contents

- [Installation](#installation)
- [Supported Hardware](#supported-hardware)
- [Usage](#usage)
- [API Reference](#api-reference)
- [Examples](#examples)
- [Changelog](#changelog)
- [License](#license)

## Installation

1. Download or clone this repository.
2. Open Arduino IDE.
3. Go to **Sketch** > **Include Library** > **Add .ZIP Library...** and select the downloaded folder.
4. The library will be available under **Sketch** > **Include Library**.

## Supported Hardware

- wCK Series servo modules (e.g., wCK-XX, wCK-XXH)
- Arduino boards with HardwareSerial support

## Usage

1. Include the library in your sketch:
    ```cpp
    #include <wCK_Series.h>
    ```
2. Create a `wCK` object, passing a pointer to your HardwareSerial port:
    ```cpp
    wCK servo(&Serial1);
    ```
3. Initialize the serial communication:
    ```cpp
    servo.begin(WCK_115200_BAUD_RATE);
    ```
4. Use the provided API to control and read from your servos.

## API Reference

### Class: `wCK`

#### Constructor
- `wCK(HardwareSerial *SerialPort);`

#### Initialization
- `void begin(unsigned int BaudRate, uint8_t config = Null, uint8_t rx = -1, uint8_t tx = -1);`

#### Actions
- `Response_packet PosSend(char ServoID, char TorqueLevel, char Position);`
- `bool PosSendH(char ServoID, char TorqueLevel, int Position);`
- `void SyncPosSend(char LastID, char TorqueLevel, char *TargetArray);`
- `Response_packet Rotation360(char ServoID, char SpeedLevel, char RotationDir);`
- `char PassiveCK(char ServoID);`
- `char BreakWCK(void);`

#### Setters
- `bool setBaudrate(char ServoID, char NewBaud);`
- `bool setPDGain(char ServoID, char *NewPgain, char *NewDgain);`
- `bool setIGain(char ServoID, char *NewIgain);`
- `bool setRuntimePDGain(char ServoID, char *NewPgain, char *NewDgain);`
- `bool setRuntimeIGain(char ServoID, char *NewIgain);`
- `bool setId(char ServoID, char NewId);`
- `bool setSpeed(char ServoID, char NewSpeed, char NewAccel);`
- `void setRuntimeSpeed(char ServoID, char NewSpeed, char NewAccel);`
- `bool setOverLoad(char ServoID, char NewOverLoad);`
- `bool setBoundary(char ServoID, char *NewLBound, char *NewUBound);`

#### Getters
- `Response_packet getPos(char ServoID);`
- `int getPosH(char ServoID);`
- `char getPDGain(char ServoID, char *Pgain, char *Dgain);`
- `char getIGain(char ServoID, char *Igain);`
- `char getSpeed(char ServoID, char* NewSpeed, char *NewAccel);`
- `char getOverLD(char ServoID);`
- `char getBound(char ServoID, char *LBound, char *UBound);`

#### Data Structures
- `Response_packet` struct: contains `load` and `position`.

#### Constants
See `wCK_Series.h` for baud rate, overload, and rotation direction constants.

## Examples

### Basic Usage

```cpp
#include <wCK_Series.h>

wCK servo(&Serial1);

void setup() {
    servo.begin(WCK_115200_BAUD_RATE);
    Response_packet feedback = servo.PosSend(1, 0, 128); // Move servo 1 to position 128 with max torque
    int pos = servo.getPosH(1); // Get high-precision position
}

void loop() {
    // Your code here
}
```

### Synchronous Position Control

```cpp
#include <wCK_Series.h>

wCK servo(&Serial1);
char targets[4] = {100, 120, 140, 160};

void setup() {
    servo.begin(WCK_115200_BAUD_RATE);
    servo.SyncPosSend(3, 0, targets); // Move servos 0-3 to target positions
}

void loop() {
    // Your code here
}
```

## Changelog

### v1.0.0
- Initial release
- Full API for wCK servo control
- Position, speed, torque, and configuration commands
- Feedback reading support

## License

MIT License

Copyright (c) 2024 HYPERION ROBOTICS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

The author of this software shall not be held liable for any damages, liabilities, or legal consequences
arising from the use, misuse, or inability to use the software.
## Changelog

### Enhancements
- **Improved Peer Management**: Fixed issues with adding multiple peers, ensuring stable and reliable communication.
- **Generic Send Function**: Refactored the `Send` function to be more flexible, allowing any data type to be sent using a single function (`Send`) instead of separate functions for each type.
- **Custom Queue Implementation**: Introduced a dynamic message queue for storing messages, and optimizing memory usage.
- **User-defined Callbacks**: Added functionality for users to set their own custom data send and receive functions, increasing flexibility.
- **Encryption Enhancements**: Fixed encryption problems and added support for both PMK and LMK keys to improve security.
- **Support for AP and STA Modes**: Enhanced the class to support both Station (STA) and Access Point (AP) modes, which were previously limited to STA only.
- **Channel Communication Fixes**: Correctly implemented message sending across different channels, not just channel 1.
- **Board Support**: Added esp32 `2.0.27` board version compatibility together with the `3.x.x` board versions.
- **Sending Types**: Introduced custom struct message communication.

### Bug Fixes
- Resolved issues with sending messages to peers on channels other than channel 1.
- Fixed encryption issues to ensure proper data security.
- Fixed the unknown variable type sending

## Warnings

This library can only work with these board versions of the esp32 board manager in arduino:
- `3.x.x`
- `2.0.17`

## License

MIT License

Copyright (c) 2024 HYPERION ROBOTICS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

The author of this software shall not be held liable for any damages, liabilities, or legal consequences
arising from the use, misuse, or inability to use the software.
