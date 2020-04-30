// Library includes
#include <LiquidCrystal.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// GPS Module Definitions
const int GPS_BAUD = 9600;
const int GPS_RX = 10;
const int GPS_TX = 11;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX, GPS_TX);

bool gps_locked;
bool locking;

// Encoder definitions
#define ENC_CLK A0
#define ENC_DT A1
#define ENC_SW A2

// Set up ClickEncoder
ClickEncoder *encoder;
int16_t enc_last, enc_curr;

// LCD definitions
#define LCD_RS 3
#define LCD_E 4
#define LCD_D4 5
#define LCD_D5 6
#define LCD_D6 7
#define LCD_D7 8

// Set up LCD screen
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Timer service for ClickEncoder button functionality
void timerIsr()
{
    // Check encoder for button updates
    encoder->service();
}

// Enum to hold possible
// input state types
enum Direction
{
    NONE,
    CW,
    CCW
};

byte CW_CHAR[] = { 
    B00000,
    B01110,
    B10001,
    B00101,
    B01001,
    B11110,
    B01000,
    B00100
};
byte CCW_CHAR[] = {
    B00000,
    B01110,
    B10001,
    B10100,
    B10010,
    B01111,
    B00010,
    B00100
};
byte CLK_CHAR[] = {
    B10001,
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B01110,
    B10001
};
byte LOCKING_CHAR[] = {
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B01110,
    B00000,
    B00000
};
byte LOCKED_CHAR[] = {
    B00000,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110,
    B00000,
    B00000
};

Direction currDir;

ClickEncoder::Button btn;
ClickEncoder::Button_e btnStatus;

// ------------
// Functions for converting RA/DEC to Alt/Az
// ------------

// Gets the number of days since J2000 to the beginning
// of the year
float DaysFromJ2000(uint16_t year)
{
    switch (year)
    {
    case 2019:
        return 6938.5;
        break;
    case 2020:
        return 7303.5;
        break;
    case 2021:
        return 7669.5;
    default:
        break;
    }
}

// Gets the number of days since the start of the month
// I'm so sorry for this code
int DaysToMonth(int month)
{
    switch (month)
    {
    case 1:
        return 0;
        break;
    case 2:
        return 31;
        break;
    case 3:
        return 59;
        break;
    case 4:
        return 90;
        break;
    case 5:
        return 120;
        break;
    case 6:
        return 151;
        break;
    case 7:
        return 181;
        break;
    case 8:
        return 212;
        break;
    case 9:
        return 243;
        break;
    case 10:
        return 273;
        break;
    case 11:
        return 304;
        break;
    case 12:
        return 334;
        break;
    default:
        break;
    }
}

float TimeToDec(uint8_t hour, uint8_t min, uint8_t sec)
{
    return hour + (min / 60) + (sec / 3600);
}

float LST(TinyGPSTime t, TinyGPSDate d, float lng)
{
    float time_dec = TimeToDec(t.hour(), t.minute(), t.second());

    float days_since_j2000 = DaysFromJ2000(d.year()) + DaysToMonth(d.month()) + d.day();

    float days = (time_dec / 24) + days_since_j2000;

    float lst = (100.46 + 0.985647 * days + lng + (15 * time_dec)) % 360;

    if(lst < 0)
    {
        lst += 360;
    }

    return lst;
}



void setup()
{
    // Begin serial connection to GPS
    ss.begin(GPS_BAUD);
    Serial.begin(115200);

    // Setup the LCD screen
    lcd.begin(16, 2);
    lcd.createChar(1, CW_CHAR);
    lcd.createChar(2, CCW_CHAR);
    lcd.createChar(3, CLK_CHAR);
    lcd.createChar(4, LOCKING_CHAR);
    lcd.createChar(5, LOCKED_CHAR);

    // Setup the encoder
    encoder = new ClickEncoder(ENC_DT, ENC_CLK, ENC_SW);
    // Disable the acceleration feature in the encoder
    // becase we only care about left/right/click
    encoder->setAccelerationEnabled(false);
    enc_last = -1;

    // Setup the timer to handle encoder clicks
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);

    // Display startup messages
    lcd.setCursor(0, 0);
    lcd.print("GPS TEST ");
    lcd.print(F("v."));
    lcd.print(TinyGPSPlus::libraryVersion());
    delay(3000);
    lcd.clear();
}


void loop()
{
    if(millis() > 5000 && gps.charsProcessed() < 10)
    {
        lcd.setCursor(0, 0);
        lcd.print("CHK GPS WIRING");
    }
    if(gps_locked)
    {
        if(!gps.location.isValid())
        {
            gps_locked = false;
            locking = true;
        }
        displayCoords();
    }
    else 
    {
        if(gps.location.isValid())
        {
            gps_locked = true;
            locking = false;
        }
        else if(!locking)
        {
            locking = true;
        }
    }
    
    if(gps_locked)
    {
        lcd.setCursor(15, 0);
        lcd.write(5);
    }
    else if(locking)
    {
        lcd.setCursor(15, 0);
        lcd.write(4);
    }

    // Get current values from encoder
    btn = encoder->getButton();
    readEncoder();

    displayDirection();
    displayBtnStatus();
    updateTime();
    smartDelay(10);
}

void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (ss.available())
            gps.encode(ss.read());
    } while (millis() - start < ms);
    
}

void displayCoords()
{
    lcd.setCursor(2,1);
    char sz[13];
    sprintf(sz, "%.4f %.4f", gps.location.lat(), gps.location.lng());
    lcd.print(sz);
}

// Displays the current direction in the top right
// of the LCD for testing purposes
void displayDirection()
{
    lcd.setCursor(0, 1);
    switch(currDir)
    {
        case Direction::CW:
            lcd.write(1);
            break;
        case Direction::CCW:
            lcd.write(2);
            break;
        case Direction::NONE:
            lcd.print("   ");
            break;
    }
}

// Updates the time in the top left of the screen
void updateTime()
{
    if(gps_locked || locking)
    {
        TinyGPSTime t = gps.time;

        // Reset LCD Cursor and show time
        // in HH:MM:SS format
        char sz[10];
        sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
        Serial.println(sz);
        lcd.setCursor(0, 0);
        lcd.print(sz);
    }
}

void displayBtnStatus()
{
    lcd.setCursor(1,1);
    switch(btn)
    {
        case ClickEncoder::Open:
            lcd.write(" ");
            break;
        case ClickEncoder::Clicked:
            lcd.write(3);
            delay(500);
            lcd.setCursor(1,1);
            lcd.print(" ");
            break;
        case ClickEncoder::Closed:
        case ClickEncoder::Pressed:
        case ClickEncoder::Held:
        case ClickEncoder::Released:
        case ClickEncoder::DoubleClicked:
            break;
    }
}

// Sets currDir based off of the
// read values from the encoder
void readEncoder()
{
    enc_curr += encoder->getValue();

    if(enc_curr == enc_last)
    {
        currDir = Direction::NONE;
    }
    else if(enc_curr > enc_last)
    {
        enc_last = enc_curr;
        currDir = Direction::CW;
        delay(150);
    }
    else
    {
        enc_last = enc_curr;
        currDir = Direction::CCW;
        delay(150);
    }
}