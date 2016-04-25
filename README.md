# Arduino [![Build Status](https://travis-ci.org/markrjr/Arduino.svg?branch=master)](https://travis-ci.org/markrjr/Arduino)
Main code for Spring 2016's drone.

Overview:
The purpose of this code is to take off, fly 20 feet forward and land autonomously,
then take inputs from a remote-control to control flight.

Take-off:
Take-off is done by setting motor values barely above the threshold for vertical movement.

Landing:
Landing is done once ultrasonic sensors detect a difference of heights exceeding 2 feet, indicating that it is above the platform.
It is done by setting motor values barely below the threshold for vertical movement.

User-Controlled:
The drone receives user input through a remote-control, which is corrected through various sensors. PID control is used to make overall flight more stable,
while a barometric sensor maintains altitude and a gyroscopic sensor corrects for its angle of attack.