# ESP32 Smart Alarm Clock

Turn a cheap analogue alarm clock into a Wi-Fi-controlled smart alarm using an ESP32, a relay module, and a browser-based control panel.

This project keeps the clock's original alarm mechanism while adding programmable alarms, network time synchronization, and manual motor control through a web browser.

I built this project to explore how an ordinary household device could be made smarter without replacing its original mechanism. I now use the modified clock daily.

## Features

- **Wi-Fi connectivity** — Connects to your local wireless network.
- **Browser-based control panel** — Configure and manage alarms from a browser.
- **Programmable alarms** — Configure up to five alarm slots.
- **NTP time synchronization** — Retrieves the current time over the internet.
- **Manual motor control** — Switch the alarm mechanism on or off from the web interface.
- **Original alarm mechanism** — Retains the analogue clock's existing alarm hardware.

## Hardware

- ESP32-WROOM-32 development board
- SRD-05VDC-SL-C 5V relay module compatible with 3.3V ESP32 control signals
- Analogue alarm clock with an original battery-powered alarm motor
- Original AA battery
- WAGO connectors
- Jumper wires
- USB power supply for the ESP32

## How It Works

The ESP32 connects to Wi-Fi, synchronizes the time, and hosts a small web server.

From a browser on the same network, you can open the ESP32's IP address and configure alarms or manually control the motor.

When a scheduled alarm triggers, the ESP32 activates the relay. The relay switches the clock's original battery supply to the alarm motor, allowing the existing alarm mechanism to operate.

The software is designed to switch the motor off after five minutes.

## Software

The project uses the Arduino framework for ESP32 and the following libraries:

- `WiFi.h`
- `WebServer.h`
- `WiFiUdp.h`
- `NTPClient.h`

The first three are included with the relevant ESP32 Arduino core; `NTPClient` must be installed separately if it isn't already available.

## Setup

1. Install the Arduino IDE and the ESP32 board support package.
2. Install the `NTPClient` library through the Arduino Library Manager.
3. Open the project sketch.
4. Replace the Wi-Fi placeholders with your network credentials.
5. Select the appropriate ESP32 board and serial port.
6. Upload the sketch.
7. Open the Serial Monitor or your router's connected-device list to find the ESP32's IP address.
8. Enter that IP address in a browser on the same network.

Once connected, use the web interface to configure alarms and control the motor.

## Important Notes

- Verify the relay module's pinout, trigger polarity, and compatibility with 3.3V GPIO signals before connecting it.
- The relay contacts should switch the clock's motor supply without connecting the clock's battery circuit directly to an ESP32 GPIO.
- Alarm settings are currently stored in RAM and are lost when the ESP32 restarts.
- The current time-zone implementation requires improvement to handle daylight-saving changes correctly.
- The current software does not provide robust Wi-Fi recovery or persistent alarm storage.
- This is an experimental project. Test the alarm thoroughly before relying on it to wake you up.

## Future Improvements

- Automatic local time-zone and daylight-saving support
- Persistent alarm settings using non-volatile storage
- Improved Wi-Fi reconnection and startup behavior
- Better handling of overlapping alarms and manual motor control
- A more responsive web interface
- Improved reliability and fault detection

## License

Unless otherwise specified, all rights are reserved. If you'd like others to reuse, modify, and redistribute your code, consider adding an open-source license such as the MIT License.

## Author

Built as a personal DIY electronics and IoT project.

See the accompanying Medium article for the project story, hardware photos, and implementation details.
