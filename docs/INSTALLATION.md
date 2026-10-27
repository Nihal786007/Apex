# Build, upload and first connection

## Required versions and files

Use a classic ESP32/WROOM development board. Existing compatible installation: **Arduino-ESP32 3.3.12** and **U8g2 2.36.19**. V5 intentionally requires Arduino-ESP32 major version 3. WiFi, WebServer, Wire and FreeRTOS come with that board package. No ESP32Servo library is needed; the existing LEDC approach is retained.

Keep these four files together in `firmware/APEX_Ultimate_V5/`:

1. APEX_Ultimate_V5.ino
2. Config.h
3. ControlCore.h
4. Dashboard.h

Opening only the `.ino` without its headers causes missing-file errors. The source is self-contained; the browser needs no internet/CDN assets.

## Arduino IDE steps

1. Disconnect the motor supply and secure the robot. Confirm Config.h says `MOTOR_POWER_VERIFIED = false`.
2. Connect ESP32 by a data-capable USB cable.
3. In Boards Manager install **esp32 by Espressif Systems**, 3.x. If needed, the official index is `https://espressif.github.io/arduino-esp32/package_esp32_index.json`.
4. In Library Manager install **U8g2 by oliver**.
5. Open APEX_Ultimate_V5.ino in its matching folder. Select the exact matching classic ESP32 board (typically ESP32 Dev Module) and COM port.
6. Click Verify. Read [BUILD_REPORT.md](BUILD_REPORT.md) for the checked configuration; other boards/pins have not been validated.
7. Click Upload. If Connecting persists, check cable/port, then hold BOOT as required by that development board while upload connects; release when writing begins.
8. Open Serial Monitor at 115200 baud and reset. Expect version, BENCH, OLED status and Wi-Fi details. A FATAL initialization message means no successful ready state; investigate it before continuing.
9. Join APEX-CONTROL, password Apex2026Robot. Accept staying connected without internet. Open **http://192.168.4.1** with HTTP, not HTTPS.
10. Refresh after every update to load the current JavaScript. If the phone switches to mobile data or a VPN interferes, keep this local connection selected.

## Bench operation

BENCH uses real range/IR/servo/OLED hardware and real FSM decisions, but suppresses physical motor PWM. Arm and START are available for supervised logic checks. The UI/OLED clearly label BENCH. Requested L/R may change; applied PWM must remain 0/0. This is not a physical movement test.

On startup/reconnect, power command is zero and the robot is disarmed. Select a mode, choose power, then Arm. Manual requires a held direction. Autonomous modes additionally require START. Mode switching stops/disarms; PAUSE and DISARM also require a deliberate Arm/Start sequence to resume. Calibrate IR before line START.

## Physical enable gate

Complete every required rating/measurement/jumper check in POWER_AND_WIRING.md first. Change `MOTOR_POWER_VERIFIED` to true only after that evidence is recorded, rebuild, and repeat the raised-wheel stage. This gate is not a sensor or remote safety certification. There is no dashboard bypass.

Settings/calibration are RAM-only and reset on reboot. This prevents silently trusting an old calibration after hardware changes. Update Config.h only for confirmed permanent values; runtime IR captures remain required before line START.

## CLI and developer tests

From the repository root with board/library packages installed:

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/APEX_Ultimate_V5
node tests/dashboard_test.cjs
cc -std=c99 tests/control_core_test.c -o control_core_test
./control_core_test
```

On Windows, use a suitable native compiler and run the resulting `.exe`. A portable TinyCC 0.9.27 was used for the local exact-core assertions. This is a development dependency only and is not bundled into firmware or the repository archive.

For a UI-only preview, run `node tests/preview_server.cjs` then visit `http://127.0.0.1:8765`. It serves **simulated telemetry** and cannot connect to the robot. Do not use the preview fixture's mock server as proof of real firmware behavior.
