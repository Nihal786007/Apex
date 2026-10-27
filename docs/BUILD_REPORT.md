# V5 verification report — 5.0.0-rc1

Date: **25 October 2026**, Asia/Calcutta. Motor output gate: **BENCH / false**.

## Full Arduino build: PASS

- Target: `esp32:esp32:esp32`, classic ESP32 Dev Module.
- Installed Arduino-ESP32: **3.3.12**.
- Installed U8g2: **2.36.19**.
- Final build exit code: **0**.
- Program storage: **1,003,479 / 1,310,720 bytes (76%)**.
- Global variables: **51,196 / 327,680 bytes (15%)**.
- Reported remaining memory for local variables: **276,484 bytes**. This compile estimate is not a runtime heap/stack endurance measurement.

No upload or physical actuation was performed. The first build also passed; the final build includes the 1024 px tablet spacing adjustment.

## Final source SHA-256

| File | SHA-256 |
|---|---|
| APEX_Ultimate_V5.ino | `74093098DEE63574B1AF60C65C53100988CFC4290C168FBB5A66D8A8D2A034F0` |
| Config.h | `A09F365DC755AC759F37AA3517E6EB692026817577923E5E8085CA74706E58B8` |
| ControlCore.h | `97059AA27BDA7D98D908262E165C604D33144DF03002DE65AD6DA064CC358231` |
| Dashboard.h | `C90455E9C31DBEE16F42CF5AE114085DC7D7A35F6649373E09A5D8FA2434D93F` |

These hashes identify the delivered default configuration. Any later personal calibration, credential or power-gate edit requires a rebuild and new evidence.

## Native exact-core tests: PASS

**232 assertions** in tests/control_core_test.c executed against the actual ControlCore.h implementation, compiled as portable C99 with TinyCC 0.9.27 on Windows. The same header compiled as C++ in the complete ESP32 build. The test compiler was downloaded from the project's official GNU mirror, hash-checked, and kept outside the deliverable repository.

Coverage includes startup, sessions, repeat/out-of-order sequences, stale epochs after stop/release/fault, mode-change stopping, manual hold timeout despite lease renewal, separate autonomous lease/run/motor-command limits, range freshness and hysteresis, IR capture/polarity, left/right corrections, both centered geometries, bounded line loss/recovery, persistent IR noise, stopped scans/settling/sample consistency, both turn directions, no blind reverse when trapped, oscillation/recovery limits, fixed-sensor restrictions, uint32 timer wrap and E-STOP in all modes.

Assertions include fixture checks and are not presented as 232 independent hardware scenarios.

## Browser and configuration tests: PASS

- tests/dashboard_test.cjs executes the actual embedded script against a mocked DOM/network. It checks request ordering, mode selection without stale release interference, captured epochs, STOP queue priority, fault retention, active-slider preservation, bench requested/applied separation and connection-loss stop signaling.
- tests/hardware_contract_test.cjs checks all **13** required GPIO assignments, retained polarity, requested known servo calibration, AP credentials and BENCH default.
- Mocked browser tests do not exercise real Wi-Fi timing, FreeRTOS scheduling, echo interrupts or L298N hardware.

## Rendered browser previews

Actual embedded HTML was rendered in the in-app browser against tests/preview_server.cjs. Every saved preview is labeled **SIMULATED TELEMETRY / NO ROBOT CONNECTED**.

| Viewport | Observed document dimensions | Result |
|---|---|---|
| 1280×800 | 1280×800 | Main landscape layout fits without page scrolling; tuning collapsed |
| 1024×768 | 1024×768 | Smaller tablet layout fits without page scrolling; tuning collapsed |
| 390×844 | Width 375 px excluding browser scrollbar; page height about 1826 px | Stacked layout, vertical scrolling; no horizontal overflow; E-STOP within header bounds |

Screenshots are in assets/. Opening tuning, accessibility enlargement, browser chrome or a software keyboard can require scrolling. No claim is made about every device or assistive-technology combination. SVG connection/power diagrams passed XML parsing; they are explanatory overviews, not validated production schematics.

## Reproduce

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/APEX_Ultimate_V5
node tests/dashboard_test.cjs
node tests/hardware_contract_test.cjs
cc -std=c99 tests/control_core_test.c -o control_core_test
./control_core_test
```

Windows convenience: `tests/run_tests.ps1 -Compiler <path-to-compatible-C-compiler>`.

## Evidence boundary

### 27 October 2026 — source-recovery compile-only recheck

The installed Arduino IDE CLI compiled the **unchanged current V5 BENCH** package for `esp32:esp32:esp32`, Arduino-ESP32 **3.3.12**, U8g2 **2.36.19**, exit **0**. Results again matched **1,003,479 bytes flash** and **51,196 bytes globals**. All four source hashes above remain identical. No upload or hardware actuation occurred.

The original GPIO2 Blink source was copied byte for byte to a private correctly named Arduino sketch folder and compiled for the same target: exit **0**, **257,676 bytes flash**, **22,100 bytes globals**. This proves compilation of the retained source, not a newly performed physical Blink test.

The recovered pre-direction-correction controller compile process terminated with exit `0x40010004`; the manual 1.0.1 recheck exited `0xC000026B`. Neither produced usable diagnostics or a successful fresh build. No code defect or successful compilation is inferred from those process failures. The saved 18 October 1.0.1 report matches SHA-256 `9655C78987F88AB2D456E0E78B7D474D15483288F0D4EBAAC8C25CF1389AE44E` and records a successful build with 981,779 bytes flash / 50,316 bytes globals. No prior successful build is established for the pre-correction source. Single-file personal V5 exports were inspected, not recompiled or uploaded in this audit.

Private build logs and byte-identical compile-input copies remain outside Git. Historical archives and [code-to-video associations](SOURCE_AND_EVIDENCE.md#code-to-video-evidence-map) are labeled separately from current software and physical evidence.

The owner reports working manual-build functions. V5 software tests and preview renders are verified as above. The following remain **unverified physically**: actual motor voltage/current ratings and paired load, regulator jumper/loaded rails, IR signal voltage, servo clearance and calibration, real echo timing, manual and autonomous motor behavior, stopping distances, thermal/current behavior, reset/failure timing and course performance. Fill TEST_PLAN.md with actual observations before claiming a hardware-validated release. No real demonstration video or fabricated measurements are included.
