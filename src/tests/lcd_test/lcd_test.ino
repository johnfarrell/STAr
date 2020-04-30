#include <LiquidCrystal.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

ClickEncoder *encoder;
int16_t last, value;

void timerIsr()
{
  encoder->service();
}

bool btnClicked;
bool clockwise;
bool countercw;

void setup() {
  // LCD setup
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.write("Position:");
  lcd.setCursor(10, 1);

  encoder = new ClickEncoder(A1, A0, A2);
  encoder->setAccelerationEnabled(false);
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  last = -1;
}

void loop() {
  ClickEncoder::Button b = encoder->getButton();
  readEncoder();

  if(b != ClickEncoder::Open)
  {
    switch(b)
    {
      case ClickEncoder::Clicked:
        btnClicked=true;
        break;
      case ClickEncoder::Closed:
      case ClickEncoder::Pressed:
      case ClickEncoder::Held:
      case ClickEncoder::Released:
      case ClickEncoder::DoubleClicked:
        break;
    }
  }

  if(btnClicked)
  {
    btnClicked = false;
    lcd.setCursor(0,0);
    lcd.print("Click");
    delay(500);
    lcd.setCursor(0,0);
    lcd.print("     ");
  }

  if(clockwise)
  {
    lcd.setCursor(10, 1);
    lcd.print(" CW");
  }
  else if(countercw)
  {
    lcd.setCursor(10, 1);
    lcd.print("CCW");
  }
  else
  {
    lcd.setCursor(10, 1);
    lcd.print("   ");
  }
  

}

void readEncoder()
{
  value += encoder->getValue();

  if(value == last)
  {
    countercw = false;
    clockwise = false;
  }
  else if(value > last)
  {
    last = value;
    clockwise = true;
    countercw = false;
    delay(150);
  }
  else
  {
    last = value;
    countercw = true;
    clockwise = false;
    delay(150);
  }
}
