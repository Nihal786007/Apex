# APEX — engineering demonstration and source timeline

**Final caption-led edition approved, 27 October 2026.** The 80-second engineering film uses supplied authentic footage, meaningful on-screen text, one explicitly labeled V5 software preview and original instrumental music. Narration is intentionally omitted by the owner’s final editorial decision. The MP4 is kept outside normal Git history; a public demonstration link is added only after its hosting is verified.

## Exact timeline and source ranges

Ranges below use seconds from the start of the identified supplied file. Source-out is exclusive. Every duration and source bound is checked; normal playback speed is preserved. Portrait footage is fitted within the landscape canvas without stretching or generating missing image areas. The final edition preserves the approved **80.00-second** timeline and source ranges, using a brief 0.12-second dip at shot boundaries without overlapping or retiming sources.

| Film timeline | Source key | Source in–out | On-screen message |
|---|---|---|---|
| 00–06 s | V19 | 10.00–16.00 s | APEX Ultimate \| ESP32 robotics platform |
| 06–11 s | PUBLIC-BLINK | 32.00–37.00 s | Start with a working controller |
| 11–18 s | V24 | 8.00–15.00 s | Build the chassis |
| 18–25 s | V27 | 7.00–14.00 s | Connect one subsystem at a time |
| 25–31 s | V06 | 4.00–10.00 s | Make the integration visible |
| 31–39 s | V14 | 2.00–10.00 s | Local tablet control \| servo and distance |
| 39–46 s | V18 | 1.00–8.00 s | Send text to the OLED |
| 46–56 s | V20 | 39.00–49.00 s | Manual driving |
| 56–63 s | V20 | 19.00–26.00 s | Movement on the floor |
| 63–70 s | V10 | 0.00–7.00 s | Debug direction, power and fault handling |
| 70–75 s | V5-UI | 0.00–5.00 s | V5 software candidate \| 232 core assertions |
| 75–80 s | HERO-STILL | 0.00–5.00 s | Next: electrical validation, line following and obstacle-course tests |


## Source key and evidence boundary

| Key | Original filename / asset | Meaning |
|---|---|---|
| V19 | `WhatsApp Video 2026-10-19 at 3.14.11 PM.mp4` | Real assembled robot; no autonomous label. |
| PUBLIC-BLINK | `first program of blinking esp32 and the output.mp4` | Existing original Blink footage, not the subtitled screen recording. |
| V24 | `20261027-0634-57.1635910.mp4` | Existing episode recording; retain honest lower source resolution. |
| V27 | `20261027-0639-04.4976117.mp4` | Hands-on wiring; do not imply circuit sign-off. |
| V06 | `WhatsApp Video 2026-10-18 at 5.21.19 PM.mp4` | Internal wiring view; no cosmetic reconstruction. |
| V14 | `WhatsApp Video 2026-10-18 at 7.05.28 PM.mp4` | Earlier manual controller and physical hardware in the same shot. |
| V18 | `WhatsApp Video 2026-10-18 at 7.06.53 PM.mp4` | Preserve the tablet-to-display sequence; no invented latency metric. |
| V20 | `WhatsApp Video 2026-10-19 at 3.15.00 PM.mp4` | Tablet interaction and robot; do not splice in an autonomous claim. |
| V10 | `WhatsApp Video 2026-10-18 at 7.04.19 PM.mp4` | Code insert illustrates development; specific lessons are supported by project notes, not claimed proven by this shot. |
| V5-UI | `assets/dashboard-manual-preview.png` | Hold the labeled simulated-telemetry preview; visibly retain that label. |
| HERO-STILL | `V19 original source frame at 12.00 s (full frame)` | Still from V19 at 12s; explicitly physical acceptance pending. |


