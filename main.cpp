#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

#include "Servo.h"
#include "objectTracking/ColorTracker.h"
#include "objectTracking/Vec3.h"

std::string servodPath = "/home/frans/Documents/servod/user/servod";

int steerID = 0;
int motorID = 1;
int cameraServoID = 2;

Servo steerServo(steerID, 1000, 2000);
Servo motorServo(motorID, 1000, 2000);
Servo cameraServo(cameraServoID, 700, 2300);

//Camera stuff
Vec3 objectColor;
Vec3 minColor;
Vec3 maxColor;
Vec3 threshold(15, 30, 30);
int frameW = 320;
int frameH = 240;

float cameraFOV = 53.14;

float wheelAngleStart = 0;
float wheelAngleMin = 45;
float wheelAngleMax = 135;

float camAngleStart = 0;
float camAngleMin = 0;
float camAngleMax = 180;

int main()
{
    std::cout << "setting up camera" << std::endl;

    //Setup camera
    raspicam::RaspiCam_Cv camera;
    camera.set(CV_CAP_PROP_FRAME_WIDTH, frameW);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, frameH);

    if(camera.open() == false)
    {
        std::cout << "Error opening acmera" << std::endl;
        return -1;
    }

    //Capture some frames to focus the camera
    for(unsigned int i = 0; i < 10; i++)
    {
        camera.grab();

        cv::Mat result;
        camera.retrieve(result);
        imwrite("/home/frans/public_html/test.jpg", result);
    }

    //setup servod
    std::cout << "Setting up servoblaster" << std::endl;

    char servodStartCmd[256];
    sprintf(servodStartCmd, "sudo %s --p1pins=16,18,22", servodPath.data());
    std::cout << servodStartCmd << std::endl;
    system(servodStartCmd);
    
    //Main loop
    try
    {
        //Set servos to default position
        steerServo.setVal(0.5);
        cameraServo.setVal(0.5);
        bool running = true;
        while(running == true)
        {
            std::cout << "1: Select an object" << std::endl;
            std::cout << "2: follow object" << std::endl;
            std::cout << "0: Exit" << std::endl;

            //Get input from the user
            std::string input;
            std::cin >> input;
            
            //Selecting an object
            if(input.compare("1") == 0)
            {
                cv::Mat image;
                //Take a picture
                camera.grab();
                camera.retrieve(image);
                
                ColorTracker ct;
                ct.setImage(image);
                ct.saveImage("/home/frans/public_html/cvRefRaw.jpg");

                objectColor = ct.getObjectColor();
                minColor = objectColor - threshold;
                maxColor = objectColor + threshold;
                

                //Printing the result
                std::cout << "Searching for color: " << objectColor.getString() << " minColor: " << minColor.getString() << " max: : " << maxColor.getString() << std::endl;
            }
            if(input.compare("2") == 0)
            {
                bool following = true;
                while(following == true)
                {
                    cv::Mat image;
                    ColorTracker ct;
                    
                    //Grabbing an image
                    camera.grab();
                    camera.retrieve(image);

                    ct.setImage(image);
                    ct.generateBinary(minColor, maxColor, true);

                    Vec2 middlePos = ct.getMiddlePos();

                    system("clear");
                    //printing the middle pos
                    std::cout << "Found object in: " << middlePos.getString() << std::endl;

                    //If the object was found
                    if(middlePos.val[0] >= 0 && middlePos.val[0] <= frameW)
                    {
    
                        //Calculating the angle to the object
                        float angle = cameraFOV * (middlePos.val[0] / frameW);
                        
                        //Calculating what angle to turn the wheels to
                        float objectAngleRel = 90 - (cameraFOV / 2) + angle; //Object angle relative to the servo 0 angle
                        objectAngleRel = objectAngleRel - cameraServo.getAngle() + 90; //Adding the angle of the camera
    
                        steerServo.turnToAngle(objectAngleRel);
                        //cameraServo.turnToAngle(objectAngleRel);

                        //std::cout << "Servo angle: " << steerServo.getAngle() << std::endl;

                        /*
                        float servoFact = objectAngleWheel / (wheelAngleMax - wheelAngleMin);

                        std::cout << servoFact;
    
                        if(servoFact > 1)
                        {
                            servoFact = 1;
                        }
                        if(servoFact < 0)
                        {
                            servoFact = 0;
                        }
    
                        //Setting the servo angle
                        steerServo.setVal(servoFact);*/

                        //Run the motor
                        motorServo.setVal(1);
                    }
                    else
                    {
                        //Object not found, stop driving
                        motorServo.setVal(-1);
                    }
                }
            }
            if(input.compare("0") == 0)
            {
                running = false;
            }
        }
    }
    catch(int e)
    {
        std::cout << "Exception: " << e << std::endl;
    }
    //Stopping the servos
    steerServo.setVal(-1);

    return 0;
}
