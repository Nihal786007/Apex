// LOCAL SCREENSHOT FIXTURE ONLY. No connection to ESP32 or physical hardware.
const http=require('node:http'),fs=require('node:fs'),path=require('node:path');
const source=fs.readFileSync(path.join(__dirname,'../firmware/APEX_Ultimate_V5/Dashboard.h'),'utf8');
let page=source.match(/R"APEX\(([\s\S]*?)\)APEX"/)[1];
page=page.replace('<p>ROBOTICS CONTROL CENTER</p>','<p>UI PREVIEW · SIMULATED TELEMETRY · NO ROBOT CONNECTED</p>');
let state={version:'5.0.0-rc1',epoch:1,mode:0,phase:'STOPPED',armed:false,running:false,bench:true,status:'PREVIEW: simulated telemetry; no hardware connected',forward:'',valid:true,cm:68.4,angle:90,pulse:1500,power:0,left:0,right:0,outputLeft:0,outputRight:0,irL:1,irR:1,irStable:true,blackL:1,blackR:1,calibrated:true,whiteSaved:true,blackSaved:true,text:'APEX ENGINEERING DEMO',oled:true,minAngle:30,maxAngle:150,correction:65,stopCm:25,clearCm:30,whiteCenter:0,recoveryMs:900,turnMs:300};
const server=http.createServer(async(req,res)=>{
 const url=new URL(req.url,'http://127.0.0.1');let body='';for await(const chunk of req)body+=chunk;
 const q=new URLSearchParams(body);res.setHeader('Cache-Control','no-store');
 const json=(data,code=200)=>{res.writeHead(code,{'Content-Type':'application/json'});res.end(JSON.stringify(data));};
 if(url.pathname==='/'){res.writeHead(200,{'Content-Type':'text/html;charset=utf-8'});res.end(page);return;}
 if(url.pathname==='/claim'){state.epoch++;state.armed=false;state.running=false;state.power=0;state.phase='STOPPED';json({ok:true,token:'0000000000000044',epoch:state.epoch});return;}
 if(url.pathname==='/status'){json(state);return;}
 if(url.pathname==='/cmd'){
  const kind=q.get('kind'),v=q.get('value');
  if(['stop','pause','disarm','mode','pan'].includes(kind)){state.armed=false;state.running=false;state.phase='STOPPED';state.left=state.right=0;state.epoch++;}
  if(kind==='mode')state.mode=+v;
  if(kind==='arm'){state.armed=true;state.epoch++;}
  if(kind==='power')state.power=+v;
  if(kind==='pan'){state.angle=+v;state.pulse=Math.round(1500-(+v-90)*500/90);}
  if(kind==='text')state.text=v.slice(0,40);
  if(kind==='start'){
   if(!state.armed){json({ok:false,epoch:state.epoch,message:'Arm first'},409);return;}
   state.running=true;state.epoch++;
   state.phase=state.mode===1?'LINE TRACKING':'SCANNING LEFT';
   state.left=state.mode===1?85:0;state.right=state.mode===1?150:0;
   state.irL=1;state.irR=state.mode===1?0:1;
   if(state.mode===2){state.angle=30;state.pulse=1833;state.cm=42.6;state.forward='Center the front sensor';}
  }
  if(kind==='drive'){state.phase='MANUAL COMMAND';state.left=state.right=state.power;}
  if(kind==='release'){state.left=state.right=0;state.phase='STOPPED';}
  state.status='PREVIEW: '+(state.running?(state.mode===1?'simulated left correction':'simulated stopped scan'):'simulated controller · no robot connected');
  json({ok:true,epoch:state.epoch,message:state.status});return;
 }
 json({ok:false,message:'Not found'},404);
});
const port=Number(process.env.APEX_PREVIEW_PORT||8765);
server.listen(port,'127.0.0.1',()=>console.log(`APEX UI fixture: http://127.0.0.1:${port} (simulated telemetry only)`));
