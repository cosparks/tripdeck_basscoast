# Trypdeck Basscoast

Software for an interactive multimedia fortune-telling installation.  This project is designed to compile and run on a raspberry pi.

## Building and Running the Project
### Install PIGPIO Library:
• to install PIGPIO, navigate to a folder where you store libraries/external resources and enter (you may have to include sudo before each command if you don't have permissions)
```
$ wget https://github.com/joan2937/pigpio/archive/master.zip
$ unzip master.zip
$ cd pigpio-master
$ make
$ sudo make install
```
NOTE: run these commands if the python part of the install fails:
```
$ sudo apt install python-setuptools python3-setuptools
$ sudo make install
```
##### (Optional) Test PIGPIO:
• check that pigpio is working correctly (don't enter the `#` or anything after it)
```
$ sudo ./x_pigpio # check C I/F

$ sudo pigpiod    # start daemon

$ ./x_pigpiod_if2 # check C      I/F to daemon
$ ./x_pigpio.py   # check Python I/F to daemon
$ ./x_pigs        # check pigs   I/F to daemon
$ ./x_pipe        # check pipe   I/F to daemon
```
### Install OpenCv:
```
...
```
### Clone the Project:
• navigate to the desired folder and in the command line of your raspberry pi enter
```
$ git clone https://github.com/cosparks/tripdeck_basscoast.git
$ git pull
```
### Build and Run:
• navigate to `.../tripdeck_basscoast/src` folder and enter
```
$ make tripdeck
$ sudo ./tripdeck
```
And you're done!

## Possible Issues with Building/Running

If pigpio fails to initalize, it might be because a pigpio daemon is still running, so try
```
$ sudo killall pigpiod
```
If the raspberry pi is taking an extremely long time to compile, it may be because it is running out of memory, so you can try increasing the size of the stack-swapping cache:

• enter
```
$ sudo nano /etc/dphys-swapfile
```
• edit the gile increase CONF_SWAPSIZE from 100 to 1000MB (make sure you have enough storage to do this), then
```
$ /etc/init.d/dphys-swapfile restart
```

### Dependencies:
openCV, pigpio, stl c++17, make
