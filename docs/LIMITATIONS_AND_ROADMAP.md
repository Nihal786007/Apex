# Known limitations and roadmap

## Current limitations

1. **Electrical design unverified.** Actual TT ratings, paired startup/stall current, loaded rails, regulator jumper, thermal behavior and IR voltage are missing. BENCH stays default.
2. **No electrical or motion feedback.** Firmware cannot measure motor voltage/current/temperature, detect every stall, verify servo shaft angle or confirm wheel rotation.
3. **Two digital IR sensors are geometrically ambiguous.** Narrow-line both-WHITE can also mean completely lost; broad both-BLACK does not classify a junction. The conservative timeouts may stop simple-looking tracks.
4. **Ultrasonic sensing is a sparse front beam.** Soft/angled/narrow/off-axis targets may be missed. Scan consistency does not prove obstacle-free chassis clearance or detect every drop-off.
5. **Timed pivots depend on the platform.** Battery, motor friction, surface and load change turn angle. Conservative capped PWM may not start the present motor/driver combination; do not conceal this with overvoltage.
6. **No rear sensor.** No automatic backing away. Reverse and pivots require supervised clearance.
7. **No mapping or maze guarantee.** The exploration heuristic can get trapped, oscillate or stop in a course a human considers easy.
8. **Software stops are not certified safety devices.** Timing depends on scheduling and physical coast. Hardware faults or a forced enable can defeat software output intent.
9. **Configuration/calibration reset on reboot.** No EEPROM/Preferences persistence is silently trusted. Current constants must be reviewed after hardware changes.
10. **Local demo credentials.** The published AP password is for a trusted demonstration environment, not secure public operation.

## Roadmap with evidence gates

| Milestone | Work | Evidence required before claiming completion |
|---|---|---|
| V5 bench candidate | Compile, native/UI tests, baseline preservation, real sensor telemetry | Build report, reproducible tests and honest fixture labels |
| V5 electrical validation | Identify motors; select motor rail/driver; verify paired current and safe logic levels | Datasheets/labels, measured rails, jumper photo, current/thermal evidence |
| V5 manual acceptance | Calibrate polarity, servo center, stop/coast and communications failure | Raised-wheel then floor checklist and original video |
| V5 autonomous acceptance | Fit track geometry; tune low-risk scan/turn/recovery behavior | Repeatable labeled course tests, failures and limitations |
| V5.1 observability | Runtime heap/stack telemetry, task watchdog integration, structured logs and optionally reviewed calibration persistence | Stress/reconnect/reset tests, log analysis, no regression in stops |
| V6 motion feedback | Wheel encoders and measured velocity/turning control | Encoder calibration, odometry error and controlled speed tests |
| V6 sensing | Multi-element line array/analog reflectance, rear/side range or other mapping sensors | Measured detection coverage and failure cases |
| Later navigation | Localization/map representation and planning appropriate to added sensors | Clearly defined maze/course benchmarks, recorded runs and failure rate |

Consider hardware watchdog/enable gating and electrical protection as part of the verified power design. Do not claim that software runtime limits alone implement current or thermal protection. Select optional hardware only after actual ratings, available pins, cost and test goals are known.

For an engineering/admissions portfolio, explain one genuine failure, the measurement that changed the hypothesis, the code/hardware change and the repeatable test. That evidence is more defensible than unsupported adjectives or claims of universal autonomy.
