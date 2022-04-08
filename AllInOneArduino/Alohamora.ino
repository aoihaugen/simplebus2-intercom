int BUS_RX = 8;
int BUS_TX = 11;
char message[18];
int bits = 0;
const int MY_ADDRESS = 1;
int result[2];
long int lastheard;
bool listening = false;

///////////////////////////////////////////////////////////////////////////////
// Message codes
// 

// Sent from intercoms to call the main switchboard.
#define MSG_CALL_TO_MAIN_SWITCHBOARD         8

// Sent from intercoms to call the caretaker phone (K).
#define MSG_CALL_CARETAKER_DOOR_ENTRY_PHONE  9

// Open building's entry door (AP). Sent when the intercom's open door button 
// is pushed. User ID tells who's opening the door.
#define MSG_OPEN_DOOR                       16

// Sent when the user hooks off the intercom's handset.
#define MSG_HOOK_OFF                        17

// Sent when the user hooks on the intercom's handset.
#define MSG_HOOK_ON                         18

// Send from intercoms to call the secondary switchboard. In model the 
// secondary button can be configured to send this message by setting the JP1
// jumper to position C.
#define MSG_CALL_TO_SECONDARY_SWITCHBOARD   19

// Turns on the video camera and the intercom's screen (AI).
#define MSG_CAMERA_ON                       20

// Sent when the external switch connected to the CFP terminals is closed.
#define MSG_CALL_FROM_FLOOR_DOOR            21

// Sent by one intercom to call other intercomms (INT).
#define MSG_CALL_INTERCOM                   24

// After sending MSG_CALL_INTERCOM three consecutive
// MSG_CALL_INTERCOM_RESPONSE are sent.
#define MSG_CALL_INTERCOM_RESPONSE          26

// Activates a generic actuator. In model the secondary button can be
// configured to send this message by setting the JP1 jumper to position A.
#define MSG_GENERIC_ACTUATOR                29

// High prioritary call to main switchboard (PAN).
#define MSG_HIGH_PRIO_CALL_TO_MAIN_SWITCHBOARD  30

// Sent from switchboard for calling an intercom.
#define MSG_CALL_FROM_SWITCHBOARD_1         32

// After sending this message I got a MSG_HOOK_ON, but only once. What does
// this mean?.
#define MSG_UNKNOWN_1                       33


#define MSG_CALL_FROM_SWITCHBOARD_2             37
#define MSG_CALL_FROM_SWITCHBOARD_3             42
#define MSG_CALL_FROM_SWITCHBOARD_4             43
#define MSG_CALL_FROM_SWITCHBOARD_5_SCREEN_ON   45


// Sent when someone calls at the building's entry. The ring tone is played
// once per each message, this message is usually sent two times, followed by 
// MSG_CALL_FROM_ENTRY_DOOR_SCREEN_ON.
#define MSG_CALL_FROM_ENTRY_DOOR            48

// Sent when the screen is turned off ?????
#define MSG_SCREEN_OFF                      49

// Similar to MSG_RING_TONE but also makes the intercom turn on the video 
// screen.
#define MSG_CALL_FROM_ENTRY_DOOR_SCREEN_ON  50


#define MSG_START_BLINKING_OPEN_DOOR_BTN    51
#define MSG_STOP_BLINKING_OPEN_DOOR_BTN     52

void setup() {
  Serial.begin(9600);
  pinMode(BUS_RX, INPUT);
  pinMode(BUS_TX, OUTPUT);


}

void bus_rx(){
  listening = true;
  while(digitalRead(BUS_RX == HIGH)){
    if(listening && (millis()-lastheard) > 64){
    listening = false;
    bits= 0;
    break;
  }
  }
  long int timer1 = micros();
  while(digitalRead(BUS_RX == LOW)){
    if(listening && (millis()-lastheard) > 64){
    listening = false;
    bits= 0;
    break;
  }
  }
  long int timer2 = micros();
  int time = howLong(timer1,timer2);
  pulse_detected(time);
}

void pulse_detected(int time){
  if(time<4096 && time>2048){
    bit_is_zero();
    lastheard = millis();
  }else if(time<4096 && time>6912){
    bit_is_one();
    lastheard = millis();
  }else if(sizeof(message)==18){
    decode();
  }  
}

void bit_is_zero(){
  if(bits < 18){
    message[bits] = 0;
    bits += 1;
  }else{
    decode();
  }
}
void bit_is_one(){
  if(bits < 18){
    message[bits] = 1;
    bits += 1;
  }else{
    decode();
  }
}

void decode(){
  if(checksum() != true){
    bits = 0;
  }else{
    bintodec();
    processMessage(result[0], result[1]);
    bits = 0;
  }
}

bool checksum(){
  int sum = 0;
  int checksum = 0;
  for(int i = 0;i<14; i++){
    if(message[i]==1){
      sum++;
    }
  }
  for(int i = 14;i<18; i++){
    if(message[i]==1){
      checksum++;
    }
  }
  if(checksum == sum){
    return true;
  }else{
    return false;
  }
}

int howLong(int a, int b){
  return b-a;
}



 void bintodec(){
  int msgAddr[6];
  int msgCode[8];

  int resultAddr = 0;
  int resultCode = 0;
  for(int j = 0; j<14;j++){
    if(j<6){
      msgAddr[j]=message[j];
    }else{
      msgCode[j-6]=message[j];
    }   
  }
  resultAddr = (msgAddr[5] * 32) + (msgAddr[4] * 16) +(msgAddr[3] * 8)+ (msgAddr[2]*4) +(msgAddr[1]*2)+ msgAddr[0];
  resultCode = (msgCode[7] * 128) + (msgCode[6] * 64) + (msgCode[5] * 32) + (msgCode[4] * 16) +(msgCode[3] * 8)+ (msgCode[2]*4) +(msgCode[1]*2)+ msgCode[0];
  int result[2] = {resultAddr, resultCode};
}

void reset(){
  bits=0;
}

void processMessage(int msgCode, int msgAddr) 

{
  Serial.print("RX <- code: ");    
  Serial.print(msgCode);
  Serial.print(" address: ");
  Serial.println(msgAddr);
  #ifdef SERIAL_OUTPUT
  Serial.print("RX <- code: ");    
  Serial.print(msgCode);
  Serial.print(" address: ");
  Serial.println(msgAddr);
  #endif


  if (msgAddr == MY_ADDRESS) {
    switch (msgCode)
    {
    case MSG_CALL_TO_SECONDARY_SWITCHBOARD:
      break;
    case MSG_CALL_FROM_ENTRY_DOOR:
      break;
    case MSG_OPEN_DOOR:
      break;
    }
  }
}

void loop() {
  if(digitalRead(BUS_RX == HIGH)){
    bus_rx();
  }
  if(listening && (millis()-lastheard) > 64){
    listening = false;
    bits= 0;
  }
  
}