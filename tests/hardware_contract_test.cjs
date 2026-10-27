const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const cfg=fs.readFileSync(path.join(__dirname,'../firmware/APEX_Ultimate_V5/Config.h'),'utf8');
const pins={ENA:25,IN1:26,IN2:27,IN3:14,IN4:12,ENB:13,SERVO_PIN:18,TRIG:5,ECHO:19,SDA_PIN:21,SCL_PIN:22,IR_LEFT:34,IR_RIGHT:35};
for(const [key,value] of Object.entries(pins))assert.equal(Number(cfg.match(new RegExp(`\\b${key}\\s*=\\s*(\\d+)`))[1]),value,key+' pin changed');
assert.match(cfg,/MOTOR_POWER_VERIFIED\s*=\s*false/,'Distributed build must default to BENCH');
assert.match(cfg,/LEFT_MOTOR_REVERSED\s*=\s*true/);assert.match(cfg,/RIGHT_MOTOR_REVERSED\s*=\s*true/);
for(const [key,value] of Object.entries({SERVO_MIN_US:1000,SERVO_CENTER_US:1500,SERVO_MAX_US:2000}))assert.equal(Number(cfg.match(new RegExp(`\\b${key}\\s*=\\s*(\\d+)`))[1]),value);
assert.match(cfg,/APEX-CONTROL/);assert.match(cfg,/Apex2026Robot/);
console.log('PASS: all 13 required GPIO assignments, motor polarity, known servo pulses, AP credentials and BENCH default.');
