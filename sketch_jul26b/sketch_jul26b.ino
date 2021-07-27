#include <config.h>
#include <ds3231.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

struct ts t;
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
File myFile;
char myFileName[20];
int lastMinute;//Last minute for LCD refresh
int today;//Day of week for opening correct schedule file

char buf[20];//SD card r/w buffer and incrementer
int idx = 0;

int nexth;
int nextm; //hour, minute and next event name
char next[20];

int currenth;
int currentm; //hour, minute and current event name
char current[20];

bool alarm;
int blinktimer = 0; //alarm utils
int blinkperiod = 100;

char line1[20]; //LCD display registers
char line2[20];

void emptybuf()
{
  memset(buf, '\0', sizeof buf);
  idx = 0;
}

void readFile() //reads next event onto current and next variables 
{
  sprintf(myFileName, "%02d/%02d/%d.txt", t.mday, t.mon, t.year); //overrides
  myFile = SD.open(myFileName);
  if (!myFile) {
    sprintf(myFileName, "%d.txt", t.wday); //overrides
    myFile = SD.open(myFileName);
    if (!myFile) {
      lcd.clear();
      lcd.print("No file or SD");
      lcd.setCursor(0, 1);
      lcd.print("Please reset :)");
      while (true);
    }
  }
  emptybuf();
  while (myFile.available())
  {
    char c = myFile.read();
    if (c == '\n' || idx == 17)
    {
      buf[strlen(buf) - 1] = '\0';
      currenth = nexth;
      currentm = nextm;
      strcpy(current, next);
      sscanf(buf, "%d:%d %[^\n]", &nexth, &nextm, next);
      Serial.println(buf);
      emptybuf();
      if (!(t.hour > nexth || (t.hour == nexth && t.min >= nextm)))
      {
        /*
        Serial.print("Current: ");
        Serial.println(current);
        Serial.print("Next : ");
        Serial.println(next);*/
        return;
      }
    }
    else
    {
      buf[idx] = c;
      idx++;
    }
  }
  myFile.close();
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  Wire.begin();
  pinMode(8, INPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  /*
    DS3231_init(DS3231_CONTROL_INTCN);
    t.hour=18;  
    t.min=1;
    t.sec=50;
    t.mday=26;
    t.mon=7;
    t.wday=1;
    t.year=2021;
    DS3231_set(t);
  */
  lcd.print("SD booting");
  if (!SD.begin(10)) {
    lcd.clear();
    lcd.print("SD boot error");
    while (true);
  }
  DS3231_get(&t);
  readFile();
  alarm = false;
}

void loop() {
  DS3231_get(&t);//get time
  if (today != t.wday){  //New day
    //Serial.println("today calibrated");
    readFile();
    today = t.wday;
    sprintf(line2, "%10s %02d:%02d", current, nexth, nextm);
  }
  if (t.hour > nexth || (t.hour == nexth && t.min >= nextm))  //Next event
  {
    //Serial.println("new event");
    readFile();
    sprintf(line2, "%10s %02d:%02d", current, nexth, nextm);
    alarm = true;
  }
  if (alarm) //Alarm flicker
  {
    blinktimer++;
    if (blinktimer >= blinkperiod)
    {
      blinktimer = 0;
      digitalWrite(9, !digitalRead(9));
    } 
  }
  if (lastMinute != t.min){ //LCD refresh
    lastMinute = t.min;
    if (t.wday == 1){
      sprintf(line1, " %02d/%02d MON %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }else if (t.wday == 2){
      sprintf(line1, " %02d/%02d TUE %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }else if (t.wday == 3){
      sprintf(line1, " %02d/%02d WED %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }else if (t.wday == 4){
      sprintf(line1, " %02d/%02d THU %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }else if (t.wday == 5){
      sprintf(line1, " %02d/%02d FRI %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }else if (t.wday == 6){
      sprintf(line1, " %02d/%02d SAT %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }else if (t.wday == 7){
      sprintf(line1, " %02d/%02d SUN %02d:%02d", t.mday, t.mon, t.hour, t.min);
    }
    lcd.clear();
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
  //Serial.print(digitalRead(8)); //LCD refresh
  if (digitalRead(8) == HIGH){
    alarm = false;
    digitalWrite(9, HIGH);
  }
}
