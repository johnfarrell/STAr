// Sketch to test functionality of the
// GY-NEO6MV2 GPS Module
//
// Author: John Farrell
// Date: 10/19/2019

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int GPS_BAUD_RATE = 9600;
const int RX = 11;
const int TX = 12;

float curr_lat;
float curr_lng;
bool gps_locked;
bool locking;

TinyGPSPlus gps;
SoftwareSerial ss(RX, TX);


void setup() {
    Serial.begin(115200);
    ss.begin(GPS_BAUD_RATE);

    Serial.println(F("gps_test.ino"));
    Serial.println(F("Test sketch for the GY-NEO6Mv2 Module"));
    Serial.print(F("Using TinyGPS++ v."));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println();
}

void loop() {
  if(millis() > 5000 && gps.charsProcessed() < 10) 
  {
    Serial.println(F("No GPS Data Recieved: check wiring"));
  }
  Serial.println();
  printDateTime(gps.date, gps.time);
  if(gps_locked)
  {
    if(!gps.location.isValid())
    {
      Serial.println(F("SATELLITE LOCK LOST, INITIATING RECONNECTION"));
      gps_locked = false;
      locking = true;
    }
    else
    {
      printLatLong(gps.location);
    }
  }
  else
  {
    if(gps.location.isValid())
    {
      Serial.println(F("\nSATELLITE LOCK ACQUIRED"));
      gps_locked = true;
    }
    else if(!locking)
    {
      Serial.print(F("ACQUIRING SATELLITE LOCK..."));
      locking = true;
    }
  }

  smartDelay(1000);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  char dz[32];
  sprintf(dz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
  Serial.print(dz);

  char sz[32];
  sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
  Serial.print(sz);
}

static void printLatLong(TinyGPSLocation &loc)
{
  Serial.print(F("Lat: "));
  Serial.print(loc.lat(), 6);
  Serial.print(F(" Long: "));
  Serial.println(loc.lng(), 6);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
