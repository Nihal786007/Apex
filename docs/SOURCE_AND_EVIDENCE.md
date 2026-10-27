# Source recovery, code guide and physical evidence

**Audit: 27 October 2026.** This guide links the saved programs to the engineering record. A similar dashboard does not identify the binary uploaded in a video. No historical test sketch has been invented or reconstructed.

Start with the [current four-file V5](../firmware/APEX_Ultimate_V5), [earlier manual controller](../reference/APEX_1_0_1_baseline.ino.txt), or [original Blink](../progress/Session_01_ESP32_First_Blink/Blink.ino). Electrical conditions remain in [POWER_AND_WIRING.md](POWER_AND_WIRING.md).

## Recovered versions and minimal archive

Six saved firmware forms were found across **16 local source-file copies**. SHA-256 comparisons identify duplicates. Only one newly recovered historical source is added; all existing originals and local exports remain untouched.

| Version / stage | Original saved location, relative to the workspace | Selected source and evidence status |
|---|---|---|
| First Blink | Existing repository: `progress/Session_01_ESP32_First_Blink/Blink.ino` | [Original](../progress/Session_01_ESP32_First_Blink/Blink.ino), GPIO2 with one-second HIGH/LOW intervals. Notes record **16 July 2026** and success. Fresh compile-only PASS on 27 October; no new upload. |
| Before direction correction | `APEX_Portfolio/reference/APEX_before_direction_fix.ino.txt` | [Unchanged recovered archive](../reference/APEX_before_direction_fix.ino.txt). Integrated L298N/manual Wi-Fi/servo/range/OLED program; motor reversal flags false/false. Exact development date unavailable. A fresh compile attempt terminated without diagnostics; no successful build is claimed for this source. |
| Manual 1.0.1 | `APEX_Tablet/APEX_Tablet.ino`; identical `APEX_Portfolio/firmware/APEX_Tablet/APEX_Tablet.ino` | [Already preserved baseline](../reference/APEX_1_0_1_baseline.ino.txt). Reversal flags true/true; Forward rejection/fault-retention correction. Saved **18 October 2026** report matches its hash and records compile PASS. Latest recheck could not start successfully; it does not supersede that saved evidence. |
| V5 four-file candidate | `APEX-Ultimate-Robotics/firmware/APEX_Ultimate_V5/` | [Current package](../firmware/APEX_Ultimate_V5): `.ino` plus `Config.h`, `ControlCore.h`, `Dashboard.h`. Recorded 25 October build and fresh **27 October compile PASS**, unchanged source hashes, BENCH retained. Physical autonomous acceptance pending. |
| V5 single-file BENCH export | `APEX_V5_Arduino_Single/APEX_V5_Arduino_Single.ino` | Kept locally. Alternate copy/paste packaging of the later V5 candidate, not an independent early experiment. Not copied into the public candidate. No fresh compile or physical test claimed. |
| V5 motor-enabled personal export | `APEX_V5_Motors_Enabled/APEX_V5_Motors_Enabled.ino` | Kept locally. Its power-verification flag is true; that is a configuration difference, not proof of suitable electrical ratings. Not the distributed operational recommendation; no fresh compile or physical acceptance claimed. |

Original locations above are provenance names, not public download links. Compiler-generated copies are not extra development versions. File modification times and video filenames do not establish source release or upload dates. The linked conversation's available cached messages contain content-reference markers rather than the missing early assistant code; they cannot recover those sketches.

Historical `.ino.txt` files are intentionally text archives, preventing Arduino from combining unrelated programs. To compile one for inspection, copy its **unchanged bytes** into a private folder with a matching `.ino` basename. Keep all four current V5 files together. Compile-only checks never authorize upload or motor power.

| Preserved historical source | SHA-256 |
|---|---|
| Before direction correction | `24246D59124581D6CB1C454EB27F44CCE9ECE84396BB310B66A8204F93602229` |
| Manual 1.0.1 | `9655C78987F88AB2D456E0E78B7D474D15483288F0D4EBAAC8C25CF1389AE44E` |

Current V5 hashes and build details are in [BUILD_REPORT.md](BUILD_REPORT.md).

## Missing originals

No separate original source was found for initial driver-only tests, left-only/right-only motor tests, standalone four-wheel Forward/Reverse/turn/PWM tests, TB6612FNG experiments, or standalone servo, HC-SR04, OLED and IR black/white sketches. Later integrated functions do **not** recover these original experiments. The saved changelog describes a 1.0.0 stage, but no distinct saved intermediate 1.0.0 source was recovered; the retained corrected manual file is 1.0.1.

