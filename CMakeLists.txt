####################################
cmake_minimum_required (VERSION 2.8) 
project (raspicam_test)
set(raspicam_DIR "~/Documents/cpp/raspicam/raspicam-0.1.1/build")
find_package(raspicam REQUIRED)
find_package(OpenCV)
IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")

set(SOURCES 
    main.cpp 
    Servo.h
    Servo.cpp
    objectTracking/Vec2.cpp 
    objectTracking/Vec3.cpp 
    objectTracking/ColorTracker.cpp 
    objectTracking/Vec2.h 
    objectTracking/Vec3.h 
    objectTracking/ColorTracker.h 
    objectTracking/ImgFunc.h
    objectTracking/ImgFunc.cpp 
    objectTracking/BinMap.h
    objectTracking/BinMap.cpp
    )

#add_executable (camerabot main.cpp)  
add_executable (camerabot ${SOURCES})
target_link_libraries (camerabot ${raspicam_CV_LIBS})
ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()
#####################################
