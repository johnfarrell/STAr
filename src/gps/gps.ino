/* gps.ino
 *
 * Author: John Farrell
 * Contact: johnjfarrell20@gmail.com
 * 
 * Manages a GY-NEO6Mv2 GPS Module for use in a parallel
 * situation with another arduino.
 * Responds to I2C messages of the form
 * 
 *  [0,1,2]/[Right ascension]/[Declination]
 * 
 * The purpose of this program is to offload the conversion between
 * celestial coordinates of stellar objects to altitude/azimuth coordinates
 * to a secondary arduino, allowing the main arduino to handle motor
 * movements and user I/O.
 * 
 * It does so by using the current time and location provided by
 * the gps module, along with a requested RA/Dec coordinate and convert
 * it to absolute Alt/Az coordinates so a dual stepper motor system
 * can make the movements.
 * 
 */

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>

const int GPS_BAUD = 9600;
const int GPS_RX = 4;
const int GPS_TX = 3;

bool gps_locked;
bool locking;

float curr_lat;
float curr_lng;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX, GPS_TX);

void setup() {
    Serial.begin(115200);
    ss.begin(GPS_BAUD);

    Serial.println(F("gps.ino"));
    Serial.print(F("Using TinyGPS++ v."));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println();

    Wire.begin(0);
    Wire.onReceive(recieveEvent);
}

void loop() {
    while(ss.available())
    {
        Serial.write(ss.read());
    }
}