Early repository notes name TB6612FNG/two motors. All recovered integrated controllers use the L298N-style ENA/ENB and IN1–IN4 interface. They must not be relabeled as TB6612 code. The driver photograph does not date the hardware transition. No reconstructed example is presented as historical proof. V5 line/obstacle logic exists, but its physical acceptance tests remain pending.

## Source navigation

| Inspect | Actual file and symbols |
|---|---|
| Earlier motor commands, PWM and STOP | [Manual 1.0.1](../reference/APEX_1_0_1_baseline.ino.txt): `motorTask`, `driveSide`, `motorOff`, `forwardBlock` |
| Earlier servo, range, display and browser | Same archive: `setPan`, `sampleRange`, `loop`, `PAGE`, `setupRoutes` |
| Current pins, polarity, pulses and BENCH | [Config.h](../firmware/APEX_Ultimate_V5/Config.h) |
| Current motor adapter | [APEX_Ultimate_V5.ino](../firmware/APEX_Ultimate_V5/APEX_Ultimate_V5.ino): `oneMotor`, `applyMotors`, `disableMotorPins` |
| Servo and physical sensor acquisition | Same `.ino`: `servoPulse`, `echoISR`, `updateUltrasonic`, `updateIR` |
| OLED and HTTP | Same `.ino`: `renderOLED`, `setupRoutes`, `sendStatus`, `dispatch` |
| Mode ownership and stopping | [ControlCore.h](../firmware/APEX_Ultimate_V5/ControlCore.h): `apex_command`, `apex_tick`, `apex_stop` |
| IR calibration and line control | Same core: `apex_calibrated`, `apex_line` |
| Obstacle decisions | Same core: `apex_scan_begin`, `apex_median3`, `apex_avoid` |
| Browser, held controls and E-STOP | [Dashboard.h](../firmware/APEX_Ultimate_V5/Dashboard.h): command queue, `releaseHeld`, pointer and STOP handlers |
| Automated verification | [Core assertions](../tests/control_core_test.c), [dashboard tests](../tests/dashboard_test.cjs), [pin contract](../tests/hardware_contract_test.cjs) |

### Motor control

Both archived controllers and V5 retain **ENA25 / IN1 26 / IN2 27** and **IN3 14 / IN4 12 / ENB13**. The four-wheel target pairs two motors in parallel per channel; code drives two channels, not four independently sensed wheels. Pairing and combined currents are wiring conditions.

| Logical command | Left | Right |
|---|---|---|
| Forward | +power | +power |
| Reverse | −power | −power |
| Left pivot | −power | +power |
| Right pivot | +power | −power |
| Stop | zero | zero |

The V5 adapter uses `electrical = reverse ? -logical : logical` for direction pins, and `ledcWrite(pwmPin, abs(logical))` for PWM. Reversal is false/false in the pre-correction archive, true/true in 1.0.1 and V5. Historical comments remain unchanged even where wording is inconsistent; the constants identify actual settings. Each installation still needs physical direction checks.

PWM is **20 kHz / 8 bit**, command **0–255**, displayed as a percentage. It is not measured speed. Output-off writes zero enable duty and all direction pins LOW; no measured braking distance is implied. A 40 ms neutral interval precedes direction changes. V5 disables applied PWM when `MOTOR_POWER_VERIFIED=false`. **Historical manual archives lack that BENCH gate and can energize outputs after Arm.** They are archived educational evidence, not current upload recommendations.

### Servo

Signal **GPIO18**, LEDC **50 Hz / 16 bit**. Old pan commands −100/0/+100 produce nominal **2100/1500/900 µs** with the saved reversal flag. V5 mapping references are **1000/1500/2000 µs**, with center 90 and logical travel **30–150°**. The current integer mapping produces approximately **1833/1500/1167 µs** at logical 30/90/150; the bounded commands do not reach the full nominal pulse endpoints. These are commands, not measured shaft positions.

The references reflect the reported calibration, not an independently measured angle test. Exact fitted-servo identity remains uncertain. Obstacle scanning stops drive, commands left/center/right, waits for settling and takes fresh samples. A suitable regulated supply and common ground remain necessary. BENCH disables motors, **not servo motion**.

### Ultrasonic sensor

Connections remain **TRIG5 / ECHO19**, with verified 3.3 V-safe ECHO conditioning required. Manual code uses bounded `pulseIn(...,25000)`. V5 uses interrupt edge capture, trigger spacing of at least 70 ms, a 25 ms timeout, and `width * .0343f / 2.0f` for centimeters. It accepts 2–400 cm samples tagged by time, sample ID and commanded pan.

