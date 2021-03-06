#!/bin/bash
sudo apt update && sudo apt upgrade -y
sudo apt-get -y install build-essential cmake vlc-bin vlc-plugin-base libvlc-dev pulseaudio libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libx264-dev
sudo apt install pkg-config
mkdir ~/resources
cd ~/resources
sudo wget https://github.com/joan2937/pigpio/archive/master.zip
unzip master.zip
rm master.zip
cd pigpio-master
make
sudo apt install python-setuptools python3-setuptools
sudo make install
cd ~/resources
git clone https://github.com/jgarff/rpi_ws281x.git
mkdir rpi_ws281x/build
cd rpi_ws281x/build
cmake -D BUILD_SHARED=ON -D BUILD_TEST=ON ..
sudo make install
