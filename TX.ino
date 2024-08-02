#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>

RF24 radio(9, 10); //CE,CSN
SoftwareSerial bluetooth(0, 1); //TX = 0, RX = 1

const uint64_t ad = 0xE9E8F0F0E1LL;
char data;

void setup() {
  pinMode(2, INPUT); //tiến
  pinMode(3, INPUT); //lùi
  pinMode(4, INPUT); //sang phải
  pinMode(5, INPUT); //sang trái
  pinMode(6, INPUT); //thả
  pinMode(7, INPUT); //autofly

  Serial.begin(9600);
  bluetooth.begin(9600);

  radio.begin();
  radio.openWritingPipe(ad);
  radio.stopListening();
}

struct infor{
  int move;
  int high;
  int press;
  int turn;
  int fall;
};

infor in4;

void loop() {
  if (digitalRead(2) == LOW){ //go
    in4.move = 0;
  }
  if (digitalRead(3) == LOW){ //back
    in4.move = 1;
  }
  if (digitalRead(4) == LOW){ //turn right
    in4.move = 2;
  }
  if (digitalRead(5) == LOW){ //turn left
    in4.move = 3;
  }
  in4.high = map(analogRead(A0), 0, 1023, 100, 200);
  if (analogRead(A1) <= 341){ //turn around
    in4.turn = 1;
  }else if (analogRead(A1) >= 682){
    in4.turn = 2;
  }else{
    in4.turn = 0;
  }
  if (digitalRead(6) == LOW){ //throw ball
    in4.fall = 0;
  }else{
    in4.fall = 1;
  }
  if (digitalRead(7) == LOW){ //autofly
    in4.press = 0;
  }else{
    in4.press = 1;
  }

  if (bluetooth.available() > 0){
    data = bluetooth.read();
    if (data == "G"){ //go
      in4.move = 0;
    }
    if (data == "B"){ //back
      in4.move = 1;
    }
    if (data == "R"){ //turn right
      in4.move = 2;
    }
    if (data == "L"){ //turn left
      in4.move = 3;
    }
    if (data == "l"){ //turn around left
      in4.turn = 1;
    }else if (data == "r"){ //turn around right
      in4.turn = 2;
    }else{
      in4.turn = 0;
    }
    if (data == "u"){
      in4.high += 1;
    }else if (data == "d"){
      in4.high -= 1;
    }
  }
  radio.write(&in4, sizeof(infor));
  delay(50);
}