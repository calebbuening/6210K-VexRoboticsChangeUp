#include "vex.h"
#include "miscmethods.h"
#include "auton-externs.h"

void oneGoal(int dir){
  // Set Gyro
<<<<<<< HEAD
  sInertial.setRotation(-135 * dir, deg); // Everything is going to be relative to our starting position for the sake of my sanity

  // Deploy intake
=======
  sInertial.setRotation(0, deg); // Everything is going to be relative to our starting position for the sake of my sanity
  if(dir == LEFT) dir = 1;
  else dir = -1;
  if(isBlue == true) signature sig = sigBlue;
  else signature sig = sigRed;
  // Deploy intakes
>>>>>>> 940bcc508659f4a7983aa5ab27d7370bcdf566ad
  intakeIn();

  // Strafe until Green
  signature sig;
  int leftX;
  int leftY;
  int rightX;
  leftX = 55 * dir;
  leftY = 0;
  sVisionLower.takeSnapshot(sigGreen);
  // looks for red ball within +/-60 pixels of centerline
  // red ball must be 40 pixels in width
  while(sVisionUpper.objectCount == 0 || abs(sVisionUpper.largestObject.centerX - 180) > 60 || sVisionUpper.largestObject.width < 40){
    wait(10, msec);
    sVisionUpper.takeSnapshot(sigGreen);
    rightX = (0 - sInertial.rotation(deg)) * 2;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
  }
  
  // Go to blue ball
  intakeOpen();
  double startTime = Brain.timer(msec);
  while (Brain.timer(msec) - startTime < 8000){
    sVisionUpper.takeSnapshot(sigGreen);
    if (sVisionUpper.objectCount > 0 && sVisionUpper.largestObject.width > 40) {
      leftX = (sVisionUpper.largestObject.centerX - 180) * .6; // used to be .8
    } else {
      leftX = 0;
    }
    leftY = 80;
    rightX = (0 - sInertial.rotation(deg)) * 2; // used to be 3
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    if (Brain.timer(msec) - startTime > 400) {
      if (getForwardVelocity() < 20) {
      break;
      }
    }
    // this line gives the robot 200ms to speed up (assuming it started at rest)
    // ForwardVelocity is the sum of the 4 wheels, so leftY would make ForwardVelocity = 320
    // ForwardVelocity will go towards zero when the robot runs into something and stops
    wait(5, msec);
  }
  driveForward(0, 0);
  // Grab ball
  intakeIn();
  // Ensure we are in the goal
  while (Brain.timer(msec) - startTime < 8000){
    sVisionUpper.takeSnapshot(sigGreen);
    if (sVisionUpper.objectCount > 0 && sVisionUpper.largestObject.width > 40) {
      leftX = (sVisionUpper.largestObject.centerX - 180) * .6; // used to be .8
    } else {
      leftX = 0;
    }
    leftY = 60;
    rightX = (0 - sInertial.rotation(deg)) * 2; // used to be 3
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    if (Brain.timer(msec) - startTime > 400) {
      if (getForwardVelocity() < 20) {
      break;
      }
    }
    wait(5, msec);
  }
  driveForward(0, 0);
  // Score
  mOutputLower.spin(fwd, 100, pct);
  mOutputUpper.spin(fwd, 100, pct);
  wait(800, msec);
  sVisionUpper.takeSnapshot(sig);
  while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 2500){
    wait(10, msec);
    sVisionUpper.takeSnapshot(sig);
  }
  mOutputLower.spin(fwd, 0, pct);
  // Make sure we score the last ball
  mOutputUpper.spin(fwd, -100, pct);
  wait(200, msec);
  intakeOpen();
  wait(400, msec);
  mOutputLower.spin(fwd, 100, pct);
  mOutputUpper.spin(fwd, 100, pct);
  // Make sure we don't score red
  sVisionUpper.takeSnapshot(sigBlue);
  while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 2500){
    wait(10, msec);
    sVisionUpper.takeSnapshot(sigBlue);
  }
  // Drive backwards
  while (0 > -4500){
    int d = 0;
    mWheelFrontLeft.resetRotation();
    mWheelBackLeft.resetRotation();
    mWheelFrontRight.resetRotation();
    mWheelBackRight.resetRotation();
    leftY = -80;
    rightX = (135 - sInertial.rotation(deg)) * 3;
    leftX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    wait(5, msec);
    d = mWheelFrontLeft.rotation(rotationUnits::raw) - mWheelFrontRight.rotation(rotationUnits::raw) + mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);
  }
  driveForward(0, 0);
}