//--------------------------------------------------------------------------------
// レイアウト用発煙装置スケッチ
// [smokePattern.ino]
// Copyright (c) 2020 Ayanosuke(Maison de DCC)
// https://desktopstation.net/bb/index.php
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//--------------------------------------------------------------------------------
#include <arduino.h>

#define pwmPin 10

unsigned long PreviosTimeA = 0;  // PreviosTime 前時間
unsigned long PreviosTimeBtn = 0;  // PreviosTime 前時間

void setup() {
  pinMode(pwmPin, OUTPUT);   // 出力に設定

  // モード指定
  TCCR1A = 0b00100001;
  TCCR1B = 0b00010010; // 001:1分周 　010: 8分周　011:64分周 100:256分周 101:1024分周

  // TOP値指定
  OCR1A = 9;
  // Duty比指定
  OCR1B=0;
  PreviosTimeA = PreviosTimeBtn = millis();
}

void loop() {
  static char mod = 0;
  
  if( (millis() - PreviosTimeA ) >= 10 ){  // 1000:1000msec 前回から1000ms経過したかチェック
    switch(mod){
        case 0:smoke0();break;
        case 1:smoke1();break;
        case 2:smoke2();break;
        default:break;
    }
    PreviosTimeA = millis();                 // 現在の時刻に更新
  }

  if( (millis() - PreviosTimeBtn ) >= 200 ){  // 1000:1000msec 前回から1000ms経過したかチェック
    if(digitalRead( 12 ) == LOW){
      mod++;
      if( mod == 3)
        mod = 0;
    }
    PreviosTimeBtn = millis();                 // 現在の時刻に更新
  }
}

// 煙の停止 
void smoke0(){
  OCR1B = 0;
}

// 煙の強弱
void smoke1(){
  static char state = 0;
  static char tim = 0;
  static char val = 0;

  switch(state){
    case 0: // 初期OFF
            tim = 0;
            val = 0;
            OCR1B = val;
            state = 1;
            break;
    case 1: // じわじわ強く
            tim ++;
            if( tim > 10 ){            
              val ++;
              tim = 0;
              OCR1B = val;
              if( val > 4 ){
                tim = 0;
                state = 2;
              }
            }
            break;
    case 2: // じわじわ弱く
            tim ++;
            if( tim > 10 ){            
              val --;
              tim = 0;
              OCR1B = val;
              if( val < 1 ){
                val = 0;
                tim = 0;
                state = 1;
              }
            }
            break;
    default:
            break;
  } 
}

// 蒸気機関車をイメージ
void smoke2(){
  static char state = 0;
  static int tim = 0;
  static int spd = 0;
  static int lop = 0;
  static char val = 0;  

  switch(state){
    case 0: // 初期OFF
            tim = 0;
            val = 1;
            OCR1B = val;
            state = 1;
            break;
    case 1: // 始動後5sec経過後、弱々しい煙発生
            tim ++;
            if( tim > 500 ){
              val = 3;
              OCR1B = val;
              tim = 0;           
              state = 2;
            }
            break;            
    case 2: // 弱々しい煙5sec保持
            tim ++;
            if( tim > 500 ){
              tim = 0;           
              state = 3;
            }
            break;  
    case 3: // 50msec後、煙を最大発生　低速走行をイメージ
            tim ++;
            if( tim > 5 ){
              OCR1B = 4;
              tim = 0;           
              state = 4;
            }
            break;      
    case 4: // 2sec後煙を弱めて、5回繰り返す　中速走行をイメージ
            tim ++;
            if( tim > 200 ){
              OCR1B = 1;
              tim = 0;
              lop++;           
              state = 3;
              if(lop>5){
                lop = 0;
                state = 5;
              }
            }
            break;
    case 5: // 30msec保持後、煙を最大発生　高速走行をイメージ
            tim ++;
            if( tim > 3){
              OCR1B = 4;
              tim = 0;           
              state = 6;
            }
            break;      
    case 6: // 500msec後煙を弱めて、5回繰り返す
            tim ++;
            if( tim > 50 ){
              OCR1B = 1;
              tim = 0;           
              lop++;
              state = 5;
              if(lop>5){
                lop = 0;
                state = 7;
              }
            }            
            break;  
    case 7: // 10msec保持後、煙を最大発生
            tim ++;
            if( tim > 1){
              OCR1B = 4;
              tim = 0;           
              state = 8;
            }
            break;      
    case 8: // 100msec後煙を弱めて、無限に回繰り返す
            tim ++;
            if( tim > 10 ){
              OCR1B = 1;
              tim = 0;           
              state = 7;
            }            
            break;  
    default:
            break;
  }
}
