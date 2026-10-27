#pragma once
#include <stdint.h>

// PHYSICAL OUTPUT GATE: keep false until docs/POWER_AND_WIRING.md is signed
// off. False is useful BENCH mode: real telemetry and FSMs work; ENA/ENB stay
// zero. This records human verification; it does NOT measure
// voltage/current/heat.
static constexpr bool MOTOR_POWER_VERIFIED = false;
static constexpr bool LEFT_MOTOR_REVERSED = true;
static constexpr bool RIGHT_MOTOR_REVERSED = true;
static constexpr bool SERVO_REVERSED = false;
static constexpr bool SENSOR_ON_SERVO = true;

static constexpr int ENA = 25, IN1 = 26, IN2 = 27, IN3 = 14, IN4 = 12, ENB = 13;
static constexpr int SERVO_PIN = 18, TRIG = 5, ECHO = 19, SDA_PIN = 21,
                     SCL_PIN = 22;
static constexpr int IR_LEFT = 34, IR_RIGHT = 35;
static constexpr uint8_t IR_BLACK_LEFT = 1, IR_BLACK_RIGHT = 1;

// Known successful pulse calibration. Degrees are commanded coordinates only.
static constexpr int SERVO_MIN_US = 1000, SERVO_CENTER_US = 1500,
                     SERVO_MAX_US = 2000;
static constexpr int SERVO_SAFE_TRAVEL_DEG = 60;  // logical 30..150, center 90
static constexpr int SERVO_SETTLE_MS = 500;
static constexpr char AP_SSID[] = "APEX-CONTROL";
static constexpr char AP_PASSWORD[] = "Apex2026Robot";
static constexpr char FIRMWARE_VERSION[] = "5.0.0-rc1";

static_assert(SERVO_MIN_US >= 1000 && SERVO_MAX_US <= 2000 &&
                  SERVO_MIN_US < SERVO_CENTER_US &&
                  SERVO_CENTER_US < SERVO_MAX_US,
              "Keep servo pulse limits inside the validated calibration");
static_assert(SERVO_SAFE_TRAVEL_DEG > 0 && SERVO_SAFE_TRAVEL_DEG <= 90,
              "Invalid commanded servo travel");