The raw inputs are private owner-supplied media; filenames are provenance, not broken download links. The existing public Blink source remains at [assets/videos/first program of blinking esp32 and the output.mp4](../assets/videos/first%20program%20of%20blinking%20esp32%20and%20the%20output.mp4). Selected still-source hashes and processing are recorded in the [development log](DEVELOPMENT_LOG.md#selected-media-provenance).

The two V20 segments are separate excerpts from one recording, not a continuous single run. The film must not suggest that an obstacle decision or stopped state from a different shot caused the observed motion. None of the movement shots is labeled autonomous. The V5 preview keeps its simulated-telemetry/no-connected-robot label visible.

“Normal speed” means 1× playback of each supplied file, with no additional retiming in this edit. Edited episode recordings may already contain cuts, overlays or prior speed changes; they are not treated as continuous measured real-time trials.

## Final presentation

The final film preserves the existing twelve-cut edit. Its review-only header was replaced with **ENGINEERING DEMONSTRATION / 2026**; no footage was rebuilt, reconstructed or changed to invent behavior. Chapter and engineering captions use 44 px typography, context labels use 28 px, and restrained title motion leaves hardware evidence visible. Landscape/full-screen viewing is recommended on phones. Earlier physical footage explicitly says **EXACT FIRMWARE REVISION NOT VERIFIED**; the V5 panel says **SIMULATED TELEMETRY / NO ROBOT CONNECTED**. The ending identifies electrical verification and physical autonomous tests as the next milestone.

### Timed engineering captions

| Timeline | On-screen engineering caption |
|---|---|
| 00–06 s | APEX is an ESP32 robotics project, connecting mechanical assembly, electronics, and software. |
| 06–11 s | The journey began with a Blink test and a working controller. |
| 11–18 s | A chassis, four gearmotors, and wheels turned individual components into a mobile platform. |
| 18–25 s | Power, signals, and shared grounds brought the subsystems together. Electrical sign-off remains pending. |
| 25–31 s | The wiring record makes integration visible, including the details that still need checking. |
| 31–39 s | Earlier footage shows local tablet control, servo commands, and a distance interface. |
| 39–46 s | OLED messages connect browser input with a physical display on the robot. |
| 46–56 s | These recordings demonstrate manual driving on the floor. They do not establish autonomous behavior or measured performance. |
| 56–63 s | This is a separate excerpt from the same recording, shown at normal speed. |
| 63–70 s | Development also involved correcting direction and investigating power delivery, startup behavior, and fault feedback. |
| 70–75 s | V5 passes 232 core assertions. This dashboard uses simulated telemetry. |
| 75–80 s | Next: electrical checks, physical line following, and repeatable obstacle-course tests. |

## Export and verification

Filename: **APEX_Engineering_Demonstration_2026.mp4**. **80.00 seconds / 2,400 frames / 1920×1080 / 30 fps**, H.264/yuv420p, AAC stereo 48 kHz. Size: **20,909,238 bytes (19.94 MiB)**. SHA-256: `1917eacb2bf412b1507ff5345e0654cb6f0bdc3f84d819bfca6baa2ad3a35a7f`.

The final export was produced from the existing approved master, preserving its audio stream and edit; originals and the review master remain unchanged. Full decoding and signal checks passed. No black interval of at least 0.20 seconds or audio silence of at least 2 seconds at −60 dB was detected. The only detected still holds of at least 2 seconds are intentional: the labeled software panel at **70–75 s** and authentic closing image at **75–80 s**. Representative source/evidence/ending frames and full-resolution captions were visually checked. No speech synchronization is required in this caption-led edition.

## Music provenance and portfolio-use permission

The instrumental score was newly generated for this project from synthesized tones: a quiet repeating chord bed with gradual attack/release, no vocals, no third-party recordings, no samples and no purchased track. It is editorial background, not robot sound. No soundtrack from the supplied episode recordings is reused. This project-generated instrumental is supplied under the repository’s **MIT License**, permitting public GitHub/portfolio use with its existing notice. That permission does not grant rights to unrelated third-party media.

The final video copies the approved master’s AAC music stream. The measured bed was approximately **−28.95 LUFS integrated / −19.73 dBTP**, intentionally quiet and without clipping. Voice generation and recording are not release requirements; no subscription or external media service was purchased.

## Publication and evidence boundary

Keep the MP4 as a release asset or equivalent verified public attachment, rather than committing it into normal Git history. Any GitHub release is explicitly an **APEX V5 SOFTWARE RELEASE CANDIDATE** with BENCH motor output disabled. The media documents earlier manual-build experiments and the later software candidate; it does not certify the electrical design or establish physical V5 autonomous acceptance. A failed hosting operation must be reported honestly, with no invented public URL.

## Next physical demonstrations

Line following and obstacle avoidance remain pending. Future footage should identify the actual firmware/version, wiring and test setup, then show continuous normal-speed behavior and failure/stop cases after electrical sign-off. Follow [TEST_PLAN.md](TEST_PLAN.md), retain original recordings and document failures. Current software evidence is in [BUILD_REPORT.md](BUILD_REPORT.md); it does not substitute for physical acceptance.
