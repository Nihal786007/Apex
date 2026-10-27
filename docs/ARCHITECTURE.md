# V5 architecture and important changes

## Source boundaries

`Config.h` is the hardware/power/calibration layer. `ControlCore.h` is a portable C/C++ state machine with fixed-size fields, no Arduino dependencies and no GPIO writes. `APEX_Ultimate_V5.ino` retains the WiFi/WebServer/U8g2 integration, owns hardware adapters and runs the control task. `Dashboard.h` extends the prior navy/teal manual dashboard. The preceding 1.0.1 source is kept intact as a reference, not compiled alongside V5.

All physical pins, AP credentials and motor reversal flags are retained. The explicit requested differences are IR34/35 support, mode state machines, validated servo pulse starting calibration, queue/epoch arbitration, asynchronous ranging, richer OLED status and a default hardware output gate.

## Single owner and hand-off

- The control task runs on classic ESP32 core 1 at a nominal 5 ms cadence. It alone changes controller decisions and writes motor/servo outputs after setup.
- The echo ISR records rise/fall pulse timing into a separate critical-section-protected capture. It does not touch motor state, Strings, HTTP, OLED or FSMs.
- IR signals are sampled each control loop; candidate levels must remain unchanged for 40 ms before updating the debounced values.
- `updateUltrasonic` launches a ping about every 70 ms and accepts an echo or 25 ms timeout without `pulseIn`. The sole deliberate busy wait is the 10 µs trigger pulse.
- HTTP handlers parse and authenticate, send a fixed-size command, and wait no more than 200 ms for its ticketed acknowledgement. A queue/ack failure raises a control-task stop notification.
- STOP/PAUSE/DISARM notify the control task urgently as well as entering the ordered queue. The command is revalidated by the control owner.
- Published snapshots are copied under a short mutex. Web JSON and OLED rendering use a copy. OLED refresh/I2C timeouts cannot hold the motor-state mutex.

## Request ordering

Every motion-capable action carries a random session token, increasing sequence and captured epoch. Claiming control stops/disarms and replaces the session. Stop, fault, release, mode change and other deliberate safety transitions advance the epoch. A delayed Arm/Start/drive from an earlier epoch cannot start the robot, even if its sequence is numerically larger.

Authenticated STOP/PAUSE/DISARM ignore an old epoch and revoke motion; they do not start anything. Stale sequences are rejected. Manual release stops only manual ownership and increments the epoch to prevent a late hold renewal. Controller LEASE is not a manual hold and does not keep a released manual command alive.

The dashboard has one ordinary request in flight, coalesces queued power adjustments and gives STOP priority. STOP clears pending motion actions, cancels a pending fetch and schedules a higher-sequence stop. Action epochs are captured when the user queues an action, not silently rewritten on dispatch. A rejected action requires a deliberate retry. Hidden pages, page close, connection failures and blur request stop; firmware timeouts remain the backup.

## Manual mode

Arm alone does not move. A nonzero power and held direction are required. Forward requires centered/settled sensor and a fresh valid clear reading. Backward/pivots retain the baseline behavior and have no spatial obstacle protection. Release stops motor drive. The 450 ms timeout applies only to manual holds, never to autonomous state progression.

## Line-following states

Before START, WHITE and BLACK captures must differ for each sensor. Capture labels represent the physical calibration surface; level polarity is inferred separately for left/right. Default prior configuration is WHITE=0, BLACK=1, but calibration must confirm it.

| Input interpretation | Decision |
|---|---|
| Left BLACK only | Reduce left command; correct left |
| Right BLACK only | Reduce right command; correct right |
| Both BLACK in default wide-stripe geometry | Drive forward as the configured centered pattern |
| Neither BLACK in default geometry | Stop briefly, search toward last correction then the opposite direction, bounded by recovery time |
| Lost pattern with no prior side evidence | Stop rather than guess |
| Recovered edge/center pattern | Return from LINE_RECOVER to LINE_TRACK |
| Recovery expires | Stop/disarm and require deliberate restart |

