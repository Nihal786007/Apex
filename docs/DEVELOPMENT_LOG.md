# Engineering development log

Keep future entries dated and backed by original photos, logs, code diffs or measured data. Do not replace a failed test with an undocumented “works now.”

## Earlier manual build — owner reports

- ESP32 AP/dashboard, SH1106 message display and ultrasonic measurements were reported working.
- Servo controls needed direction and travel corrections; the new specification reports successful 1000/1500/2000 µs pulses.
- Motors eventually moved, but Forward/Backward were reversed. The local baseline inverted both polarity flags.
- Movement only above “75 power” was reported; interpretation as percentage/raw PWM was not conclusively recorded.
- Driver input approximately 11.6 V and previous overheating were reported. Later “works at 12 V” is not a replacement for voltage/current ratings and measured safety checks.
- A Forward-blocking message could be overwritten by a STOP request from the browser error handler. Baseline 1.0.1 preserves the fault.

These reports establish useful troubleshooting history, not a validated autonomous system or complete power design.

## 25 October 2026 — V5 candidate

### Review before implementation

Preserved the 1.0.1 source and recorded its task/handler architecture, dependencies, pin map, polarity and message behavior. Identified blocking ranging, cross-context mutation, lack of an epoch barrier, missing IR support and unknown paired-current/power requirements.

### Implementation

- Retained WiFi/WebServer/U8g2/LEDC and the manual dashboard design language.
- Added a portable exact-testable decision core, single control owner and fixed-size queue/acknowledgement protocol.
- Added session + sequence + epoch ordering, explicit modes and independent autonomy limits.
- Replaced pulseIn with asynchronous edge capture, added GPIO34/35 debounce/calibration, and added bounded line/scan state machines.
- Kept OLED live distance while adding mode, IR/correction, commanded angle and text/state cycling.
- Adopted the requested known pulse calibration and a conservative logical travel limit.
- Defaulted hardware motor output to BENCH because mandatory electrical evidence remains missing.

### Software evidence

Native assertions execute the exact portable core. Browser tests execute the actual embedded script against mocked DOM/network. A loopback-only fixture renders labeled simulated telemetry for preview screenshots. Arduino compilation targets the installed classic ESP32 setup. Exact counts, memory figures, hashes and boundaries are recorded in BUILD_REPORT.md.

### What remains physical

Power sign-off, servo/IR calibration on the actual build, corrected direction/turns, stopping distance, line-track behavior and obstacle-course behavior have not been observed here. No performance figures or fabricated robot footage are included. The next engineering milestone is completing the staged checklist with authentic measurements.

## 27 October 2026 — source recovery

Audited 16 local source-file copies, identifying six saved firmware forms and exact duplicate groups. Added one unchanged pre-direction-correction archive; retained the original Blink, manual 1.0.1 and current four-file V5. Local single-file BENCH/motor-enabled exports remain outside the public candidate. No original standalone motor/TB6612/sensor sketches were recovered, and no historical code was reconstructed as evidence.

The [source guide](SOURCE_AND_EVIDENCE.md) records source locations, hashes, configuration differences, subsystem functions, missing files and code-to-video limits. V5 and Blink passed fresh compile-only checks; historical compile-attempt limits are recorded in BUILD_REPORT.md. Physical media cannot establish an exact firmware revision. All originals and current V5 source bytes remain unchanged.

## Future entry template

```text
Date / firmware hash:
Question or failure:
Hypothesis:
One controlled change:
Test setup and expected outcome:
Actual observation / measurement:
Evidence file:
Interpretation and limits:
Next test:
```


## 27 October 2026 — media evidence and local presentation

The owner supplied **39 video files and 14 images**, approximately **17 minutes 58 seconds** of video. All 53 file hashes differ, but edited episodes reuse earlier source shots; this is not 39 independent experiments. The initial intake inspected every image and 12 decoded samples from every video. Selected edit ranges were decoded separately for the local visual draft. Source audio is not used or represented as reviewed/cleared audio.

The material adds authentic construction, wiring, sensor/display, tablet-controller and physical-movement evidence for the **earlier manual build**. It does not establish physical V5 line following or obstacle avoidance. New acceptance evidence must identify the actual firmware and test setup.

### Chronology without invented dates

| Date / clue | What it establishes |
|---|---|
| 16 July 2026 in original session notes | The recorded date of the first Blink session; not independently established as the capture date of every clip |
| 10 August 2026 Git commit timestamps | The three original repository-history commits; not the dates all build steps happened |
| WhatsApp filename dates | Source-identification clues; forwarding/export dates can differ from original event dates |
| 27 October screen-recording filenames | Dates in the supplied recording names; edited episode order is narrative context, not an independently verified event chronology |
| 25 October local V5 build report | Recorded software verification for the source hashes in that report |

The original README/roadmap described a TB6612FNG/two-motor build. V5 targets an L298N/four-motor arrangement. The photographed driver module and recorded build stages support discussing revisions, but they do not by themselves date the transition or verify the complete current wiring. The photographed blue microservo is not automatically identified as MG90S.

### Selected media provenance

The initial README selection comprises four authentic presentation assets plus one existing labeled V5 UI preview. Five additional owner-supplied build photographs were added on 28 October, as recorded below. Frame exports preserve the recorded hardware. There is no generative reconstruction, component replacement, background cleanup or color retouching. The hero is cropped from a decoded frame to remove excess sky/floor while retaining the visible robot; JPEG exports reduce repository weight. The driver photo remains byte-identical to the original.

