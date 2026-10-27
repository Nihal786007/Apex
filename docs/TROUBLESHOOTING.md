# Troubleshooting by observed condition

| Observation | Meaning / next check |
|---|---|
| Requested commands change, applied PWM stays zero | Normal in BENCH. Read the banner. Do not enable until electrical sign-off is complete |
| Forward blocked, Backward responds | Read retained readiness reason: center/settle, no valid fresh echo, or obstacle hysteresis. Put a broad target 50–100 cm forward and ensure the sensor is not aimed into floor/chassis |
| Forward ready but motor does not physically turn | It is not proof of electrical output or wheel motion. Check BENCH gate, driver logic power, enables, loaded rail, paired motor orientation/current and drag |
| Mode button stops robot | Intended. Mode changes disarm. Choose power, Arm, then START for autonomy |
| START blocked | Check mode, arming, nonzero power, fresh clear center, line calibration and stable IR. Automatic re-arming is deliberately absent |
| Servo controls disabled | Pause autonomy first; scan ownership must not compete with a manual slider |
| Sensor settling message | Wait at least 500 ms and for a new valid ping; scan samples must be tagged after settling |
| No echo stops autonomy | Intended fail-closed behavior. Open/angled/soft/narrow targets may not return a usable echo. Do not interpret no echo as infinite free space |
| Scan inconsistent | Three-sample spread exceeded tolerance. Inspect sensor wiring/target geometry, servo vibration and settling; do not blindly average a moving target into a clear path |
| Robot trapped | No clear sampled path, recovery bound or repeated alternation reached. No rear sensor means no autonomous reverse. Inspect area and recover manually only when safe |
| Line mode never starts | Capture WHITE and BLACK separately with both sensors over broad patches. Each input must change. Verify 3.3 V-safe output, stable height and comparator setting |
| Line corrections steer away | Verify left/right IR assignment, both motor-side assignments and black polarity. Change one mapping at a time with power off |
| Both WHITE causes stop | Default both-BLACK geometry treats it as lost line. Narrow-line both-WHITE geometry cannot distinguish centered from lost and has a 1 s evidence timeout |
| Both BLACK causes stop in narrow-line geometry | It is an ambiguous patch/junction; two digital sensors cannot reliably classify it |
| Motor starts only at high percentage | Percentage alone is not a diagnosis. Stop a stalled/humming motor. Identify ratings, measure loaded supply/logic rail, assess driver loss, paired current and mechanical load; do not increase raw 3S voltage |
| Stops after about 8 s / 30 s | Continuous-command / autonomous-run guard. Inspect setup and restart deliberately; these are not thermal feedback |
| Stops when switching apps / losing Wi-Fi | Intended. Page requests STOP; firmware hold/lease expiry is the backup |
| State changed / stale command rejected | A stop/fault/mode change invalidated an older request. Clear the condition and repeat deliberately. Do not auto-retry Arm/START |
| OLED missing | Verify SH1106 128×64, power, SDA21/SCL22, 3.3 V-compatible pull-ups and 0x3C/0x3D. Control can continue without display; do not treat its absence as a power-safety pass |
| OLED distance shows NO ECHO while custom text works | Display path works; range path still needs diagnosis. Check TRIG5/ECHO19 divider, sensor power and target |
| ESP32 resets or fails boot when driver connected | Motor noise/power sag or strap-pin loading may be involved. Disconnect motor power; inspect common return paths, supply arrangement and GPIO12/GPIO5 boot levels |

The dashboard's angle and direction are commands, not measured shaft/wheel motion. Applied PWM means firmware output intent after the bench/neutral gate; a stuck enable jumper, bad driver or disconnected motor still needs electrical inspection.