Forward readiness needs a centered/settled sensor and valid data at most **350 ms** old; configured V5 settling is **500 ms**. Default stop/release hysteresis is **25/30 cm**. Missing/stale readings lock forward motion. These source rules do not independently establish sensor accuracy or physical stopping distance. Reverse/pivots do not have complete obstacle coverage.

### OLED

Both integrated versions use **U8g2**, `U8G2_SH1106_128X64_NONAME_F_HW_I2C`, **SDA21 / SCL22**, and setup probes at 0x3C/0x3D. V5 renders at most every 250 ms, retaining distance/NO ECHO, mode, state, IR/correction and commanded angle, while cycling status/custom text. Browser input accepts up to **40 printable ASCII characters**. A displayed distance is not a calibrated accuracy result.

### IR sensors and line following

Only recovered V5 code implements **GPIO34 / GPIO35**. These input-only pins have no internal pulls. Pair stability is debounced for **40 ms**. WHITE and BLACK captures while stopped derive each channel's polarity; both must distinguish the surfaces. Calibration resets on reboot.

`apex_line` slows one side when only one sensor sees black. Both-BLACK is the default centered geometry. Optional both-WHITE centering requires recent edge evidence; an ambiguous pattern cannot prove a line. Lost-line recovery pauses, then uses bounded opposite pivots and the last correction. Failed recovery, persistent unstable inputs or an unsafe forward range stops it. Two digital sensors cannot classify every junction or detect every floating/stuck signal. No standalone original IR sketch or completed physical line test was recovered.

### Wi-Fi, browser and controller safety

AP **APEX-CONTROL**, documented demo password **Apex2026Robot**, normal control address **http://192.168.4.1**. These are local demo credentials, not public-network security. Manual `PAGE` and V5 `Dashboard.h` both expose `GET /`, `POST /claim`, `POST /cmd`, `GET /status`, but their message formats differ; do not mix old UI and V5.

V5 commands include `arm`, `start`, `stop`, `pause`, `disarm`, `release`, `lease`, `mode`, `power`, `pan`, `drive`, `text`, `white`, `black`, `tune`; drive values are `F/B/L/R`. Validated parameters, session tokens, sequence numbers and epochs reject stale actions. These ordering guards are not a comprehensive security system.

One control task owns outputs and decisions. Fixed queues carry HTTP commands; web/OLED read copied status. Stop/pause/disarm notify the owner task urgently. Pointer release/cancel/lost-capture sends release; connection faults attempt stopping. Firmware independently enforces **450 ms hold**, **1.8 s lease**, **30 s autonomy run**, **8 s continuous nonzero-command limits**, and a loop-deadline check. Mode changes stop/disarm; autonomy requires Arm then START. Network E-STOP is software, not a certified physical emergency circuit.

### Obstacle avoidance

`OA_CRUISE` needs fresh centered range. Obstruction starts stopped `SCAN_LEFT → SCAN_CENTER → SCAN_RIGHT`: three distinct settled samples per direction, a median and consistency check. `CHOOSE_PATH` prefers a clear front, otherwise a sampled clear side; trapped conditions stop. `PRE_TURN` recenters and checks a fresh echo, then a bounded `TURN_LEFT/RIGHT` leads to stopped `RECENTER` and new data. Recovery/alternating-turn limits bound oscillation. No blind automatic reverse is used without rear sensing. Timed pivots and sparse range samples do not provide mapping or guaranteed maze solving.

## Code-to-video evidence map

**Source similarity is not deployment proof.** Physical clips lack an upload log/hash linking them to a retained integrated sketch: **“Exact firmware revision not verified.”** Film source keys are editorial aliases, not firmware versions. Filename times are not upload evidence.

