#include <SoftwareSerial.h>
#include <AccelStepper.h>

#define AZ_DIR 5
#define AZ_STEP 4
#define IFACE_TYPE 1

#define ALT_DIR 7
#define ALT_STEP 6

AccelStepper Az_Step = AccelStepper(IFACE_TYPE, AZ_STEP, AZ_DIR);
AccelStepper Alt_Step = AccelStepper(IFACE_TYPE, ALT_STEP, ALT_DIR);

long count = 0;
int dir = 1;

#define AZ_FULL_ROTATION 24000
#define ALT_FULL_ROTATION 3200

void setup()
{
    Az_Step.setCurrentPosition(0);
    Az_Step.setMaxSpeed(3200);
    Az_Step.setAcceleration(1000);

    Alt_Step.setCurrentPosition(0);
    Alt_Step.setMaxSpeed(1600);
    Alt_Step.setAcceleration(500);

    Az_Step.moveTo(AZ_FULL_ROTATION);
    Alt_Step.moveTo(ALT_FULL_ROTATION);
}

void loop()
{
    if(Az_Step.distanceToGo() == 0)
    {
        Az_Step.moveTo(-1 * Az_Step.currentPosition());
    }
    if(Alt_Step.distanceToGo() == 0)
    {
        Alt_Step.moveTo(-1 * Alt_Step.currentPosition());
    }
    
    Az_Step.run();
    Alt_Step.run();
    // if(count >= 100000)
    // {
    //     dir *= -1;
    //     count = 0;
    // }
    // stepper.setSpeed(dir * 200 * 16);
    

    // stepper.runSpeed();

    

    // count += 1;
    // delay(1000);

    // stepper.setSpeed(-1 * 200 * 16);
    // stepper.runSpeed();
    // delay(1000);

    // stepper.stop();

    
    // // digitalWrite(AZ_DIR, HIGH);
    // Serial.print(stepper.currentPosition() + "\n");

    // for (int i = 0; i < 3 * STEPS_P_REV; i++)
    // {
    //     digitalWrite(AZ_STEP, HIGH);
    //     delayMicroseconds(1000);
    //     digitalWrite(AZ_STEP, LOW);
    //     delayMicroseconds(1000);
    // }

    // delay(1000);

    // digitalWrite(AZ_DIR, LOW);

    // for (int i = 0; i < 3 * STEPS_P_REV; i++)
    // {
    //     digitalWrite(AZ_STEP, HIGH);
    //     delayMicroseconds(1000);
    //     digitalWrite(AZ_STEP, LOW);
    //     delayMicroseconds(1000);
    // }
    
    // delay(1000);
}