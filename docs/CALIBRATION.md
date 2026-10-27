# Calibration and tuning

## Servo

The owner reports successful 1000, 1500 and 2000 µs pulses. Config.h uses those as min/center/max and does not extend beyond them. The conservative commanded travel limit is 60° on either side of logical center, so the default slider spans 30–150°. At these endpoints the pulse is inside 1000–2000 µs. With SERVO_REVERSED=false, logical Left increases pulse width; Center is 1500 µs. The labels are commanded logical coordinates, not measured physical degrees.

With motor power off and the linkage unloaded: command Center, cut servo power, refit the horn so the sensor physically points forward without twisting the shaft, then repower and test small movements. Toggle SERVO_REVERSED only if physical Left/Right are wrong. Reduce travel if it binds; increase toward the already-tested pulse endpoints only after checking clearance. Buzzing/heating or weak movement calls for mechanical/power investigation, not wider pulses. No servo current or position sensor is fitted.

## IR polarity and surfaces

GPIO34/35 have no internal pull resistors. Verify both digital OUT voltages are safe for ESP32 before connecting. Set sensor height consistently, adjust each module's threshold using its own documentation, and use a broad white patch followed by a broad black patch so both sensors sample the intended surface.

1. Stop/disarm; wait until the dashboard shows stable/debounced input.
2. Place both over WHITE. Capture WHITE.
3. Place both over BLACK. Capture BLACK.
4. Calibration passes only if each channel changes between captures. The resulting black level is displayed separately for each side.
5. Repeat under the actual lighting and operating height. Prior results WHITE=0/BLACK=1 are starting evidence, not a guarantee after changing modules or thresholds.

Config.h exposes initial black levels, and capture-based calibration updates RAM polarity automatically. Identical captures fail calibration. A constant disconnected output can look like a legitimate level; a completed capture sequence is not a hardware continuity/current/voltage test.

## Line geometry

Default **both BLACK centered** needs a stripe wide enough to cover both sensors when aligned. Left-only BLACK slows the left pair; right-only BLACK slows the right pair. Both WHITE is then a lost-line pattern. A previous correction side is needed for bounded recovery; otherwise it stops instead of choosing blindly.

If the physical arrangement straddles a narrow stripe, select **both WHITE centered** in Mode tuning. This geometry cannot distinguish a centered narrow stripe from an entirely white floor with only two digital outputs. V5 requires new edge evidence within 1 second and stops otherwise. Both BLACK is treated as ambiguous and stops in this geometry. This limitation cannot be eliminated by tuning a constant; reposition sensing or add better sensors for reliable long straights/junctions.

## Power, steering and avoidance tuning

The slider is PWM 0–255 displayed as 0–100%, not measured speed or a safe-voltage control. Resolve the existing ~11.6 V/unknown-TT-motor power issue before physical tuning. No minimum-duty remapping or startup boost is added.

Pause before applying settings. Bounded UI values:

| Setting | Default | Allowed |
|---|---:|---:|
| Line correction subtraction | 65 PWM units | 0–180 |
| Obstacle stop | 25 cm | 15–100 cm |
| Release / side-clear distance | stop +5 / +10 cm | Derived |
| Lost-line recovery | 900 ms | 300–1500 ms |
| Timed pivot | 300 ms | 100–600 ms |

OA pivots cap PWM at 130/255; line recovery caps it at 100/255. These conservative values may not overcome the current build's reported starting friction/driver losses. Do not raise voltage or add a kick to hide that problem. Verify/select the motor power stage first; then tune with measured raised-wheel and floor evidence. Timed pivots are not calibrated angular turns without encoders.

Settings stop/disarm when applied. The 8 s continuous-command and 30 s autonomous-run limits are deliberate stopping guards, not bugs; restart only after inspecting the test setup. Configurable software thresholds do not replace current/thermal protection or physical stopping-distance measurements.