| Demonstration | Inspectable source | Hardware / test status | Video evidence and boundary |
|---|---|---|---|
| First ESP32 Blink | Original Blink | GPIO2; notes report successful session | Existing [first-program clip](../assets/videos/first%20program%20of%20blinking%20esp32%20and%20the%20output.mp4), 32–37 s in film. Strong stage association; exact firmware revision not verified. |
| Initial driver, left-only/right-only motor tests | No separate original; later `driveSide`/`oneMotor` for inspection | Independent motor tests not established | `20261027-0645-47.5534816.mp4` shows power/motor handling, not a verified standalone test program. Exact firmware revision not verified. |
| Four-motor Forward/Reverse, left/right turns | Manual `motorTask`; later V5 `apex_tick` + `applyMotors` | Manual movement/heading changes supplied; each named direction and all four responses not independently proven | `WhatsApp Video 2026-10-19 at 3.15.00 PM.mp4`, 19–26 s floor and 39–49 s tablet excerpts; `...3.14.11 PM.mp4`, 10–16 s terrace. Exact firmware revision not verified. |
| PWM and browser manual control | Manual `PAGE`/routes; later V5 dashboard | UI/robot documented; no measured speed/duty/current relationship | Same 3.15.00 PM recording; `20261027-0649-21.2348135.mp4` recap. Exact firmware revision not verified. |
| Servo and distance interface | Manual `setPan`/`sampleRange`; later V5 adapters | Physical pan/range interface supplied; angle/range accuracy not independently measured | `WhatsApp Video 2026-10-18 at 7.05.28 PM.mp4`, 2–10 s. Exact firmware revision not verified. |
| OLED message | Manual `PAGE`/`loop`; later V5 `renderOLED` | Tablet entry and physical display supplied | `WhatsApp Video 2026-10-18 at 7.06.53 PM.mp4`, 1–8 s; also `...7.04.25 PM.mp4`. Exact firmware revision not verified. |
| Release-to-stop / E-STOP | Manual release/`motorOff`; V5 release/`apex_stop` | Software paths and automated checks; physical timing not measured | Bench interactions do not provide a controlled timed stop trial tied to a hash. Exact firmware revision not verified. |
| IR black/white detection | V5 `updateIR`/`apex_calibrated` | Software implemented/tested; actual level/polarity/calibration pending | Photographs show differing IR component styles, not a completed black/white acceptance test. No verified test clip. |
| Line following | V5 `apex_line` | Implemented/software tested; physical acceptance pending | [Line UI preview](../assets/dashboard-line-preview.png) is simulated telemetry, not a physical run. |
| Obstacle avoidance | V5 `apex_avoid` | Implemented/software tested; physical acceptance pending | [Obstacle UI preview](../assets/dashboard-obstacle-preview.png) is simulated telemetry, not a course. |
| Final three-mode V5 | Current source and tests | Compile/test evidence identifies hashes, not deployment | Film 70–75 s uses the [labeled manual UI](../assets/dashboard-manual-preview.png): simulated/no robot connected. |
| Failures and debugging | Pre-correction and 1.0.1 archives | Reported polarity, startup and Forward-block problems; physical cause not conclusively solved | `20261027-0642-14.1339708.mp4` has an edited failure title; `...0643-34.3573612.mp4` is a debugging episode. Neither proves a root cause or exact revision. |

Raw supplied filenames are provenance, not broken public download links. Full film source names/ranges are in [DEMO_AND_RELEASE.md](DEMO_AND_RELEASE.md); selected media hashes are in the [development log](DEVELOPMENT_LOG.md#selected-media-provenance). Reused episode shots are not independent experiments. No raw-video collection is added to Git.

## Configuration differences and unresolved physical checks

| Setting | Before correction | Manual 1.0.1 | Current V5 |
|---|---|---|---|
| Motor GPIO | 25/26/27; 14/12/13 | Same | Same |
| Motor reversal | false / false | true / true | true / true |
| Servo/range/OLED GPIO | 18; 5/19; 21/22 | Same | Same |
| IR | Not implemented | Not implemented | 34 / 35 |
| Servo coordinates | −100…+100 pan | Same | 30…150 logical degrees |
| BENCH motor gate | Absent; Arm can enable outputs | Absent; Arm can enable outputs | `MOTOR_POWER_VERIFIED=false` |
| Ownership/order | Earlier shared state, no V5 epoch barrier | Adds Forward fault retention | Single owner, queues, session/sequence/epoch, three modes |
| Electrical sensing | No voltage/current/temperature feedback | Same | Same |

Motor ratings and paired startup/stall current, reported 11.6–12 V input, overheating, motor rail and L298N 5V-EN/logic arrangement remain unresolved. A switch or low PWM is not voltage/current protection. Common ground, adequate servo supply, ECHO divider, 3.3 V-safe IR/I²C, GPIO12/5 boot straps, GPIO34/35 restrictions, fuse/cutoff and suitable battery protection still need physical verification. Follow the power guide and [test plan](TEST_PLAN.md). No powered test/upload was performed in this audit.

## Contribution and next evidence

The owner supplied the physical build, assembly/wiring work, filmed experiments and debugging observations. Software, reviews, tests, documentation and editorial preparation were developed with **AI assistance**. The portfolio does not claim every source line was independently written by the owner. Admissions claims should describe the owner's actual decisions, construction, testing, understanding and learning.

Future tests should record the source hash/version, upload log, actual wiring/supply measurements, continuous normal-speed commands/response and pass/failure observations. That would connect code to hardware more strongly than duplicate programs or a polished montage. Physical line following and autonomous-course validation remain future work.
