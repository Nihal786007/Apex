#include <assert.h>
#include <stdio.h>
#include "../firmware/APEX_Ultimate_V5/ControlCore.h"

static ApexCore c;
static ApexInput input;
static uint32_t now,sequence;
static unsigned checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line %d: %s | %s phase=%d L=%d R=%d\n",__LINE__,#x,c.status,c.phase,c.left,c.right);assert(x);}}while(0)
static ApexAck command(ApexCommandType type,int a){
  ApexCommand q;memset(&q,0,sizeof(q));q.type=type;q.a=a;q.session=44;q.seq=++sequence;q.epoch=c.epoch;
  return apex_command(&c,&q,now);
}
static void sample(uint32_t dt,float cm,int valid,int left,int right,int keepLease){
  now+=dt;input.at=now;input.sampleId++;input.rangePan=c.cfg.panMounted?c.pan:90;
  input.rangeValid=valid;input.cm=cm;input.irLeft=left;input.irRight=right;input.irStable=1;
  if(keepLease)command(LEASE,0);
  apex_tick(&c,&input,now);
}
static void init(void){
  apex_defaults(&c);memset(&input,0,sizeof(input));now=1000;sequence=0;
  ApexCommand claim;memset(&claim,0,sizeof(claim));claim.type=CLAIM;claim.session=44;
  CHECK(apex_command(&c,&claim,now).ok);
  sample(0,80,1,1,1,0);CHECK(command(SET_POWER,150).ok);
}
static void autonomous(ApexMode mode){
  init();CHECK(command(SET_MODE,mode).ok);c.calibrated=1;
  CHECK(command(ARM,0).ok);CHECK(command(START,0).ok);
}
static void scanTick(float l,float f,float r){
  float cm=c.phase==SCAN_LEFT?l:c.phase==SCAN_RIGHT?r:f;
  sample(100,cm,1,1,1,1);
}
static void completeScan(float l,float f,float r){
  int i;for(i=0;i<45 && c.running && c.phase!=PRE_TURN;i++)scanTick(l,f,r);
}
int main(void){
  init();CHECK(!c.armed && !c.left && !c.right && c.power==150);
  CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'F').ok);sample(5,80,1,1,1,0);
  CHECK(c.left==150 && c.right==150);
  CHECK(command(SET_MODE,LINE_FOLLOW).ok);CHECK(!c.armed && !c.running && !c.left && !c.right);

  init();CHECK(command(ARM,0).ok);uint32_t oldEpoch=c.epoch;
  CHECK(command(STOP,0).ok);
  ApexCommand late;memset(&late,0,sizeof(late));late.session=44;late.seq=++sequence;late.epoch=oldEpoch;late.type=ARM;
  CHECK(!apex_command(&c,&late,now).ok);CHECK(!c.armed);
  CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'B').ok);
  oldEpoch=c.epoch;CHECK(command(RELEASE,0).ok);
  late.seq=++sequence;late.epoch=oldEpoch;late.type=DRIVE;late.a='B';
  CHECK(!apex_command(&c,&late,now).ok);CHECK(!c.left && !c.right);

  init();CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'B').ok);
  sample(451,80,1,1,1,1);CHECK(!c.armed && strstr(c.status,"hold timeout"));
  init();CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'B').ok);sample(5,80,1,1,1,0);
  CHECK(c.left<0 && c.right<0);CHECK(command(RELEASE,0).ok);sample(7000,80,1,1,1,1);
  CHECK(command(DRIVE,'B').ok);sample(5,80,1,1,1,0);CHECK(c.armed && c.left<0);

  init();sample(0,20,1,1,1,0);CHECK(command(ARM,0).ok);
  CHECK(!command(DRIVE,'F').ok);CHECK(!c.armed && strstr(c.status,"Obstacle"));
  sample(10,28,1,1,1,0);CHECK(c.obstacle);sample(10,31,1,1,1,0);CHECK(!c.obstacle && !c.armed);
  init();CHECK(command(SET_PAN,30).ok);CHECK(command(ARM,0).ok);
  CHECK(!command(DRIVE,'F').ok);CHECK(strstr(c.status,"Center"));

  init();CHECK(command(SET_MODE,LINE_FOLLOW).ok);CHECK(command(ARM,0).ok);
  CHECK(!command(START,0).ok);CHECK(!c.running);
  sample(10,80,1,0,0,0);CHECK(command(CAL_WHITE,0).ok);
  CHECK(command(CAL_BLACK,0).ok);CHECK(!c.calibrated);
  sample(50,80,1,1,1,0);CHECK(command(CAL_BLACK,0).ok);CHECK(c.calibrated);
  CHECK(command(ARM,0).ok);CHECK(command(START,0).ok);
  sample(600,80,1,1,1,1);CHECK(c.running && c.left==150 && c.right==150);
  sample(10,80,1,1,0,1);CHECK(c.left<c.right && c.phase==LINE_TRACK);
  sample(10,80,1,0,1,1);CHECK(c.right<c.left);
  sample(10,80,1,0,0,1);CHECK(c.phase==LINE_RECOVER && c.left==0);
  sample(150,80,1,0,0,1);CHECK(c.left>0 && c.right<0);
  sample(50,80,1,1,0,1);CHECK(c.phase==LINE_TRACK && c.running);
  sample(10,80,1,0,0,1);sample(901,80,1,0,0,1);CHECK(!c.running && !c.armed);

  autonomous(LINE_FOLLOW);sample(10,80,1,0,0,1);CHECK(!c.running);
  autonomous(LINE_FOLLOW);c.cfg.whiteCenter=1;
  sample(500,80,1,0,0,1);CHECK(c.running);
  sample(501,80,1,0,0,1);CHECK(!c.running && strstr(c.status,"evidence"));
  autonomous(LINE_FOLLOW);c.cfg.whiteCenter=1;sample(10,80,1,1,1,1);CHECK(!c.running && strstr(c.status,"ambiguous"));
  autonomous(LINE_FOLLOW);c.cfg.blackLeft=c.cfg.blackRight=0;
  sample(10,80,1,0,0,1);CHECK(c.left==150 && c.right==150);
  sample(10,0,0,0,0,1);CHECK(!c.running && !c.left && !c.right);

  autonomous(LINE_FOLLOW);input.irStable=0;now+=10;input.at=now;input.sampleId++;
  apex_tick(&c,&input,now);CHECK(c.running && !c.left && !c.right);
  now+=251;input.at=now;input.sampleId++;command(LEASE,0);apex_tick(&c,&input,now);
  CHECK(!c.running && strstr(c.status,"unstable"));

  autonomous(OBSTACLE_AVOID);sample(10,80,1,1,1,1);CHECK(c.left>0 && c.right>0);
  sample(10,20,1,1,1,1);CHECK(c.phase==SCAN_LEFT && c.pan==30 && !c.left && !c.right);
  sample(100,70,1,1,1,1);CHECK(c.scanCount==0 && !c.left);
  completeScan(70,20,100);CHECK(c.phase==PRE_TURN && c.chosenTurn==1 && c.pan==90);
  int i;for(i=0;i<10 && c.phase==PRE_TURN;i++)sample(100,20,1,1,1,1);
  CHECK(c.phase==TURN_RIGHT);sample(50,20,1,1,1,1);CHECK(c.left>0 && c.right<0);
  sample(300,20,1,1,1,1);CHECK(c.phase==RECENTER && !c.left && !c.right);
  sample(100,80,1,1,1,1);sample(10,80,1,1,1,1);CHECK(c.phase==OA_CRUISE && c.left>0);

  autonomous(OBSTACLE_AVOID);sample(10,20,1,1,1,1);completeScan(10,20,10);
  CHECK(!c.running && !c.left && !c.right && strstr(c.status,"Trapped"));
  autonomous(OBSTACLE_AVOID);sample(10,20,1,1,1,1);
  sample(500,10,1,1,1,1);sample(100,100,1,1,1,1);sample(100,20,1,1,1,1);
  CHECK(!c.running && strstr(c.status,"inconsistent"));
  autonomous(OBSTACLE_AVOID);sample(10,20,1,1,1,1);sample(1401,0,0,1,1,1);
  CHECK(!c.running && strstr(c.status,"timeout"));

  autonomous(OBSTACLE_AVOID);c.phase=CHOOSE_PATH;c.scanDistances[0]=10;c.scanDistances[1]=20;c.scanDistances[2]=80;
  c.oscillations=2;c.priorTurn=-1;sample(10,80,1,1,1,1);CHECK(!c.running && strstr(c.status,"alternating"));
  autonomous(OBSTACLE_AVOID);c.recoveries=3;sample(10,20,1,1,1,1);CHECK(!c.running && strstr(c.status,"limit"));

  autonomous(LINE_FOLLOW);sample(1801,80,1,1,1,0);CHECK(!c.running && strstr(c.status,"lease"));
  autonomous(LINE_FOLLOW);c.cfg.motorOnMs=60000;sample(30001,80,1,1,1,1);CHECK(!c.running && strstr(c.status,"run time"));
  autonomous(LINE_FOLLOW);sample(10,80,1,1,1,1);sample(8001,80,1,1,1,1);CHECK(!c.running && strstr(c.status,"motor-command"));
  autonomous(LINE_FOLLOW);oldEpoch=c.epoch;sample(10,0,0,1,1,1);
  late.type=START;late.epoch=oldEpoch;late.seq=++sequence;
  CHECK(!apex_command(&c,&late,now).ok);CHECK(!c.running);

  init();c.cfg.panMounted=0;CHECK(command(SET_PAN,30).ok);sample(100,80,1,1,1,0);
  CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'F').ok);
  CHECK(command(SET_MODE,OBSTACLE_AVOID).ok);CHECK(command(ARM,0).ok);CHECK(!command(START,0).ok);

  init();now=UINT32_MAX-200;c.leaseAt=now;c.servoAt=now-1000;
  sample(0,80,1,1,1,0);CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'B').ok);
  sample(450,80,1,1,1,0);CHECK(c.armed);sample(1,80,1,1,1,0);CHECK(!c.armed);

  init();CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'B').ok);sample(5,80,1,1,1,0);
  ApexCommand owner;memset(&owner,0,sizeof(owner));owner.type=CLAIM;owner.session=99;
  CHECK(apex_command(&c,&owner,now).ok);CHECK(!c.armed && !c.left && !c.right && c.session==99);
  CHECK(!command(ARM,0).ok);CHECK(!c.armed);

  init();late.type=SET_POWER;late.session=44;late.seq=c.seq;late.epoch=c.epoch;late.a=255;
  CHECK(!apex_command(&c,&late,now).ok);CHECK(c.power==150);
  CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'B').ok);sample(5,80,1,1,1,0);
  late.type=SET_MODE;late.a=LINE_FOLLOW;late.seq=++sequence;late.epoch=c.epoch-1;
  CHECK(!apex_command(&c,&late,now).ok);CHECK(!c.armed && !c.left && !c.right);

  init();CHECK(command(ARM,0).ok);CHECK(command(DRIVE,'F').ok);
  now+=351;command(LEASE,0);apex_tick(&c,&input,now);
  CHECK(!c.armed && strstr(c.status,"fresh"));

  init();sample(0,80,1,1,0,0);CHECK(command(CAL_WHITE,0).ok);
  sample(50,80,1,0,1,0);CHECK(command(CAL_BLACK,0).ok);
  CHECK(c.calibrated && c.cfg.blackLeft==0 && c.cfg.blackRight==1);

  autonomous(OBSTACLE_AVOID);c.phase=CHOOSE_PATH;
  c.scanDistances[0]=80;c.scanDistances[1]=20;c.scanDistances[2]=45;
  sample(10,80,1,1,1,1);CHECK(c.chosenTurn==-1 && c.phase==PRE_TURN);
  for(i=0;i<10 && c.phase==PRE_TURN;i++)sample(100,20,1,1,1,1);
  CHECK(c.phase==TURN_LEFT);sample(50,20,1,1,1,1);CHECK(c.left<0 && c.right>0);

  for(i=0;i<3;i++){
    if(i)autonomous((ApexMode)i);else{init();command(ARM,0);command(DRIVE,'B');sample(5,80,1,1,1,0);}
    ApexCommand emergency;memset(&emergency,0,sizeof(emergency));emergency.type=STOP;emergency.session=44;
    CHECK(apex_command(&c,&emergency,now).ok);CHECK(!c.armed && !c.running && !c.left && !c.right);
  }

  printf("PASS: %u assertions against the exact portable controller core.\n",checks);
  return 0;
}
