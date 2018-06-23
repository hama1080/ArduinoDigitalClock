#include <MsTimer2.h>

//D3,D11でpwmが使えない(Timer2を使うため)
//シリアル通信を行うと0,1ピンが上手く動作しない。
const int HOUR10 = 5;
const int HOUR1 = 6;
const int MINUTE10 = 9;
const int MINUTE1 = 10;
const int COLON = 13;

const int anodeA = 0;
const int anodeB = 1;
const int anodeC = 2;
const int anodeD = 12;
const int anodeE = 4;
const int anodeF = 7;
const int anodeG = 8;

const int SW0 = 15;
const int SW1 = 16;
const int SW2 = 17;

const int digits[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00100111, // 7
  0b01111111, // 8
  0b01101111, // 9
  0b00000000, //none
};

int dinamic = 0;
int settingTmp = 0;

int hour = 0;
int minute = 0;
int second = 0;

boolean displayOn = true;

//数字表示関数
void displayNumber(int n){
  digitalWrite(anodeA, digits[n] & 0b00000001 ? HIGH : LOW);
  digitalWrite(anodeB, digits[n] & 0b00000010 ? HIGH : LOW);
  digitalWrite(anodeC, digits[n] & 0b00000100 ? HIGH : LOW);
  digitalWrite(anodeD, digits[n] & 0b00001000 ? HIGH : LOW);
  digitalWrite(anodeE, digits[n] & 0b00010000 ? HIGH : LOW);
  digitalWrite(anodeF, digits[n] & 0b00100000 ? HIGH : LOW);
  digitalWrite(anodeG, digits[n] & 0b01000000 ? HIGH : LOW);
}

void setup()
{
  pinMode(HOUR10, OUTPUT);
  pinMode(HOUR1, OUTPUT);
  pinMode(MINUTE10, OUTPUT);
  pinMode(MINUTE1, OUTPUT);
  pinMode(COLON,OUTPUT);
  
  pinMode(anodeA, OUTPUT);
  pinMode(anodeB, OUTPUT);
  pinMode(anodeC, OUTPUT);
  pinMode(anodeD, OUTPUT);
  pinMode(anodeE, OUTPUT);
  pinMode(anodeF, OUTPUT);
  pinMode(anodeG, OUTPUT);
  
  pinMode(COLON, OUTPUT);
  
  pinMode(SW0, INPUT);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);

  MsTimer2::set(1000, timeUpdate); // 1000ms period
  MsTimer2::start();
}

void timeUpdate()
{
  static boolean output = HIGH;
  
  digitalWrite(COLON,output);
  if(displayOn)
    output = !output;
  else
    output = LOW;
  
  second++;
  if(second == 60){
    second = 0;
    minute++;
  }
  if(minute == 60){
    minute = 0;
    hour++;
  }
  if(hour == 24){
    hour = 0;
  }
}



void wait(){
  int cnt;
  for(cnt=0; cnt < 5; cnt++){
    clockDisplay();
  }
}

void setting(){
  settingTmp = 1; //1:minute, 2:hour
  while(1)
  {
    second = 0;
    clockDisplay();
    
    if(digitalRead(SW1)==LOW){
      if(settingTmp == 1){
        minute++;
        if(minute >= 60)
          minute = 0;
        wait();
      }else if(settingTmp == 2){
        hour++;
        if(hour >= 24)
          hour = 0;
        wait();
      }
    }
    
    if(digitalRead(SW2)==LOW){
      if(settingTmp == 1){
        minute--;
        if(minute < 0)
          minute = 59;
        wait();
      }else if(settingTmp == 2){
        hour--;
        if(hour < 0)
          hour = 24;
        wait();
      }
    }
  
    if(digitalRead(SW0)==LOW){
      if(settingTmp == 1){
        delay(500);
        settingTmp = 2;
      }else if(settingTmp == 2){
        delay(500);
        settingTmp = 0;
        break;
      }
    }
  }
}

void clockDisplay()
{
   switch(dinamic){
    case 0 :{

      //カソードの処理
      digitalWrite(HOUR10,HIGH);
      //アノードの処理
      displayNumber(hour/10);
      
      if(settingTmp == 1){
        delay(1);
      }else if(settingTmp == 2){
        delay(40);
      }
      delay(3);
      displayNumber(10);
      digitalWrite(HOUR10,LOW);
      break;
    }
    case 1 :{

      //カソードの処理
      digitalWrite(HOUR1,HIGH);
      //アノードの処理
      displayNumber(hour%10);
      
      if(settingTmp == 1){
        delay(1);
      }else if(settingTmp == 2){
        delay(40);
      }
      delay(3);
      displayNumber(10);
      digitalWrite(HOUR1,LOW);
      break;
    }
    case 2 :{
      //カソードの処理
      //analogWrite(MINUTE10, brightness);
      digitalWrite(MINUTE10,HIGH);
      //アノードの処理
      displayNumber(minute/10);
      
      if(settingTmp == 1){
        delay(40);
      }else if(settingTmp == 2){
        delay(1);
      }
      delay(3);
      displayNumber(10);
      digitalWrite(MINUTE10,LOW);
      break;
    }
    case 3 :{
      //カソードの処理
      digitalWrite(MINUTE1,HIGH);
      //アノードの処理
      displayNumber(minute%10);
      
      if(settingTmp == 1){
        delay(40);
      }else if(settingTmp == 2){
        delay(1);
      }
      delay(3);
      displayNumber(10);
      digitalWrite(MINUTE1,LOW);
      break;
    }
  }
  dinamic++;
  if(dinamic==4)  dinamic = 0;
}

void loop()
{
  if(displayOn){
    clockDisplay();
  }
  
  if(digitalRead(SW0) == LOW){
    delay(200);
    setting();
  }
  
  //明るさ設定
  //22時～5時で部屋が暗ければ消灯
  if((22 <= hour || hour <= 4) && ((100 - analogRead(A0)/10) < 10))
    displayOn = false;
  else
    displayOn = true;
}
