# WeatherPane Mark 2
A new variation on the WeatherPane that has an antique look and uses stepper motors to point to the weather forecast. Pictures and a semi-build log are here: http://imgur.com/a/7An7O

WeatherPane v2
Created May 1st, 2016

This sketch will run three 28YBJ-48 stepper motors to point to weather data
obtained from the Weather Underground and displayed on a clock-like face.
The stepper motors are initialized after each update to make sure
that the pointers are reasonably accurate. The home condition
is established using hall sensors on the analog data pins. There are magnets imbedded in the
3D printed hands that trip the sensor. 

Data collected from WU includes the current weather condition, current
temperature, and current chance of precipitation.

Contact me with any questions, updates, or pictures of your own: doktorinjh@gmail.com

This code is not licensed or approved for commercial use and may not be replicated
without contacting the creator.
 
Change Log:
 * 6/16/16 - Changed stuff back, change all ints to double
 * 6/14/16 - Commented out Stepper.h and MemoryFree.h, changed Pins to #define, lowered char counts, commented out serial and console prints
 * 6/11/16 - Changed Double to Int for steppers
 * 5/29/16 - Moved scripts to WeatherPaneScripts instead of SD card
 * 5/27/16 - Removed millis for time, added MemoryFree for leak check
 
# Install
There are 3 files that you'll need to get started and maybe the additional stepper motor libraries? You will also need a WeatherUnderground API Key and insert that into Line 5 of the Bridge file. More WU API info is here: https://www.wunderground.com/weather/api/d/docs?MR=1

# 3D Print Files
There are several files for 3D printing the hands, hand shafts, and mounting blocks. I've included some hand variations, too. 

# Face Plate
There are some instructions in the build album that show how to get the face plate onto the pine board. You'll need some paint, modge podge, a reverse printed image, and patience. 

# Known Problems
I have had issues with the Bridge on the Yun freezing up and the whole system requires a reboot. If anyone figures it out, please let me know. This sketch isn't as clean as I'd like it, but it has been sitting on my shelf for too long and I wanted to share it. The MemoryFree library was left in because I was using that to try and diagnose the Bridge shutdown issue. 
