#pragma once
/* Portable C/C++ decision core. No Arduino, heap allocation, GPIO or
 * networking. The control task is its sole owner. Tests execute this exact
 * implementation.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum { MANUAL, LINE_FOLLOW, OBSTACLE_AVOID } ApexMode;
typedef enum {
  IDLE,
  MANUAL_HOLD,
  LINE_TRACK,
  LINE_RECOVER,
  OA_CRUISE,
  SCAN_LEFT,
  SCAN_CENTER,
  SCAN_RIGHT,
  CHOOSE_PATH,
  PRE_TURN,
  TURN_LEFT,
  TURN_RIGHT,
  RECENTER
} ApexPhase;
typedef enum {
  CLAIM,
  ARM,
  START,
  PAUSE,
  STOP,
  DISARM,
  SET_MODE,
  DRIVE,
  RELEASE,
  SET_POWER,
  SET_PAN,
  SET_TEXT,
  CAL_WHITE,
  CAL_BLACK,
  TUNE,
  LEASE
} ApexCommandType;
typedef struct {
  ApexCommandType type;
  uint64_t session;
  uint32_t seq, epoch, ticket;
  int a, b, c, d, e;
  char text[41];
} ApexCommand;
typedef struct {
  uint32_t at, sampleId;
  int rangePan, irLeft, irRight;
  float cm;
  uint8_t rangeValid, irStable;
} ApexInput;
typedef struct {
  uint32_t holdMs, leaseMs, runMs, motorOnMs, staleMs, settleMs;
  uint32_t recoverMs, whiteBlindMs, scanTimeoutMs, turnMs;
  int stopCm, clearCm, sideClearCm, correction, safeTravel;
  int blackLeft, blackRight, whiteCenter, panMounted;
} ApexSettings;
typedef struct {
  uint32_t ticket, epoch;
  int ok;
  char message[96];
} ApexAck;
typedef struct {
  ApexSettings cfg;
  ApexInput in;
  ApexMode mode;
  ApexPhase phase;
  uint64_t session;
  uint32_t seq, epoch, holdAt, leaseAt, runAt, phaseAt, servoAt;
  uint32_t motorAt, lastSample, scanSample, lastEvidence, clearSince,
      irUnstableAt;
  int power, pan, left, right, manualDirection, lastCorrection;
  int scanCount, scanIndex, chosenTurn, priorTurn, recoveries, oscillations;
  float scanValues[3], scanDistances[3];
  uint8_t armed, running, motorTimer, obstacle, whiteSaved, blackSaved;
  uint8_t calibrated, whiteL, whiteR, blackL, blackR, irUnstable;
  char status[96], text[41];
} ApexCore;

static uint32_t apex_elapsed(uint32_t now, uint32_t then) { return now - then; }
static int apex_clamp(int n, int lo, int hi) {
  return n < lo ? lo : (n > hi ? hi : n);
}
static const char* apex_mode_name(ApexMode m) {
  return m == MANUAL        ? "MANUAL"
         : m == LINE_FOLLOW ? "LINE FOLLOWING"
                            : "OBSTACLE AVOIDANCE";
}
static const char* apex_phase_name(ApexPhase p) {
  switch (p) {
    case MANUAL_HOLD:
      return "MANUAL COMMAND";
    case LINE_TRACK:
      return "LINE TRACKING";
    case LINE_RECOVER:
      return "RECOVERING LINE";
    case OA_CRUISE:
      return "PATH CLEAR";
    case SCAN_LEFT:
      return "SCANNING LEFT";
    case SCAN_CENTER:
      return "SCANNING CENTER";
    case SCAN_RIGHT:
      return "SCANNING RIGHT";
    case CHOOSE_PATH:
      return "CHOOSING PATH";
    case PRE_TURN:
      return "CENTERING BEFORE TURN";
    case TURN_LEFT:
      return "TURNING LEFT";
    case TURN_RIGHT:
      return "TURNING RIGHT";
    case RECENTER:
      return "RECENTERING";
    default:
      return "STOPPED";
  }
}
static void apex_status(ApexCore* c, const char* s) {
  snprintf(c->status, sizeof(c->status), "%s", s);
}
static void apex_zero(ApexCore* c) {
  c->left = 0;
  c->right = 0;
}
static void apex_stop(ApexCore* c, const char* why) {
  apex_zero(c);
  c->armed = 0;
  c->running = 0;
  c->phase = IDLE;
  c->manualDirection = 'S';
  c->motorTimer = 0;
  c->epoch++;
  apex_status(c, why);
}
static void apex_servo(ApexCore* c, int angle, uint32_t now) {
  angle = apex_clamp(angle, 90 - c->cfg.safeTravel, 90 + c->cfg.safeTravel);
  if (c->pan != angle) {
    c->pan = angle;
    c->servoAt = now;
    if (c->cfg.panMounted) c->obstacle = 1;
  }
}
static void apex_phase(ApexCore* c, ApexPhase phase, uint32_t now,
                       const char* msg) {
  c->phase = phase;
  c->phaseAt = now;
  apex_status(c, msg);
}
static int apex_range_fresh(const ApexCore* c, uint32_t now) {
  return c->in.sampleId && c->in.rangeValid &&
         c->in.rangePan == (c->cfg.panMounted ? c->pan : 90) &&
         apex_elapsed(now, c->in.at) <= c->cfg.staleMs &&
         (!c->cfg.panMounted ||
          ((int32_t)(c->in.at - c->servoAt) >= 0 &&
           apex_elapsed(c->in.at, c->servoAt) >= c->cfg.settleMs));
}
static const char* apex_forward_block(const ApexCore* c, uint32_t now) {
  if (c->cfg.panMounted && c->pan != 90) return "Center the front sensor";
  if (c->cfg.panMounted && apex_elapsed(now, c->servoAt) < c->cfg.settleMs)
    return "Sensor settling";
  if (!apex_range_fresh(c, now)) return "No fresh valid forward echo";
  if (c->obstacle) return "Obstacle lock: clear beyond release distance";
  return "";
}
static void apex_defaults(ApexCore* c) {
  memset(c, 0, sizeof(*c));
  c->mode = MANUAL;
  c->phase = IDLE;
  c->pan = 90;
  c->epoch = 1;
  c->obstacle = 1;
  c->manualDirection = 'S';
  c->cfg.holdMs = 450;
  c->cfg.leaseMs = 1800;
  c->cfg.runMs = 30000;
  c->cfg.motorOnMs = 8000;
  c->cfg.staleMs = 350;
  c->cfg.settleMs = 500;
  c->cfg.recoverMs = 900;
  c->cfg.whiteBlindMs = 1000;
  c->cfg.scanTimeoutMs = 1400;
  c->cfg.turnMs = 300;
  c->cfg.stopCm = 25;
  c->cfg.clearCm = 30;
  c->cfg.sideClearCm = 35;
  c->cfg.correction = 65;
  c->cfg.safeTravel = 60;
  c->cfg.blackLeft = 1;
  c->cfg.blackRight = 1;
  c->cfg.panMounted = 1;
  apex_status(c, "Stopped: bench setup ready");
  snprintf(c->text, sizeof(c->text), "APEX READY");
}
static void apex_calibrated(ApexCore* c) {
  c->calibrated = c->whiteSaved && c->blackSaved && c->whiteL != c->blackL &&
                  c->whiteR != c->blackR;
  if (c->calibrated) {
    c->cfg.blackLeft = c->blackL;
    c->cfg.blackRight = c->blackR;
  }
}
static ApexAck apex_command(ApexCore* c, const ApexCommand* q, uint32_t now) {
  ApexAck ack;
  memset(&ack, 0, sizeof(ack));
  ack.ticket = q->ticket;
  const char* error = "";
  if (q->type == CLAIM) {
    apex_stop(c, "New controller: outputs stopped");
    c->session = q->session;
    c->seq = 0;
    c->power = 0;
    c->leaseAt = now;
    ack.ok = 1;
  } else if (!c->session || q->session != c->session) {
    error = "Controller session expired";
  } else if (q->type == STOP || q->type == PAUSE || q->type == DISARM) {
    if (q->seq > c->seq) c->seq = q->seq;
    apex_stop(c, q->type == PAUSE    ? "Paused: Arm and Start again"
                 : q->type == DISARM ? "Disarmed"
                                     : "Emergency STOP: re-arm deliberately");
    ack.ok = 1;
  } else if (q->seq <= c->seq) {
    error = "Stale command rejected";
  } else {
    c->seq = q->seq;
    if (q->type != LEASE && q->type != RELEASE && q->epoch != c->epoch) {
      error = "State changed: repeat action deliberately";
    } else if (q->type == LEASE) {
      c->leaseAt = now;
      ack.ok = 1;
    } else if (q->type == RELEASE) {
      if (c->mode == MANUAL) {
        apex_zero(c);
        c->manualDirection = 'S';
        c->phase = IDLE;
        c->epoch++;
        c->motorTimer = 0;
        if (c->armed) apex_status(c, "Released: stopped");
        ack.ok = 1;
      } else
        error = "Manual drive is unavailable in this mode";
    } else if (q->type == SET_MODE) {
      if (q->a < MANUAL || q->a > OBSTACLE_AVOID)
        error = "Invalid mode";
      else {
        apex_stop(c, "Mode changed: stopped and disarmed");
        c->mode = (ApexMode)q->a;
        apex_servo(c, 90, now);
        ack.ok = 1;
      }
    } else if (q->type == ARM) {
      apex_zero(c);
      c->manualDirection = 'S';
      c->running = 0;
      c->armed = 1;
      c->phase = IDLE;
      c->epoch++;
      c->leaseAt = now;
      apex_status(c, "Armed: hold manually or START selected autonomy");
      ack.ok = 1;
    } else if (q->type == START) {
      if (c->mode == MANUAL)
        error = "Manual mode uses held direction buttons";
      else if (!c->armed)
        error = "Arm before START";
      else if (c->power == 0)
        error = "Set a nonzero power command";
      else if (c->mode == OBSTACLE_AVOID && !c->cfg.panMounted)
        error = "Obstacle mode requires a pan-mounted sensor";
      else if (c->mode == LINE_FOLLOW && !c->calibrated)
        error = "Capture WHITE and BLACK IR calibration first";
      else if (c->mode == LINE_FOLLOW && !c->in.irStable)
        error = "IR inputs must be stable before START";
      else if (apex_forward_block(c, now)[0])
        error = apex_forward_block(c, now);
      else {
        c->running = 1;
        c->runAt = now;
        c->leaseAt = now;
        c->lastEvidence = now;
        c->recoveries = 0;
        c->oscillations = 0;
        c->priorTurn = 0;
        c->lastCorrection = 0;
        c->epoch++;
        apex_phase(c, c->mode == LINE_FOLLOW ? LINE_TRACK : OA_CRUISE, now,
                   "Autonomy started");
        ack.ok = 1;
      }
    } else if (q->type == DRIVE) {
      if (c->mode != MANUAL)
        error = "Select MANUAL before using drive buttons";
      else if (!c->armed)
        error = c->status;
      else if (c->power == 0)
        error = "Set a nonzero power command";
      else if (q->a != 'F' && q->a != 'B' && q->a != 'L' && q->a != 'R')
        error = "Invalid direction";
      else if (q->a == 'F' && apex_forward_block(c, now)[0])
        error = apex_forward_block(c, now);
      else {
        c->manualDirection = q->a;
        c->holdAt = now;
        c->leaseAt = now;
        c->phase = MANUAL_HOLD;
        ack.ok = 1;
        apex_status(c, q->a == 'F'   ? "Forward command"
                       : q->a == 'B' ? "Backward command"
                       : q->a == 'L' ? "Left pivot command"
                                     : "Right pivot command");
      }
    } else if (q->type == SET_POWER) {
      if (q->a < 0 || q->a > 255)
        error = "Power must be 0..255";
      else {
        c->power = q->a;
        ack.ok = 1;
        if (q->a == 0) apex_stop(c, "Zero power: stopped and disarmed");
      }
    } else if (q->type == SET_PAN) {
      if (c->running)
        error = "Pause autonomy before manual servo control";
      else if (q->a < 90 - c->cfg.safeTravel || q->a > 90 + c->cfg.safeTravel)
        error = "Angle outside safe commanded travel";
      else {
        apex_stop(c, "Servo command: re-arm before driving");
        apex_servo(c, q->a, now);
        ack.ok = 1;
      }
    } else if (q->type == SET_TEXT) {
      snprintf(c->text, sizeof(c->text), "%.40s", q->text);
      ack.ok = 1;
    } else if (q->type == CAL_WHITE || q->type == CAL_BLACK) {
      if (c->running || c->left || c->right)
        error = "Stop before IR calibration";
      else if (!c->in.irStable)
        error = "Hold both sensors steady over the calibration surface";
      else {
        apex_stop(c, "IR sample captured; check calibration status");
        if (q->type == CAL_WHITE) {
          c->whiteSaved = 1;
          c->whiteL = c->in.irLeft;
          c->whiteR = c->in.irRight;
        } else {
          c->blackSaved = 1;
          c->blackL = c->in.irLeft;
          c->blackR = c->in.irRight;
        }
        apex_calibrated(c);
        ack.ok = 1;
      }
    } else if (q->type == TUNE) {
      if (c->running || c->left || c->right)
        error = "Pause before tuning";
      else if (q->a < 0 || q->a > 180 || q->b < 15 || q->b > 100 || q->c < 0 ||
               q->c > 1 || q->d < 300 || q->d > 1500 || q->e < 100 ||
               q->e > 600)
        error = "Tuning value outside safe software bounds";
      else {
        apex_stop(c, "Tuning applied; re-arm deliberately");
        c->cfg.correction = q->a;
        c->cfg.stopCm = q->b;
        c->cfg.clearCm = q->b + 5;
        c->cfg.sideClearCm = q->b + 10;
        c->cfg.whiteCenter = q->c;
        c->cfg.recoverMs = q->d;
        c->cfg.turnMs = q->e;
        c->obstacle = 1;
        ack.ok = 1;
      }
    } else
      error = "Unknown command";
  }
  if (error[0]) {
    char copy[96];
    snprintf(copy, sizeof(copy), "%s", error);
    if (q->session == c->session &&
        (q->type == DRIVE || q->type == START || q->type == ARM ||
         q->type == SET_MODE || q->type == SET_PAN))
      apex_stop(c, copy);
    snprintf(ack.message, sizeof(ack.message), "%s", copy);
  } else
    snprintf(ack.message, sizeof(ack.message), "%s", c->status);
  ack.epoch = c->epoch;
  return ack;
}

static void apex_scan_begin(ApexCore* c, uint32_t now) {
  apex_zero(c);
  c->recoveries++;
  if (c->recoveries > 3) {
    apex_stop(c, "Trapped: recovery limit; no rear sensor for reversing");
    return;
  }
  c->scanIndex = 0;
  c->scanCount = 0;
  c->scanSample = c->in.sampleId;
  apex_servo(c, 90 - c->cfg.safeTravel, now);
  apex_phase(c, SCAN_LEFT, now, "Obstacle detected: scanning LEFT");
}
static float apex_median3(float a, float b, float d) {
  float t;
  if (a > b) {
    t = a;
    a = b;
    b = t;
  }
  if (b > d) {
    t = b;
    b = d;
    d = t;
  }
  if (a > b) b = a;
  return b;
}
static void apex_line(ApexCore* c, uint32_t now) {
  int l = c->in.irLeft == c->cfg.blackLeft,
      r = c->in.irRight == c->cfg.blackRight;
  if (!c->in.irStable) {
    if (!c->irUnstable) {
      c->irUnstable = 1;
      c->irUnstableAt = now;
    }
    if (apex_elapsed(now, c->irUnstableAt) > 250)
      apex_stop(c, "IR inputs unstable: stopped");
    apex_zero(c);
    return;
  }
  c->irUnstable = 0;
  if (l != r) {
    c->lastEvidence = now;
    c->lastCorrection = l ? -1 : 1;
    c->phase = LINE_TRACK;
    int slow = apex_clamp(c->power - c->cfg.correction, 0, 255);
    c->left = l ? slow : c->power;
    c->right = r ? slow : c->power;
    apex_status(c, l ? "Line correction LEFT" : "Line correction RIGHT");
    return;
  }
  if ((l && r && !c->cfg.whiteCenter) || (!l && !r && c->cfg.whiteCenter)) {
    if (c->cfg.whiteCenter &&
        apex_elapsed(now, c->lastEvidence) > c->cfg.whiteBlindMs) {
      apex_stop(c, "Both WHITE cannot verify a narrow line: evidence timeout");
      return;
    }
    c->phase = LINE_TRACK;
    c->left = c->right = c->power;
    apex_status(c, c->cfg.whiteCenter
                       ? "Centered WHITE pattern; awaiting edge evidence"
                       : "Centered BLACK pattern");
    return;
  }
  if (c->cfg.whiteCenter && l && r) {
    apex_stop(c, "Both BLACK ambiguous: junction or patch; stopped");
    return;
  }
  if (c->phase != LINE_RECOVER) {
    apex_phase(c, LINE_RECOVER, now, "Line lost: bounded recovery");
    apex_zero(c);
  }
  uint32_t elapsed = apex_elapsed(now, c->phaseAt);
  if (elapsed >= c->cfg.recoverMs || !c->lastCorrection) {
    apex_stop(c, "Line not recovered: stopped");
    return;
  }
  if (elapsed < 100) {
    apex_zero(c);
    return;
  }
  int side = c->lastCorrection;
  if (elapsed > c->cfg.recoverMs / 2) side = -side;
  int p = c->power < 100 ? c->power : 100;
  c->left = side < 0 ? -p : p;
  c->right = -c->left;
}
static void apex_avoid(ApexCore* c, uint32_t now) {
  if (c->phase == OA_CRUISE) {
    if (!apex_range_fresh(c, now) || c->pan != 90) {
      apex_stop(c, "Autonomy stopped: missing fresh forward echo");
      return;
    }
    if (c->obstacle) {
      apex_scan_begin(c, now);
      return;
    }
    c->left = c->right = c->power;
    apex_status(c, "PATH CLEAR: forward command");
    if (!c->clearSince) c->clearSince = now;
    if (apex_elapsed(now, c->clearSince) > 1800) {
      c->recoveries = 0;
      c->oscillations = 0;
    }
    return;
  }
  apex_zero(c);
  c->clearSince = 0;
  if (c->phase == SCAN_LEFT || c->phase == SCAN_CENTER ||
      c->phase == SCAN_RIGHT) {
    if (apex_elapsed(now, c->phaseAt) > c->cfg.scanTimeoutMs) {
      apex_stop(c, "Scan timeout: missing or invalid echo");
      return;
    }
    if (!apex_range_fresh(c, now) || c->in.sampleId == c->scanSample) return;
    c->scanSample = c->in.sampleId;
    c->scanValues[c->scanCount++] = c->in.cm;
    if (c->scanCount < 3) return;
    float lo = c->scanValues[0], hi = lo;
    int i;
    for (i = 1; i < 3; i++) {
      if (c->scanValues[i] < lo) lo = c->scanValues[i];
      if (c->scanValues[i] > hi) hi = c->scanValues[i];
    }
    float median =
        apex_median3(c->scanValues[0], c->scanValues[1], c->scanValues[2]);
    float tolerance = median * .35f;
    if (tolerance < 15) tolerance = 15;
    if (hi - lo > tolerance) {
      apex_stop(c, "Scan inconsistent: check sensor/target");
      return;
    }
    c->scanDistances[c->scanIndex++] = median;
    c->scanCount = 0;
    if (c->scanIndex == 1) {
      apex_servo(c, 90, now);
      apex_phase(c, SCAN_CENTER, now, "SCANNING CENTER");
    } else if (c->scanIndex == 2) {
      apex_servo(c, 90 + c->cfg.safeTravel, now);
      apex_phase(c, SCAN_RIGHT, now, "SCANNING RIGHT");
    } else
      apex_phase(c, CHOOSE_PATH, now, "Comparing validated scan samples");
    return;
  }
  if (c->phase == CHOOSE_PATH) {
    float l = c->scanDistances[0], f = c->scanDistances[1],
          r = c->scanDistances[2];
    if (f >= c->cfg.clearCm)
      c->chosenTurn = 0;
    else if (l < c->cfg.sideClearCm && r < c->cfg.sideClearCm) {
      apex_stop(c, "Trapped: no clear sampled direction; no blind reverse");
      return;
    } else
      c->chosenTurn = l >= r ? -1 : 1;
    if (c->chosenTurn && c->priorTurn && c->chosenTurn != c->priorTurn)
      c->oscillations++;
    if (c->oscillations >= 3) {
      apex_stop(c, "Repeated alternating turns: operator help needed");
      return;
    }
    if (c->chosenTurn) c->priorTurn = c->chosenTurn;
    apex_servo(c, 90, now);
    apex_phase(c, PRE_TURN, now, "Recentering: waiting for fresh echo");
    return;
  }
  if (c->phase == PRE_TURN || c->phase == RECENTER) {
    if (apex_elapsed(now, c->phaseAt) > c->cfg.scanTimeoutMs) {
      apex_stop(c, "Center echo timeout: stopped");
      return;
    }
    if (!apex_range_fresh(c, now) || (int32_t)(c->in.at - c->phaseAt) < 0)
      return;
    if (c->phase == RECENTER || !c->chosenTurn) {
      if (c->obstacle) {
        apex_scan_begin(c, now);
        return;
      }
      apex_phase(c, OA_CRUISE, now, "PATH CLEAR");
      return;
    }
    apex_phase(c, c->chosenTurn < 0 ? TURN_LEFT : TURN_RIGHT, now,
               c->chosenTurn < 0 ? "TURNING LEFT" : "TURNING RIGHT");
    return;
  }
  if (c->phase == TURN_LEFT || c->phase == TURN_RIGHT) {
    if (!apex_range_fresh(c, now)) {
      apex_stop(c, "Turn stopped: sensor data missing");
      return;
    }
    if (apex_elapsed(now, c->phaseAt) >= c->cfg.turnMs) {
      apex_phase(c, RECENTER, now,
                 "Turn finished: stopped for new center sample");
      return;
    }
    int p = c->power < 130 ? c->power : 130;
    c->left = c->phase == TURN_LEFT ? -p : p;
    c->right = -c->left;
    return;
  }
  apex_stop(c, "Unexpected autonomous state: stopped");
}
static void apex_tick(ApexCore* c, const ApexInput* in, uint32_t now) {
  c->in = *in;
  if (in->sampleId != c->lastSample) {
    c->lastSample = in->sampleId;
    if ((!c->cfg.panMounted || c->pan == 90) && in->rangePan == 90 &&
        (!c->cfg.panMounted || (int32_t)(in->at - c->servoAt) >= 0)) {
      if (!in->rangeValid || in->cm <= c->cfg.stopCm)
        c->obstacle = 1;
      else if (in->cm >= c->cfg.clearCm)
        c->obstacle = 0;
    }
  }
  apex_zero(c);
  if (!c->armed) return;
  if (apex_elapsed(now, c->leaseAt) > c->cfg.leaseMs) {
    apex_stop(c, "Controller lease expired: stopped");
    return;
  }
  if (c->mode == MANUAL) {
    if (c->manualDirection == 'S') {
      c->motorTimer = 0;
      return;
    }
    if (apex_elapsed(now, c->holdAt) > c->cfg.holdMs) {
      apex_stop(c, "Manual hold timeout: stopped");
      return;
    }
    if (c->manualDirection == 'F' && apex_forward_block(c, now)[0]) {
      apex_stop(c, apex_forward_block(c, now));
      return;
    }
    c->left = c->right = c->power;
    if (c->manualDirection == 'B') c->left = c->right = -c->power;
    if (c->manualDirection == 'L') c->left = -c->power;
    if (c->manualDirection == 'R') c->right = -c->power;
  } else if (c->running) {
    if (apex_elapsed(now, c->runAt) > c->cfg.runMs) {
      apex_stop(c, "Autonomous run time limit: stopped");
      return;
    }
    if (c->mode == LINE_FOLLOW) {
      const char* why = apex_forward_block(c, now);
      if (why[0]) {
        apex_stop(c, why);
        return;
      }
      apex_line(c, now);
    } else
      apex_avoid(c, now);
  }
  if (c->left || c->right) {
    if (!c->motorTimer) {
      c->motorAt = now;
      c->motorTimer = 1;
    }
    if (apex_elapsed(now, c->motorAt) > c->cfg.motorOnMs) {
      apex_stop(c, "Continuous motor-command limit: stopped");
      return;
    }
  } else
    c->motorTimer = 0;
}
