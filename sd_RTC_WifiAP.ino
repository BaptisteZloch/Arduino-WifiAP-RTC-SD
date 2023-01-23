#include "SPI.h"
#include "SD.h"
#include <Wire.h>
#include <RTClib.h>//Lib
#include <ESP8266WiFi.h>

const int chipSelect = D8;  // used for ESP8266

const char* ssid     = "ESP8266-AP";//access point's name

RTC_DS3231 rtc;//real time clock declaration
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

File root;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  Wire.begin();
  Serial.println("");   Serial.println("");   Serial.println("");
  delay(1000);
  Serial.println("__________________________Setup starting__________________________");

  // Initialize the wifi
  WiFi.softAPdisconnect();//disconnect all devices previously connected
  Serial.println("Setting soft-AP ... ");
  if (WiFi.softAP(ssid))//create Access point on esp8266
  {
    Serial.println("Ready");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());//print the IP that we have to connect in our browser
  }
  else
  {
    Serial.println("Failed!");
  }

  //Initialize the SD sur SPI
  Serial.println("SD Card init...");
  if ( !SD.begin(chipSelect) ) {
    Serial.println("SD - Initialization failed!");
    while (1);
  } else {
    Serial.println("SD - Initialization OK");
  }
  // list des fichiers de la SD
  root = SD.open("/");
  printDirectory(root, 0);

  if (!rtc.begin()) {//starting RTC module
    Serial.println("Couldn't find RTC");
    while (1);//stop point
  }


  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(2020, 10, 11, 21, 38, 00));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }



  // lire le contenu du fichier
  readSdFile("datalog3.txt");


  Serial.println("");
  Serial.println("__________________________Setup done__________________________");

}

void loop() {

  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(2000);

}


void printDirectory(File dir, int numTabs) {
  Serial.println("");
  Serial.println("__________________________SD list file__________________________");
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();

  }
  Serial.println("__________________________done!");
}

void readSdFile(String sdFileName) {

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(sdFileName);

  // if the file is available, write to it:
  Serial.println("");
  Serial.println("__________________________" + sdFileName + "__________________________");
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }

  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening " + sdFileName);
  }
  Serial.println("_____fin__________________" + sdFileName + "__________________________");
  Serial.println("");
}
