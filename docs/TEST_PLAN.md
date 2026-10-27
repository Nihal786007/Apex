# Thirteen-stage acceptance plan

Record real observations in the result column; blank/PENDING is not a pass. Date, board/library version, firmware hash, power arrangement, surface, sensor height and tester should accompany physical results. Stop a stage on failure and diagnose before proceeding.

**Physical gate:** stages 1–6 run with motor power disconnected and MOTOR_POWER_VERIFIED=false. Do not proceed to powered stages until stage 7 is signed off. All four wheels must be securely raised initially; keep hands/wires clear and cutoff accessible.

| Stage | Procedure | Pass / expected behavior | Failure / troubleshooting | Physical result |
|---|---|---|---|---|
| 1. Compile/dependencies | Keep all four firmware files together; Verify classic ESP32, Arduino-ESP32 3.x, U8g2 | No errors; installed configuration matches build report | Missing header: restore folder; 2.x LEDC errors: use required board core; wrong target: select classic ESP32 | Local compile PASS; owner upload pending |
| 2. Wi-Fi/controller | Upload with motor power off; Serial 115200; join APEX-CONTROL and open HTTP 192.168.4.1 | BENCH visible; connected, disarmed, power zero; one active session | Check USB data cable, port, BOOT, mobile-data switching/VPN. FATAL startup is a failure | PENDING for V5 |
| 3. OLED/live distance | Broad flat target at known distances; send 40 ASCII characters; watch at least one full 6 s text/state cycle | Distance row remains present and updates while message/state cycles; missing OLED does not freeze controller | Confirm SH1106 128×64, I2C address/pull-up levels, divider and target geometry; old baseline report is not a V5 physical pass | PENDING |
| 4. Servo | Unload linkage, Center, small Left/Right, then configured endpoints | Center physically forward, correct mounting direction, no binding; UI displays commanded degrees and pulse | Refit horn at Center with power off; toggle only servo reversal if needed; reduce travel; investigate regulated 5 V under load for jitter/weak motion | PENDING |
| 5. IR calibration | Verify voltage first; hold both over broad white, capture, then broad black | Both stable levels differ across surfaces; expected prior WHITE0/BLACK1 verified or inferred polarity displayed; line START rejected before calibration | Check height, comparator setting, lighting, left/right wiring and output voltage; constant levels fail calibration | PENDING |
| 6. Motor logic, no motor power | In BENCH, Arm and hold each direction; perform white/black captures; exercise autonomous logic by moving test targets/surfaces | Requested signs match table below, applied PWM stays 0/0, ENA/ENB physically remain disabled; release and STOP clear requests | If applied nonzero, wrong build/gate; if physical wheels move, cut power and inspect enables/jumpers; logic trace is not physical-motion proof | PENDING |
| 7. Electrical safety | Fill POWER_AND_WIRING sign-off: motor rating, two-motor current per channel, motor/logic rails under load, regulator jumper, cooling, fuse, ground, IR levels | Every required value verified and appropriate; no raw 3S assumption for low-voltage TT motors | Any unknown/unsuitable rating is FAIL; do not set power gate true. Prior overheating requires investigation | NOT VERIFIED |
| 8. Raised-wheel manual | After stage 7, rebuild gate true, upload with power off; raise wheels, center sensor, clear target, choose suitable low nonzero PWM, Arm, brief commands | Forward all wheels propel forward; Backward all reverse; Left/Right signs correct; release/STOP disable drive | Correct only affected polarity/physical pair assignment, not both wiring and flag; release a humming/stalled motor immediately; check driver loss/sag/drag | PENDING |
| 9. Low-speed ground | Short supervised pulses on clear level floor after raised-wheel pass | Predictable direction, controllability and measured stopping clearance; no resets/rapid heating | Cut power for stalls, heat or resets; record loaded supply and mechanical condition; do not compensate with unsafe voltage | PENDING |
| 10. Line following | Calibrate at operating height; use broad black tape compatible with both-BLACK center; select mode, power, Arm, START | Center follows; left/right edges cause proper correction; lost line searches only within bounded time and stops if unrecovered | Wrong correction: verify sensor side assignment/polarity, track width and motor orientation; both-WHITE geometry has intentional 1 s evidence limit | PENDING |
| 11. Obstacle course | Use broad stationary boxes and generous chassis clearance; OA, power, Arm, START | Forward needs fresh clear center; obstacle stops motion; LEFT/CENTER/RIGHT scans settle; validated side selected, bounded turn, new center sample; trapped/no echo stops | No echo is not infinity; adjust geometry/lighting is irrelevant to ultrasound; inspect scan target/settling, power, timed turn calibration; no arbitrary-maze success claim | PENDING |
| 12. Modes and E-STOP | Change each mode during a short active request; test PAUSE, STOP and DISARM in all modes | Immediate logical revocation on acceptance/control-task notification, zero outputs by control cadence; mode changes disarm; Arm then START required for autonomy; no automatic resume | Any continuing/restarting command is FAIL; retain logs, do not test on floor until corrected; neutral/coast is not active braking | PENDING |
| 13. Failure/reset/order | Raised-wheel or bench: release, disable Wi-Fi, hide page, cover/unplug sensor only with safe power handling, change sessions, reset and inject stale requests in controlled tests | Manual command expires at 450 ms plus scheduling; lease at 1.8 s; stale generations cannot restart; sensor failure stops autonomy; reset starts disarmed at zero; no page-close restart | Use native/browser tests for unsafe fault sequences; physical isolation with motor cutoff first; stuck IR levels cannot always identify disconnect | Software tests PASS; physical PENDING |

## Requested motor signs

Positive means the robot's logical forward direction; polarity flags map this to electrical levels. Channel A is left pair, B right pair. Physical PWM is zero in BENCH even if these requested values are nonzero.

| Action | Left request | Right request |
|---|---:|---:|
| Forward | +P | +P |
| Backward | -P | -P |
| Left pivot | -P | +P |
| Right pivot | +P | -P |
| STOP/release | 0 | 0 |

## Specific failure acceptance checks

- Trigger a forward obstacle at ≤25 cm, then clear beyond 30 cm. It must remain stopped/disarmed until deliberate Arm plus a new hold/START.
- Scan while stationary. Each position must wait 500 ms and collect three separate pings. Disagreeing samples or unavailable valid samples must not produce a guessed forward motion.
- With wheels raised, validate timed pivot behavior separately from path choice. A displayed TURNING state is a command, not confirmation of rotation.
- In line mode, transient debouncing holds zero output; persistent unstable IR >250 ms disarms. Recovery lasts at most its configured 300–1500 ms. A no-history loss stops instead of searching blindly.
- Verify manual heartbeat does not accidentally drive an autonomous mode. Autonomous lease renewal alone must not keep a released manual hold running.
- Inspect 8 s continuous-command and 30 s autonomous-run guards in BENCH or native tests; do not stress unknown motors to test software timeouts.
- Check normal tablet layout with tuning collapsed; then enlarge text/open keyboard. Scrolling is acceptable for accessibility and settings; E-STOP must stay discoverable.

## Result record template

```text
Date / tester:
Board / core / U8g2:
Firmware hash / Config gate:
Motor model / rated voltage / paired current evidence:
Supply idle / loaded voltage / logic voltage:
IR output voltage / captured white and black levels:
Servo center / travel / mounting:
Stage:
Procedure / expected result:
Observed result / PASS or FAIL:
PWM / surface / lighting / target geometry:
Resets / stall / temperature concerns:
Photo/video/log evidence:
Next correction and retest:
```

No physical V5 result has been filled in here. Owner reports for the earlier manual build are listed separately in README.md.
