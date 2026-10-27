#pragma once
#include <Arduino.h>
const char DASHBOARD[] PROGMEM=R"APEX(
<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>APEX Ultimate · Robotics Control Center</title>
<style>
:root{color-scheme:dark;--bg:#080f19;--card:#101e2d;--line:#263b50;--muted:#98afc2;--teal:#63dfcb;--danger:#e95b68}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:#edf6ff;font:14px system-ui,-apple-system,sans-serif}
button,input,select{font:inherit}button{min-height:44px;border:1px solid #38556b;border-radius:9px;background:#203a4e;color:#edf6ff;padding:9px 12px;cursor:pointer}
button:hover{border-color:var(--teal)}button:disabled{opacity:.38;cursor:default}button:active{background:#25675e}button:focus-visible,input:focus-visible,select:focus-visible{outline:2px solid var(--teal);outline-offset:3px}
main{max-width:1460px;margin:auto;padding:16px;min-height:100svh;display:grid;grid-template-rows:auto auto auto 1fr auto;gap:10px}
header{display:flex;align-items:center;justify-content:space-between;gap:15px}.brand{display:flex;align-items:center;gap:12px}.mark{display:grid;place-items:center;width:44px;height:44px;border:1px solid #3e8c85;border-radius:12px;color:var(--teal);font-size:25px}
h1{font-size:22px;letter-spacing:2px;margin:0}header p{font-size:11px;letter-spacing:1.4px;color:var(--muted);margin:3px 0}.header-actions{display:flex;align-items:center;gap:8px}.pill{font-size:12px;padding:7px 10px;border:1px solid var(--line);border-radius:20px;color:var(--muted)}
.stop{background:#b62e40;border-color:#d54e5d;font-weight:800;letter-spacing:.5px;min-width:135px}.modes{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}.modes button{min-height:48px;background:#101e2d;text-align:left;font-size:12px;font-weight:700;letter-spacing:.8px}.modes small{display:block;color:var(--muted);font-weight:400;font-size:10px;letter-spacing:0;margin-top:3px}.modes .active{border-color:var(--teal);background:#153b3b;box-shadow:inset 3px 0 var(--teal)}
.banner{border:1px solid #675029;background:#302719;color:#f0d49b;border-radius:9px;padding:9px 12px;font-size:12px}.banner.live{border-color:#28584e;background:#11342c;color:#99e6d4}
.grid{display:grid;grid-template-columns:1.05fr 1fr 1fr;gap:10px;align-content:start}.card{background:var(--card);border:1px solid var(--line);border-radius:14px;padding:14px;min-width:0}h2{margin:0;font-size:13px;letter-spacing:1px}h3{font-size:12px;color:var(--muted);margin:12px 0 7px}.section-title{display:flex;justify-content:space-between;align-items:center;margin-bottom:12px}.small,.muted{font-size:11px;color:var(--muted)}
.drive{display:grid;grid-template-columns:1fr 1fr;gap:8px}.drive button{min-height:62px;font-weight:650;touch-action:none;user-select:none;-webkit-user-select:none}.controls{display:grid;grid-template-columns:repeat(2,1fr);gap:7px;margin-top:9px}.controls button{font-size:12px}.arm{border-color:#3b8a7c;background:#195247}.autostart{background:#195247;border-color:#3b8a7c}.range-label{display:flex;justify-content:space-between;align-items:center;margin-top:15px}.range-label output{color:var(--teal);font-weight:700}
input[type=range]{width:100%;accent-color:var(--teal);margin:11px 0}input[type=text],input[type=number],select{background:#091420;color:#eff8ff;border:1px solid #3b5267;border-radius:7px;padding:9px;width:100%;min-height:40px}label{font-size:12px}.servo-buttons{display:grid;grid-template-columns:repeat(3,1fr);gap:6px}.message{display:flex;gap:6px;margin-top:7px}.message input{min-width:0}.message button{white-space:nowrap;font-size:12px}
.state{margin-top:12px;background:#0a1521;border-radius:9px;border:1px solid #2b4358;padding:10px;min-height:65px}.state b{display:block;color:var(--teal);font-size:12px;letter-spacing:.8px;margin-bottom:5px}#status{font-size:12px;line-height:1.4;overflow-wrap:anywhere}.distance{font-size:46px;font-weight:700;letter-spacing:-2px;color:var(--teal);line-height:1.2}.unit{font-size:13px;color:var(--muted);letter-spacing:0}.readiness{border:1px solid #66522e;background:#302719;color:#f1d398;border-radius:8px;padding:9px;font-size:12px;margin:8px 0}.readiness.clear{border-color:#2a6252;background:#12382d;color:#9fe8d4}
.ir-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:10px}.ir{background:#091420;border:1px solid #294357;border-radius:8px;padding:9px}.ir b{display:block;font-size:16px;margin-top:4px}.data-line{display:flex;justify-content:space-between;gap:8px;font-size:11px;color:var(--muted);margin-top:8px}.data-line span:last-child{color:#dbeafa}.cal-buttons{display:grid;grid-template-columns:1fr 1fr;gap:6px;margin-top:8px}.cal-buttons button{font-size:11px;padding:6px}.oled{background:#081116;border:1px solid #2a4653;border-radius:8px;padding:9px;color:#b8f5df;font:12px monospace;min-height:40px;overflow-wrap:anywhere;margin-top:8px}
details{margin-top:12px;border-top:1px solid var(--line);padding-top:9px}summary{cursor:pointer;color:var(--muted);font-size:12px;min-height:28px}.tune-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}.tune-grid label{font-size:11px}.tune-grid select{font-size:11px}.tune-grid input{padding:5px;min-height:34px}.full{grid-column:1/-1}footer{display:flex;justify-content:space-between;gap:12px;color:var(--muted);font-size:10px}.divider{border-top:1px solid var(--line);margin-top:12px;padding-top:9px}
@media(min-width:900px) and (min-height:650px){main{min-height:100svh}.grid{align-content:center}}
@media(min-width:900px) and (max-width:1100px){main{padding:10px;gap:8px}}
@media(max-width:899px){.grid{grid-template-columns:1fr 1fr}.telemetry{grid-column:1/-1}.ir-grid{max-width:500px}.distance{font-size:36px}main{padding:10px}}
@media(max-width:560px){.grid{grid-template-columns:1fr}.telemetry{grid-column:auto}header{align-items:flex-start;flex-wrap:wrap}.header-actions{width:100%;justify-content:space-between}h1{font-size:20px}.modes button{font-size:10px;padding:8px}.modes small{display:none}.pill{font-size:10px}.drive button{min-height:66px}footer{display:block}}
@media(max-height:650px) and (min-width:900px){main{padding:9px;gap:7px}.card{padding:10px}.drive button{min-height:48px}.range-label{margin-top:9px}.section-title{margin-bottom:8px}.distance{font-size:36px}header p{display:none}.modes button{min-height:44px}.modes small{display:none}footer{display:none}}
</style></head><body><main>
<header><div class="brand"><div class="mark">Λ</div><div><h1>APEX ULTIMATE</h1><p>ROBOTICS CONTROL CENTER</p></div></div><div class="header-actions"><span class="pill" id="connection">Disconnected</span><button id="reconnect">Connect</button><button id="emergency" class="stop">■ E-STOP</button></div></header>
<nav class="modes" aria-label="Robot mode"><button data-mode="0" class="active">01 / MANUAL<small>Held commands · direct control</small></button><button data-mode="1">02 / LINE FOLLOWING<small>Two-sensor calibrated tracking</small></button><button data-mode="2">03 / OBSTACLE AVOIDANCE<small>Stop · scan · choose · explore</small></button></nav>
<div id="powerBanner" class="banner">BENCH MODE — Physical motor outputs disabled. Verify motor ratings, paired current and driver power before enabling.</div>
<div class="grid">
<section class="card"><div class="section-title"><h2>DRIVE &amp; MODE</h2><span class="pill" id="armed">DISARMED</span></div>
<div class="drive"><button data-dir="F">▲ Forward</button><button data-dir="B">▼ Backward</button><button data-dir="L">◀ Turn left</button><button data-dir="R">Turn right ▶</button></div>
<div class="controls"><button id="arm" class="arm">Arm motors</button><button id="disarm">Disarm</button><button id="start" class="autostart">START</button><button id="pause">PAUSE</button></div>
<label class="range-label" for="power">Motor power command <output id="powerValue">0%</output></label><input id="power" type="range" min="0" max="255" value="0"><div class="small">PWM percentage, not measured speed or current protection.</div>
<div class="state" role="status"><b id="phase">STOPPED</b><div id="status">Connect to the local controller.</div></div>
<div class="data-line"><span>Requested L / R</span><span id="requested">0 / 0</span></div><div class="data-line"><span>Applied PWM L / R</span><span id="applied">0 / 0</span></div>
<details><summary>Mode tuning · pause before applying</summary><div class="tune-grid"><label>Correction (PWM)<input id="correction" type="number" min="0" max="180" value="65"></label><label>Obstacle stop (cm)<input id="stopCm" type="number" min="15" max="100" value="25"></label><label class="full">Centered line pattern<select id="whiteCenter"><option value="0">Both BLACK · stripe covers both sensors</option><option value="1">Both WHITE · straddle narrow line; limited evidence</option></select></label><label>Recovery (ms)<input id="recoveryMs" type="number" min="300" max="1500" value="900"></label><label>Pivot (ms)<input id="turnMs" type="number" min="100" max="600" value="300"></label><button id="tune" class="full">Apply tuning &amp; stop</button></div></details>
</section>
<section class="card"><div class="section-title"><h2>SERVO &amp; DISPLAY</h2><span class="small">Commanded coordinates</span></div>
<div class="servo-buttons"><button id="servoLeft">Left</button><button id="servoCenter">Center</button><button id="servoRight">Right</button></div><label class="range-label" for="angle">Servo command <output id="angleValue">90°</output></label><input id="angle" type="range" min="30" max="150" value="90" step="1"><div class="data-line"><span>Pulse / feedback</span><span id="pulse">1500 µs · no position feedback</span></div>
<h3>OLED MESSAGE · 40 ASCII CHARACTERS</h3><div class="message"><input id="text" type="text" maxlength="40" value="APEX READY" aria-label="OLED message"><button id="sendText">Send</button></div><div id="oledText" class="oled">APEX READY</div><div id="oledStatus" class="small" style="margin-top:7px">OLED status pending</div>
<div class="divider"><h2>IR CALIBRATION</h2><div class="small" style="margin-top:7px">Stop. Place both sensors over a broad white patch, capture WHITE; repeat over black.</div><div class="cal-buttons"><button id="white">Capture WHITE</button><button id="black">Capture BLACK</button></div><div id="calibration" class="readiness">Not calibrated — line START locked</div><div id="polarity" class="small">Configured black level: L 1 / R 1</div></div>
</section>
<section class="card telemetry"><div class="section-title"><h2>LIVE TELEMETRY</h2><span class="small" id="version">V5</span></div><div class="distance"><span id="distance">—</span> <span class="unit">cm</span></div><div id="forward" class="readiness">Waiting for sensor data</div><div class="small" id="releaseThreshold">Stop ≤25 cm · release ≥30 cm</div>
<div class="ir-grid"><div class="ir"><span class="small">LEFT IR</span><b id="irLeft">—</b></div><div class="ir"><span class="small">RIGHT IR</span><b id="irRight">—</b></div></div><div class="data-line"><span>IR signal</span><span id="irStable">Waiting</span></div>
<div class="divider"><h3>OPERATING RULES</h3><div class="small" style="line-height:1.7">Mode changes stop and disarm.<br>Autonomy requires Arm, then START.<br>Lost line, echo or controller lease stops motion.<br>No blind reversing or arbitrary maze-solving claim.</div></div>
</section></div>
<footer><span>APEX-CONTROL · 192.168.4.1 · Local control</span><span>Manual hold 450 ms · Lease 1.8 s · Run 30 s · Continuous command 8 s</span></footer>
</main><script>
const $=id=>document.getElementById(id);
let token='',epoch=0,seq=0,connected=false,busy=false,aborter=null,held=null;
let pending=[],stopPending=false,lastPoll=0,lastHold=0,lastLease=0,settingsLoaded=false;
let state={mode:0,armed:false,running:false,bench:true,minAngle:30,maxAngle:150};
function enabled(){
  const blocked=!connected||stopPending;
  document.querySelectorAll('[data-dir]').forEach(b=>b.disabled=blocked||!state.armed||state.mode!==0);
  ['arm','disarm','power','sendText','white','black','tune'].forEach(id=>$(id).disabled=blocked);
  $('start').disabled=blocked||!state.armed||state.mode===0||state.running;
  $('pause').disabled=blocked||!state.running;
  ['angle','servoLeft','servoCenter','servoRight'].forEach(id=>$(id).disabled=blocked||state.running);
  document.querySelectorAll('[data-mode]').forEach(b=>b.disabled=blocked);
}
function queue(kind,value='',extra={}){
  if(!connected)return;
  if(kind==='power')pending=pending.filter(q=>q.kind!=='power');
  pending.push({kind,value,epoch,...extra});
}
function stopNow(kind='stop'){
  held=null;pending=[];state.armed=false;state.running=false;stopPending=true;enabled();
  if(aborter)aborter.abort();
  if(token)pending.unshift({kind,value:'',epoch:0});
  else stopPending=false;
  $('status').textContent='Stopping…';
}
function beaconStop(){
  held=null;pending=[];state.armed=false;state.running=false;enabled();
  if(token)navigator.sendBeacon('/cmd',new URLSearchParams({token,seq:++seq,epoch:0,kind:'stop',value:''}));
}
async function http(path,data){
  const controller=new AbortController();aborter=controller;
  const timeout=setTimeout(()=>controller.abort(),1200);
  try{
    const response=await fetch(path,data?{method:'POST',body:new URLSearchParams(data),cache:'no-store',signal:controller.signal}:{cache:'no-store',signal:controller.signal});
    let body;try{body=await response.json();}catch{throw Error('Invalid controller response');}
    if(response.status===403)throw Error('Controller session changed: reconnect');
    if(!response.ok && response.status!==409)throw Error(body.message||'Request failed');
    return body;
  }finally{clearTimeout(timeout);if(aborter===controller)aborter=null;}
}
function lost(error){
  beaconStop();connected=false;stopPending=false;pending=[];enabled();
  $('connection').textContent='Disconnected';$('status').textContent=error.message+' · reconnect deliberately';
}
async function connect(){
  if(busy)return;
  busy=true;beaconStop();pending=[];connected=false;enabled();
  try{
    const reply=await http('/claim',{});token=reply.token;epoch=reply.epoch;seq=0;
    connected=true;stopPending=false;held=null;state.armed=false;state.running=false;settingsLoaded=false;
    $('power').value=0;$('powerValue').textContent='0%';lastPoll=0;lastLease=0;
  }catch(e){lost(e);}finally{busy=false;enabled();}
}
function render(s){
  state=s;epoch=s.epoch;if(!s.armed||s.mode!==0)held=null;
  if(!settingsLoaded){for(const id of ['correction','stopCm','whiteCenter','recoveryMs','turnMs'])$(id).value=s[id];settingsLoaded=true;}
  $('connection').textContent='Connected';$('armed').textContent=s.armed?(s.bench?'ARMED · BENCH':'ARMED'):'DISARMED';
  $('powerBanner').classList.toggle('live',!s.bench);
  $('powerBanner').textContent=s.bench?'BENCH MODE — Physical motor outputs disabled. Live telemetry and state machines are available for logic tests.':'MOTOR OUTPUT ENABLED — Physical supply checks must be completed. Keep cutoff accessible.';
  $('phase').textContent=s.phase;$('status').textContent=s.status;
  document.querySelectorAll('[data-mode]').forEach(b=>b.classList.toggle('active',+b.dataset.mode===s.mode));
  $('distance').textContent=s.valid?s.cm.toFixed(1):'—';$('forward').textContent=s.forward||'Forward ready';$('forward').classList.toggle('clear',!s.forward);
  $('releaseThreshold').textContent=`Stop ≤${s.stopCm} cm · release ≥${s.clearCm} cm`;
  $('angleValue').textContent=s.angle+'°';$('pulse').textContent=s.pulse+' µs · no position feedback';
  if(document.activeElement!==$('angle'))$('angle').value=s.angle;
  $('angle').min=s.minAngle;$('angle').max=s.maxAngle;
  if(document.activeElement!==$('power')){$('power').value=s.power;$('powerValue').textContent=Math.round(s.power/255*100)+'%';}
  $('irLeft').textContent=s.irL+' · '+(s.irL===s.blackL?'BLACK':'WHITE');$('irRight').textContent=s.irR+' · '+(s.irR===s.blackR?'BLACK':'WHITE');
  $('irStable').textContent=s.irStable?'Stable / debounced':'Changing';
  $('calibration').textContent=s.calibrated?'IR calibrated · line START available':`Calibration needed · WHITE ${s.whiteSaved?'saved':'missing'} / BLACK ${s.blackSaved?'saved':'missing'}`;
  $('calibration').classList.toggle('clear',s.calibrated);$('polarity').textContent=`Black level: L ${s.blackL} / R ${s.blackR}`;
  $('requested').textContent=s.left+' / '+s.right;$('applied').textContent=s.outputLeft+' / '+s.outputRight;
  $('oledText').textContent=s.text;$('oledStatus').textContent=s.oled?'OLED online · live distance always retained':'OLED not detected · control remains available';
  $('version').textContent=s.version;
  enabled();
}
async function pump(){
  if(busy||!connected||document.hidden)return;
  const now=Date.now();let action=null;
  if(pending.length)action=pending.shift();
  else if(held && now-lastHold>=150){action={kind:'drive',value:held.dir,epoch};lastHold=now;}
  else if(now-lastLease>=500){action={kind:'lease',value:'',epoch};lastLease=now;}
  const pollDue=!action&&now-lastPoll>=300;
  if(!action&&!pollDue)return;
  busy=true;
  try{
    if(action){
      const r=await http('/cmd',{token,seq:++seq,...action});
      if(Number.isInteger(r.epoch))epoch=r.epoch;
      if(['stop','pause','disarm'].includes(action.kind))stopPending=false;
      if(!r.ok){held=null;pending=[];state.armed=false;state.running=false;$('status').textContent=r.message;lastPoll=0;}
      else if(action.kind!=='lease'){lastPoll=0;$('status').textContent=r.message;}
    }else{lastPoll=now;render(await http('/status?token='+encodeURIComponent(token)));}
  }catch(e){if(!(e.name==='AbortError'&&stopPending))lost(e);}
  finally{busy=false;enabled();}
}
function releaseHeld(){
  if(!held)return;
  held=null;pending=pending.filter(q=>q.kind!=='drive');pending.unshift({kind:'release',value:'',epoch});
}
document.querySelectorAll('[data-dir]').forEach(b=>{
  b.addEventListener('pointerdown',e=>{
    if(held||!connected||!state.armed||state.mode!==0||stopPending)return;
    e.preventDefault();b.setPointerCapture(e.pointerId);held={dir:b.dataset.dir,id:e.pointerId};lastHold=0;
  });
  for(const event of ['pointerup','pointercancel','lostpointercapture'])b.addEventListener(event,e=>{if(held&&held.id===e.pointerId)releaseHeld();});
  b.addEventListener('contextmenu',e=>e.preventDefault());
});
document.querySelectorAll('[data-mode]').forEach(b=>b.onclick=()=>{held=null;pending=[];queue('mode',b.dataset.mode);state.armed=false;state.running=false;enabled();});
$('reconnect').onclick=connect;$('emergency').onclick=()=>stopNow('stop');
$('arm').onclick=()=>queue('arm');$('start').onclick=()=>queue('start');
$('pause').onclick=()=>stopNow('pause');$('disarm').onclick=()=>stopNow('disarm');
$('power').oninput=()=>{$('powerValue').textContent=Math.round($('power').value/255*100)+'%';};
$('power').onchange=()=>queue('power',$('power').value);
function setAngle(angle){held=null;pending=[];queue('pan',angle);}
$('servoLeft').onclick=()=>setAngle(state.minAngle);$('servoCenter').onclick=()=>setAngle(90);$('servoRight').onclick=()=>setAngle(state.maxAngle);
$('angle').oninput=()=>{$('angleValue').textContent=$('angle').value+'°';};$('angle').onchange=()=>setAngle(+$('angle').value);
$('sendText').onclick=()=>queue('text',$('text').value);$('white').onclick=()=>queue('white');$('black').onclick=()=>queue('black');
$('tune').onclick=()=>queue('tune','',{correction:$('correction').value,stopCm:$('stopCm').value,whiteCenter:$('whiteCenter').value,recoveryMs:$('recoveryMs').value,turnMs:$('turnMs').value});
window.addEventListener('blur',()=>{if(connected)stopNow();});
document.addEventListener('visibilitychange',()=>{if(document.hidden)beaconStop();});
window.addEventListener('pagehide',beaconStop);
setInterval(pump,50);enabled();connect();
</script></body></html>
)APEX";
