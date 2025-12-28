#include <Wire.h>
#include "Adafruit_VL53L1X.h"

// HUZZAH32 pins
const int SDA1 = 23, SCL1 = 22;  // Sensor 1  (Wire)
const int SDA2 = 19, SCL2 = 18;  // Sensor 2  (Wire1)

Adafruit_VL53L1X sensor1;
Adafruit_VL53L1X sensor2;

int gameScoreTeam1 = 0;  //score in game for team 1
int gameScoreTeam2 = 0;

int setScoreTeam1 = 0;
int setScoreTeam2 = 0;

unsigned long timerT1_start = 0;
unsigned long timerT2_start = 0;

const int DISTANCE_THRESHOLD = 20;
const unsigned long TIME_TO_SCORE = 2000;


void setup() {
  Serial.begin(115200);
  delay(200);

  // I2C buses
  Wire.begin(SDA1, SCL1);
  Wire1.begin(SDA2, SCL2);
  Wire.setClock(100000);
  Wire1.setClock(100000);

  // Init sensors on default address 0x29
  if (!sensor1.begin(0x29, &Wire)) {
    Serial.println("Sensor 1 not found (SDA=23 SCL=22)");
    while (1) delay(10);
  }
  if (!sensor2.begin(0x29, &Wire1)) {
    Serial.println("Sensor 2 not found (SDA=19 SCL=18)");
    while (1) delay(10);
  }

  sensor1.startRanging();
  sensor2.startRanging();
  Serial.println("Both sensors ready");
}


void getPoint(int16_t sensor, unsigned long &sensor_timer, int &scoreTeam) {
  if (sensor < DISTANCE_THRESHOLD) {
    if (sensor_timer == 0) {
      sensor_timer = millis();
    }
  } else {
    sensor_timer = 0;
  }
  if (sensor_timer != 0 && millis() - sensor_timer > TIME_TO_SCORE) {
    scoreTeam++;
    displayScore();
    sensor_timer = 0;
  }
}


void displayScore() {
  bool team1_wins = (gameScoreTeam1 == 4 && gameScoreTeam2<3) || (gameScoreTeam1 == 5 && gameScoreTeam2==3);
  bool team2_wins = (gameScoreTeam2 == 4 && gameScoreTeam1<3) || (gameScoreTeam2 == 5 && gameScoreTeam1==3);

  if (team1_wins || team2_wins) { //Set
    Serial.print("Set!");
    if (team1_wins) {
      setScoreTeam1++;
    }else{
      setScoreTeam2++;
    }
    gameScoreTeam1 = 0;
    gameScoreTeam2 = 0;
    Serial.print("Set Score: ");
    Serial.print(setScoreTeam1);
    Serial.print(":");
    Serial.print(setScoreTeam2);
    Serial.println();
    return;
  }

  if (gameScoreTeam1 == 3 && gameScoreTeam2 == 3) { //DEUCE
    Serial.print("DEUCE!");
    return;
  }

  if (gameScoreTeam1 == 4 && gameScoreTeam2 == 4) { //From advatange to DEUCE
    gameScoreTeam1 = 3;
    gameScoreTeam2 = 3;
  }


 if (gameScoreTeam1 == 4 && gameScoreTeam2 == 3) { //advatange to team1;
    Serial.print("Advantage for team 1");
    return;
  }

   if (gameScoreTeam1 == 3 && gameScoreTeam2 == 4) { //advatange to team2;
    Serial.print("Advantage for team 2");
    return;
  }

  //regular score
  Serial.println("Scored! New score:");
  switch (gameScoreTeam1) {
    case 0: Serial.print("0"); break;
    case 1: Serial.print("15"); break;
    case 2: Serial.print("30"); break;
    case 3: Serial.print("40"); break;
  }
  Serial.print(":");
  switch (gameScoreTeam2) {
    case 0: Serial.print("0"); break;
    case 1: Serial.print("15"); break;
    case 2: Serial.print("30"); break;
    case 3: Serial.print("40"); break;
  }
  Serial.println();
}

void loop() {

  int16_t d1 = sensor1.distance();
  int16_t d2 = sensor2.distance();

  getPoint(d1, timerT1_start, gameScoreTeam1);
  getPoint(d2, timerT2_start, gameScoreTeam2);
}
