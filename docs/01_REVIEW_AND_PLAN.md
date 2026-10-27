# Baseline review and incremental implementation plan

Reviewed 25 October 2026 before implementation. The new attachment supplies requirements, not a new firmware source. The source of truth available here is the prior local **1.0.1** sketch, preserved unchanged as `reference/APEX_1_0_1_baseline.ino.txt`. If the physical robot runs a newer source, compare it before migration.

## Working architecture retained

Classic ESP32, Arduino-ESP32 3.3.12, U8g2 2.36.19, SH1106 full-buffer display, WebServer, WiFi AP, LEDC motors at 20 kHz/8 bits and servo at 50 Hz/16 bits. Motor polarity flags are both true based on the reported mounting; their physical correctness remains a staged check. SSID/password and every existing GPIO stay unchanged. Existing manual controls, session claim, explicit arming, STOP, release-to-stop, timeout, servo pan, custom OLED text and distance display are retained.

The earlier local source maps servo pulses beyond the newly supplied successful calibration. V5 intentionally resets the configurable pulse calibration to **1000/1500/2000 µs**, with a conservative travel limit. This is an explicit requested calibration change, not a claim of measured angular feedback.

## Findings

| Finding | Effect | Targeted change |
|---|---|---|
| Web handlers mutate motor/servo state and call servo PWM while the motor task owns motor outputs | Ownership becomes hard to reason about with autonomous modes | Queue commands to one control task; handlers only copy snapshots and wait for bounded acknowledgements |
| Sequence numbers lack a stop-generation barrier | A delayed higher-numbered Arm/Start can be accepted after a stop | Require current session, increasing sequence and epoch; stop/fault/mode changes increment epoch |
| `pulseIn` blocks up to 25 ms per sample in the UI loop | Slower Wi-Fi/OLED service | Asynchronous echo edge capture with timeout; only the 10 µs TRIG pulse is busy-waited |
| Frequent String construction in motor task | Avoidable heap churn | Fixed-size status/message fields and pure controller state |
| Manual timeout assumes one driving model | Could stop autonomous motion incorrectly | Separate manual hold, autonomous lease, run-duration and motor-on guards |
| Forward rejection previously hid its own fault | Confusing button behavior | Keep latched reasons and visible readiness; do not send STOP to overwrite a drive rejection |
| Single front sensor cannot see behind or verify pivot swept volume | Unsafe automatic retreat or maze claims | No automatic reversing; trapped robot stops; bounded timed turns only in a prepared course |
| Unknown paired TT motor stall currents and 11.6 V supply | Prior overheating cannot be declared resolved | Default BENCH mode physically suppresses ENA/ENB; explicit documented power verification gate |
| No voltage/current/temperature or encoder inputs | Cannot verify electrical safety or actual motion in software | Never claim such feedback; expose commanded/applied PWM separately |
| GPIO34/35 have no internal pulls and cannot accept 5 V | Floating/overvoltage IR inputs are possible | INPUT only; require compatible outputs/external conditioning and calibration |

## Smallest reliable upgrade sequence

Exact baseline-to-V5 function boundaries:

| Baseline function/field | V5 boundary and reason |
|---|---|
| motorTask / requested / actual | controlTask + apex_tick + applyMotors; one owner and mode arbitration |
| driveSide / motorOff | oneMotor / disableMotorPins; preserved reversal flags, output gate and neutral interval |
| trip | apex_stop; also invalidates old motion generations |
| sampleRange / pulseIn | updateUltrasonic + echoISR; nonblocking pulse acquisition and angle/time tagging |
| setPan | queued SET_PAN + apex_servo + servoPulse; only the owner writes PWM |
| setupRoutes | Existing WebServer approach retained; dispatch and ticketed acknowledgements replace handler state mutation |
| PAGE | Dashboard.h; existing visual language/control functions extended with modes and calibration |
| loop's OLED block | renderOLED; copied snapshot and always-present fresh-distance row |
| guard/shared globals | Snapshot mutex plus private ApexCore; no HTTP-held control-state lock |
| no prior IR/autonomy functions | updateIR, apex_line and apex_avoid are additive mode/sensor logic |

1. Preserve the baseline and document hardware assumptions.
2. Extract motor-command arbitration and finite-state logic into a portable core, directly exercised by native tests.
3. Keep Arduino hardware setup and OLED renderer; replace shared handler mutation with a command queue and control-task snapshots.
4. Add debounced GPIO34/35 IR reads and calibrated black-level configuration.
5. Add bounded LINE_FOLLOW states and configurable two-sensor center geometry.
6. Add stop/settle/sample/choose/turn/recenter OBSTACLE_AVOID states with three-sample consistency checks.
7. Extend the existing navy/teal dashboard with mode tabs, START/PAUSE/DISARM, readiness and calibration controls.
8. Compile with the installed board/library versions, run core/browser tests, render the actual controller for screenshots, and package evidence honestly.
9. Require staged electrical and physical acceptance before motor-enabled release.

## Ownership and safety contract

Only the control task changes controller state or drives servo/motor outputs after initialization. Echo ISR records pulse edges only. Web handlers enqueue fixed-size messages; main-loop OLED reads a copied snapshot. Authenticated STOP/PAUSE/DISARM bypass an outdated epoch, always revoke motion and increment epoch. Motion-capable commands must match the new epoch. A new claim stops/disarms and changes session. Every mode switch stops/disarms. Autonomous Start is separate from Arm and manual holds cannot control an autonomous mode.

The dashboard serializes ordinary requests, clears pending actions on STOP, and captures epoch when an action is queued. Controller heartbeat does not count as a manual hold. Autonomous behavior requires its own live-controller lease and bounded run/motor-on limits. Network loss is a controlled stop, never autonomous continuation by accident.

## Explicit limits

No physically verified motor voltage/current/thermal data has been supplied. The user's statement that it works at 12 V is recorded as a report, not a motor rating or paired-current test. Physical actuation stays disabled by default. The prototype has one ultrasonic sensor, two digital IR sensors, no rear sensor and no encoders. Modes are cautious rule-based control, not arbitrary maze solving, localization, mapping or intersection classification.
