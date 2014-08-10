#include "Servo.h"

std::string servodWrite = "/dev/servoblaster";

Servo::Servo(int servoID, float minPW, float maxPW)
{
    mServoID = servoID;
    mMinPW = minPW;
    mMaxPW = maxPW;

    mVal = -1;
    mMaxAngle = 180;
    mLowPW = 500;
    mHighPW = 2500;
}

void Servo::setVal(float val)
{
    if(val != -1 && val < 0 && val > 1)
    {
        std::cout << "Failed to set servo value = " << val << ", value must be -1 or > 0 and < 1" << std::endl;
        
        return;
    }

    mVal = val;

    run();
}
void Servo::run()
{
    float pulseWidth = 0;
    //if the servo should stop running
    if(mVal != -1)
    {
        //Calculating the pulse width
        pulseWidth = mMinPW + (mMaxPW - mMinPW) * mVal;
    }

    //Sending the command to servod
    char writeCmd[256];
    sprintf(writeCmd, "echo %i=%fus > %s", mServoID, pulseWidth, servodWrite.data());

    std::cout << "Servo cmd: " << writeCmd << std::endl;

    system(writeCmd);
}

void Servo::turnToAngle(float angle)
{
    float angleFact = angle / mMaxAngle;
    
    float pw = mLowPW + (mHighPW - mLowPW) * angleFact;


    //Keeping the angle within the limits of the servo
    if(pw > mMaxPW)
    {
        pw = mMaxPW;
    }
    if(pw < mMinPW)
    {
        pw = mMinPW;
    }

    //Calculating the mVal variable since that's what run() uses
    mVal = (pw - mMinPW) / (mMaxPW - mMinPW);

    std::cout << "Servo angle fact " << angleFact << " servoAngle: " << angle << " mVal: " << mVal << " pw: " << pw << std::endl;

    run();
}

float Servo::getAngle()
{
    float cPWM = mLowPW + mVal * (mMaxPW -mMinPW);
    
    float angle = mMaxAngle * (cPWM / mMaxPW);

    return angle;
}
