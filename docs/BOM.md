# Bill of materials — existing build and verification items

No prices, vendor identities or unmeasured electrical ratings are invented. Verify the exact parts fitted to the robot before completing the purchase/design fields.

| Item | Quantity | Role / status |
|---|---:|---|
| Classic ESP32 / ESP32-WROOM-32 development board | 1 | Controller; exact board/USB chip to record |
| L298N complete driver module | 1 | Two paired-motor channels; regulator arrangement/current/cooling unverified |
| TT geared DC motor | 4 | Two per side; model, rated voltage and startup/stall current unknown |
| Wheel / chassis assembly | Existing | Record wheel size, track width, mass and mounting |
| Positional microservo (V5 target: MG90S; fitted model unconfirmed) | 1 | Front sensor pan; 1000/1500/2000 µs pulse calibration reported. The blue servo in supplied component media is not sufficient to verify its exact model. |
| HC-SR04 | 1 | Front range sensing; ECHO must be divided |
| Digital IR line-sensor module | 2 | GPIO34/35; WHITE=0/BLACK=1 previously reported; verify voltage and calibration |
| SH1106 128×64 I2C OLED | 1 | Live distance, mode, state and text |
| Three-cell 18650 pack | Reported 1 pack | Earlier raw driver input ~11.6 V; protection/charging/cell ratings to document |
| Motor supply / rated regulator and current protection | To specify | Select after motor ratings and paired-current data are known |
| Regulated 5 V supply stage | To verify/specify | Servo, standard HC-SR04 and driver logic as appropriate |
| ECHO divider resistors | Existing suitable divider | Example 2.2 kΩ series and 3.3 kΩ to GND |
| Enable pull-down resistors | Up to 2 | Approximately 10 kΩ if the module lacks suitable pull-downs |
| Fuse, cutoff, connectors, wiring, decoupling | To verify | Ratings and physical arrangement must suit the actual loads |
| USB data cable | 1 | ESP32 programming and initial bench power |

Add real photographs, part markings and supplier datasheets as evidence. A generic label such as “TT motor” is not enough to finalize the electrical design.


## Historical hardware boundary

The original public README/roadmap described a TB6612FNG and two motors. Those records remain in Git history. The supplied component photograph shows an L298N-style module and four loose TT motors are photographed; neither establishes when every revision was fitted or verifies operating ratings. This list is the V5 documented target, not a fully verified as-built parts list. See [media provenance and chronology](DEVELOPMENT_LOG.md#selected-media-provenance).