For a narrow line between widely spaced sensors, the UI can select both-WHITE as centered. Two digital sensors then cannot distinguish being perfectly centered from being entirely off-track on white. V5 therefore requires new edge evidence within 1 s; otherwise it stops. Both-BLACK in this geometry is ambiguous and stops. This conservative option may interrupt long straight tracks; use a suitable track/sensor geometry or better sensing rather than claiming universal line following.

Transient debounce changes command zero while settling. Persistent unstable IR input for more than 250 ms stops/disarms. A disconnected sensor stuck at a plausible constant logic level cannot always be diagnosed electrically in software. A fresh forward ultrasound is required during line operation and bounded pivots, but does not cover the sides or rear.

Correction strength, centered geometry and bounded recovery duration are tunable while paused. This is digital proportional steering by pattern, not analog PID or reliable intersection classification.

## Obstacle-avoidance states

`OA_CRUISE → SCAN_LEFT → SCAN_CENTER → SCAN_RIGHT → CHOOSE_PATH → PRE_TURN → TURN_LEFT/RIGHT → RECENTER → OA_CRUISE`.

At an obstacle, motion stops before any scan. Each commanded servo position waits the configured 500 ms, then collects three distinct fresh valid pings tagged with the corresponding pan angle and trigger time. Median distance is accepted only if the sample spread is no greater than max(15 cm, 35% of the median). Missing usable samples cause a scan timeout and stop; invalid echoes are never treated as infinity.

Center is preferred if clear; otherwise the clearer valid side meeting the side-clear threshold is selected. The servo recenters and supplies a fresh observation before the timed pivot. The pivot is capped at 130/255 PWM and the configured 100–600 ms duration. Recenter then requires a new sample after turning before forward travel resumes. These are commanded durations, not measured turning angles.

Three failed recovery cycles or three alternating-direction decisions cause a latched stop. No clear direction also stops. Automatic retreat is deliberately absent: with no rear sensor, the software cannot verify it is safe to reverse. An operator can inspect the area and use supervised manual recovery.

A single front beam cannot verify the complete pivot swept volume, detect every obstacle or solve arbitrary mazes. Test only a controlled course with clearance around the entire chassis. The reported high manual starting threshold means these low-power timed turns are especially unvalidated on the current hardware.

## Independent limits and outputs

Both modes require a live 1.8 s controller lease, a maximum 30 s autonomous run, 8 s continuous nonzero motor-command interval, and a control loop that has not missed a 100 ms deadline. Bounds stop/disarm; they do not automatically resume. These are conservative software limits, not temperature/current sensing. MOTOR_POWER_VERIFIED=false additionally suppresses all physical ENA/ENB outputs while exposing requested versus applied commands.

The deadline guard detects a missed interval when the task resumes. It is not an independent motor-disable circuit if the task never resumes. An external/hardware watchdog or driver-enable gate is a future power-system improvement; an accessible physical cutoff remains necessary.

Motor reversal or sign changes include 40 ms zero-output neutral time. Servo frequency and motor frequency use distinct initial LEDC frequency/resolution settings. Do not change a shared motor timer's frequency on only one channel.

## OLED and API

The OLED always retains a live fresh-distance row, even while cycling the state/custom-message row. Mode, phase, IR values, correction and commanded angle remain visible. Long custom messages cycle their first 21 and remaining 19 characters; no position feedback is claimed.

`POST /claim` returns token and epoch. `POST /cmd` accepts token, seq, epoch, kind and bounded parameters. `GET /status?token=...` returns a snapshot and does not renew the lease. Commands include arm/start/pause/stop/disarm/mode/drive/release/power/pan/text/white/black/tune/lease. No remote endpoint can enable the physical power gate. HTTP runs over a local AP; demo credentials are not strong authorization for use in an untrusted crowd.
