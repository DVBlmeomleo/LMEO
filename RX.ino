#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <Servo.h>

Servo e1;
Servo e2;
Servo e3;
Servo e4;
Servo drop;

RF24 radio(9, 10); //CE,CSN

Adafruit_MPU6050 mpusus;

const uint64_t ad = 0xE9E8F0F0E1LL;

int es1 = 1000, es2 = 1000, es3 = 1000, es4 = 1000;

struct infor{
  int move;
  int high;
  int press;
  int turn;
  int fall;
};

infor in4;

unsigned long time, timempu, wait;

void nosignal(){
  es1 = in4.high;
  es2 = es1;
  es3 = es1;
  es4 = es1;
}

void fly(){
  if (es1 > 2000){
    es1 = 2000;
  }else if(es1 < 1000){
    es1 = 1000;
  }
  if (es2 > 2000){
    es2 = 2000;
  }else if(es1 < 1000){
    es2 = 1000;
  }
  if (es3 > 2000){
    es3 = 2000;
  }else if(es1 < 1000){
    es3 = 1000;
  }
  if (es4 > 2000){
    es4 = 2000;
  }else if(es1 < 1000){
    es4 = 1000;
  }
  if (es3 == 1000){
    es3 = 0;
  }
  e1.writeMicroseconds(es1);
  e2.writeMicroseconds(es2);
  e3.writeMicroseconds(es3);
  e4.writeMicroseconds(es4);
}

void move(int x){
  if (x == 0){
    es1 = in4.high - 10;
    es2 = in4.high - 10;
    es3 = in4.high + 10;
    es4 = in4.high + 10;
  }
  if (x == 1){
    es1 = in4.high + 10;
    es2 = in4.high + 10;
    es3 = in4.high - 10;
    es4 = in4.high - 10;
  }
  if (x == 2){
    es1 = in4.high + 10;
    es2 = in4.high - 10;
    es3 = in4.high - 10;
    es4 = in4.high + 10;
  }
  if (x == 3){
    es1 = in4.high - 10;
    es2 = in4.high + 10;
    es3 = in4.high + 10;
    es4 = in4.high - 10;
  }
}

void turn(int x){
  if (x == 1){
    es1 += 5;
    es2 -= 5;
    es3 += 5;
    es4 -= 5;
  }
  if (x == 2){
    es1 -= 5;
    es2 += 5;
    es3 -= 5;
    es4 += 5;
  }
}

int memoryx = 0, memoryy = 0, temp;

void mpu(){
  sensors_event_t a, g, t;
  mpusus.getEvent(&a, &g, &t); //get acceleration, rotation, tempurature
  int x, y;
  x = g.gyro.x;
  y = g.gyro.y;
  if (x - memoryx >= 10){
    es1 -= 10;
    es2 += 10;
    es3 += 10;
    es4 -= 10;
  }else if (memoryx - x >= 10){
    es1 += 10;
    es2 -= 10;
    es3 -= 10;
    es4 += 10;
  }
  if (y - memoryy >= 10){
    es1 += 10;
    es2 += 10;
    es3 -= 10;
    es4 -= 10;
  }else if (memoryy - y >= 10){
    es1 -= 10;
    es2 -= 10;
    es3 += 10;
    es4 += 10;
  }
  memoryx = x;
  memoryy = y;
  if (a.acceleration.z >= 2.5){
    es1 -= 10;
    es2 -= 10;
    es3 -= 10;
    es4 -= 10;
  }else if(a.acceleration.z <= -2.5){
    es1 += 10;
    es2 += 10;
    es3 += 10;
    es4 += 10;
  }
  temp = t.temperature;
}

void gps(){ //tính sau
}

void autofly(){} // tính sau

void setup() {
  mpusus.begin();
  mpusus.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpusus.setGyroRange(MPU6050_RANGE_500_DEG);
  mpusus.setFilterBandwidth(MPU6050_BAND_21_HZ);
  timempu = millis();

  e1.attach(2);
  e2.attach(3);
  e3.attach(4);
  e4.attach(5);
  drop.attach(6);

  radio.begin();
  radio.openReadingPipe(1, ad);
  radio.startListening(); //start the radio comunication for receiver
  delay(2000);
}

void loop() {
  while ( radio.available() ) {
    radio.read(&in4, sizeof(infor));
    wait = millis();   // receive the data | 
  }
  if (millis() - wait >= 2000){
    nosignal();
  }else{
    if (in4.press == 0){ //no auto fly
      es1 = in4.high * 10;
      es2 = in4.high * 10;
      es3 = in4.high * 10;
      es4 = in4.high * 10;
      if (in4.turn != 0){
        turn(in4.turn);
      }else{
        move(in4.move);
      }
    }else{ 
      autofly();
    }
    if (in4.fall == 0){
      drop.write(180);
    }else{
      drop.write(0);
    }
  }
  if (millis() - timempu >= 1000){
    timempu = millis();
    mpu();
  }
  fly();
}