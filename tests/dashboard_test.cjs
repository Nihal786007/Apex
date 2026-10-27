const fs=require('node:fs'),path=require('node:path'),vm=require('node:vm'),assert=require('node:assert/strict');
const html=fs.readFileSync(path.join(__dirname,'../firmware/APEX_Ultimate_V5/Dashboard.h'),'utf8');
const script=html.match(/<script>([\s\S]*?)<\/script>/)[1].replace(/setInterval\(pump,50\);enabled\(\);connect\(\);\s*$/,'');
const nodes=new Map();
function element(id){if(!nodes.has(id))nodes.set(id,{value:'0',textContent:'',disabled:false,dataset:{},listeners:{},classList:{toggle(k,v){this[k]=v;}},addEventListener(k,fn){this.listeners[k]=fn;},setPointerCapture(){}});return nodes.get(id);}
const modeButtons=[0,1,2].map(n=>{const b=element('mode'+n);b.dataset.mode=String(n);return b;});
const driveButtons=['F','B','L','R'].map(n=>{const b=element('drive'+n);b.dataset.dir=n;return b;});
let now=1000,response={ok:true,epoch:10,message:'OK'},fail=false;
const calls=[],beacons=[];
const document={hidden:false,activeElement:null,getElementById:element,querySelectorAll(q){return q==='[data-mode]'?modeButtons:q==='[data-dir]'?driveButtons:[];},addEventListener(){}};
const context=vm.createContext({document,window:{addEventListener(){}},navigator:{sendBeacon(url,body){beacons.push({url,body});}},URLSearchParams,AbortController,Date:{now:()=>now},setTimeout(){return 1;},clearTimeout(){},setInterval(){},fetch:async(url,opts)=>{calls.push({url,opts});if(fail)throw Error('Network lost');return {ok:response.ok,status:response.ok?200:409,json:async()=>response};}});
vm.runInContext(script,context);
const run=s=>vm.runInContext(s,context);
const base={epoch:10,mode:0,armed:true,running:false,bench:true,minAngle:30,maxAngle:150,phase:'STOPPED',status:'BENCH PREVIEW',valid:true,cm:65,angle:90,pulse:1500,power:0,irL:1,irR:0,blackL:1,blackR:1,irStable:true,calibrated:true,whiteSaved:true,blackSaved:true,left:90,right:140,outputLeft:0,outputRight:0,text:'APEX READY',oled:true,stopCm:25,clearCm:30,correction:65,whiteCenter:0,recoveryMs:900,turnMs:300,version:'5.0.0-rc1',forward:''};
context.fixture=base;
(async()=>{
  run("token='test';connected=true;render(fixture);");
  assert.equal(element('requested').textContent,'90 / 140');assert.equal(element('applied').textContent,'0 / 0');
  assert.equal(element('forward').textContent,'Forward ready');
  assert.match(element('powerBanner').textContent,/BENCH/);

  run("held={dir:'F',id:1};pending=[{kind:'drive',value:'F',epoch}];");
  modeButtons[1].onclick();
  assert.equal(run('held'),null);assert.equal(run('pending.length'),1);assert.equal(run('pending[0].kind'),'mode');
  response={ok:true,epoch:11,message:'Mode changed'};await run('pump()');
  assert.equal(calls.at(-1).opts.body.get('kind'),'mode');assert.equal(calls.at(-1).opts.body.get('epoch'),'10');

  run("state.armed=true;held={dir:'F',id:1};queue('start');queue('pan',30);stopNow();");
  assert.equal(run('pending.length'),1);assert.equal(run('pending[0].kind'),'stop');
  assert.equal(run('state.armed'),false);assert.equal(run('stopPending'),true);
  response={ok:true,epoch:20,message:'Stopped'};await run('pump()');
  assert.equal(calls.at(-1).opts.body.get('kind'),'stop');assert.equal(calls.at(-1).opts.body.get('epoch'),'0');
  assert.equal(run('stopPending'),false);

  run("epoch=20;queue('arm');epoch=21;");
  response={ok:false,epoch:22,message:'State changed: repeat deliberately'};await run('pump()');
  assert.equal(calls.at(-1).opts.body.get('epoch'),'20');assert.equal(run('state.armed'),false);
  assert.equal(run('pending.length'),0);assert.match(element('status').textContent,/State changed/);

  run("connected=true;state.armed=true;state.mode=0;held={dir:'F',id:1};lastHold=0;");
  const before=calls.length;response={ok:false,epoch:23,message:'No fresh valid forward echo'};await run('pump()');
  assert.equal(calls.length,before+1);assert.equal(calls.at(-1).opts.body.get('kind'),'drive');
  assert.equal(run('held'),null);assert.equal(run('pending.length'),0);
  assert.equal(element('status').textContent,'No fresh valid forward echo');

  document.activeElement=element('power');element('power').value='190';run('render(fixture)');
  assert.equal(element('power').value,'190','Do not overwrite an active slider drag');document.activeElement=null;

  run("connected=true;held={dir:'F',id:1};queue('power',100);");fail=true;await run('pump()');
  assert.equal(run('connected'),false);assert.equal(run('held'),null);assert.equal(run('pending.length'),0);
  assert.equal(beacons.at(-1).body.get('kind'),'stop');
  assert.equal(element('driveF').disabled,true);
  console.log('PASS: dashboard request ordering, epoch capture, STOP priority, bench display, fault retention and connection-loss checks.');
})().catch(e=>{console.error(e);process.exitCode=1;});