| Presentation asset | Original source | Source position | Processing |
|---|---|---|---|
| [apex-prototype.jpg](../assets/apex-prototype.jpg) | `WhatsApp Video 2026-10-19 at 3.14.11 PM.mp4` (V19) | 12.00 s | Display rotation honored; crop=576:432:0:218; JPEG frame export, q=2; no color, background or component retouching |
| [chassis-construction.jpg](../assets/chassis-construction.jpg) | `20261027-0634-57.1635910.mp4` (V24) | 14.50 s | Display rotation honored; JPEG frame export, q=2; no color, background or component retouching |
| [tablet-hardware.jpg](../assets/tablet-hardware.jpg) | `WhatsApp Video 2026-10-18 at 7.05.28 PM.mp4` (V14) | 6.00 s | Display rotation honored; JPEG frame export, q=2; no color, background or component retouching |
| [l298n-module.jpg](../assets/l298n-module.jpg) | `codex-clipboard-aa871cae-3566-46c9-b18b-3273dbdf2f5a.jpg` (I13) | Original photograph | Original photograph copied byte-for-byte; no enhancement or edits |
| [dashboard-manual-preview.png](../assets/dashboard-manual-preview.png) | Actual embedded HTML rendered against the local fixture | Software preview | Existing simulated-telemetry/no-robot label retained; no physical-test claim |


For audit, the source SHA-256 values are:

- `V19` / `WhatsApp Video 2026-10-19 at 3.14.11 PM.mp4`: `81d789568c1505dc31b119c9ab570184d184fddd9e8474ced7eabc6cf299643f`.
- `V24` / `20261027-0634-57.1635910.mp4`: `bde13f0c8fffd3a4dd3d816120b21eed4f85b7a4e67d08beae253900cbe969a0`.
- `V14` / `WhatsApp Video 2026-10-18 at 7.05.28 PM.mp4`: `194b3f9bb08a3d2c1bcba996fdadfbaab9abc51c07b8a8d23ddb992ea18258f6`.
- `I13` / `codex-clipboard-aa871cae-3566-46c9-b18b-3273dbdf2f5a.jpg`: `201eddca066708486cd47b81064fa005b39777d33bc948a2077ccbfb255efffb`.


The hero and tablet frame are from the earlier physical build, not a V5 autonomous acceptance session. The construction still comes from a screen recording of an edited episode; its modest resolution and visible cursor are not concealed.

### Material deliberately left out of the presentation

- The two supplied robot illustrations are concept/reference material, not photographs of the actual build.
- The clip named `WhatsApp Video 2026-10-19 at 3.25.04 PM.mp4` shows a third-party RC-transmitter webpage; it is research context, not APEX hardware evidence.
- The clip named `WhatsApp Video 2026-10-18 at 5.21.41 PM.mp4` yielded black inspection samples and is held for a separate playback check.
- Repeated episode versions, promotional effects, memes and unreviewed source music/audio are not used as technical evidence in the main storyboard.
- Unselected originals and the private intake inventory stay outside the public checkout. Existing public historical media is retained rather than silently removed.

The default four-file V5 source, its recorded hashes and BENCH configuration are unchanged by this presentation work. The owner approved publication on 27 October; subsequent presentation additions preserve those source files.

## 28 October 2026 — additional physical-build photographs

The owner supplied five further authentic photographs to clarify the mechanical layout, upper-deck electronics and internal wiring. The date here is the intake date, not an independently established capture date or firmware revision. All five originals are preserved outside the public checkout. Public copies honor EXIF orientation, resize proportionally to a maximum dimension of 1200 pixels and use optimized JPEG quality 84 with metadata omitted. There is no cropping, retouching, component replacement or generative alteration. Combined public size: 1,100,362 bytes.

| Public asset | Original photograph | What is visible |
|---|---|---|
| [Top-deck electronics](../assets/build-details/top-deck-electronics.jpg) | `codex-clipboard-2e8af450-9276-4bdd-b3e1-2771b32c62e6.jpg` | ESP32, OLED, pushbutton, converter/display and sensor wiring |
| [Lower-deck overview](../assets/build-details/lower-deck-overview.jpg) | `codex-clipboard-1a102628-b87c-451a-b052-6c25725cb07b.jpg` | Battery holder, L298N-style driver, connectors, wiring and edge-mounted sensor boards |
| [Battery and motor driver](../assets/build-details/battery-and-motor-driver.jpg) | `codex-clipboard-bc735cac-861d-415c-99b8-4b7e26c9e6b5.jpg` | Closer view of battery placement, rocker switch, driver and lever connectors |
| [Decks opened](../assets/build-details/decks-open.jpg) | `codex-clipboard-e506a07d-eb7c-4355-8404-e5326cc7d70f.jpg` | Upper-deck modules beside the exposed lower power/motor deck |
| [Assembled side view](../assets/build-details/assembled-side-view.jpg) | `codex-clipboard-66d1aa1c-6629-492f-8e87-2b061500afde.jpg` | Chassis spacing, wheels, gearmotors, raised deck and ultrasonic mount |

These photos provide construction evidence, including inspection states with loose leads. They do not establish wire continuity, the converter's selected input/output display mode, actual motor voltage/current, exact sensor assignments, running firmware or successful physical autonomy. The V5 two-input sensor configuration and pending acceptance status remain unchanged.
