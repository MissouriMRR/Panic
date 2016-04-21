#!/bin/bash
########### MST PREP SCRIPT FOR TRAVIS CI ###########

cd "drone_control"

echo -e "#include <Arduino.h>\n$(cat *.ino)" > *.ino

for file in *.ino
do
 mv "$file" "${file%.ino}.cpp"
done
