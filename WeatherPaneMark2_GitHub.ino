/* WeatherPane v2
 * Created May 1st, 2016
 * www.weatherpane.net
 *
 * This sketch will run three 28YBJ-48 stepper motors to point to weather data
 * obtained from the Weather Underground and displayed on a clock-like face.
 * The stepper motors are initialized after each update to make sure
 * that the pointers are reasonably accurate. The home condition
 * is established using hall sensors on the analog data pins.
 *
 * Data collected from WU includes the current weather condition, current
 * temperature, and current chance of precipitation.
 *
 * Contact me with any questions or updates: info@weatherpane.net or doktorinjh@gmail.com
 *
 * This code is not licensed or approved for commercial use and may not be replicated
 * without contacting the creator.
 * 
 * Change Log:
 * 6/16/16 - Changed stuff back, change all ints to double
 * 6/14/16 - Commented out Stepper.h and MemoryFree.h, changed Pins to #define, lowered char counts, commented out serial and console prints
 * 6/11/16 - Changed Double to Int for steppers
 * 5/29/16 - Moved scripts to WeatherPaneScripts instead of SD card
 * 5/27/16 - Removed millis for time, added MemoryFree for leak check
 * 
 */

//#include <Stepper.h>                                              // Include Libraries
#include <Process.h>
#include <Bridge.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <MemoryFree.h>

MultiStepper steppers;

//#define IconPin A0                                           // Analog pin for the Icon Hall Sensor
//#define TempPin A1                                           // Analog pin for the Temp Hall Sensor
//#define PrecipPin A2                                         // Analog pin for the Precip Hall Sensor
const int IconPin = A0;                                           // Analog pin for the Icon Hall Sensor
const int TempPin = A1;                                           // Analog pin for the Temp Hall Sensor
const int PrecipPin = A2;                                         // Analog pin for the Precip Hall Sensor

double IconCurrent = 1000;                                        // Initilize variable for Icon Hall Sensor
double TempCurrent = 1000;                                        // Initilize variable for Temp Hall Sensor
double PrecipCurrent = 1000;                                      // Initilize variable for Precip Hall Sensor
//int IconCurrent = 1000;                                           // Initilize variable for Icon Hall Sensor
//int TempCurrent = 1000;                                           // Initilize variable for Temp Hall Sensor
//int PrecipCurrent = 1000;                                         // Initilize variable for Precip Hall Sensor

const int IconCorrection = 40;                                    // If Icon Hall Sensor isn't 0 degrees, adjust here
const int TempCorrection = 110;                                   // If Temp Hall Sensor isn't 0 degrees, adjust here
const int PrecipCorrection = 80;                                  // If Precip Hall Sensor isn't 0 degrees, adjust here

char location[30];                                                // Variables for location and update time
char update_time[45];

char current_temp[7];                                            // Variables for current conditions
char current_weather[30];
char current_precip[7];

#define STEPS 200                                                 // Define Steps for motor, not sure if needed
//Stepper icon_stepper(STEPS, 2, 4, 3, 5);                          // Setup Icon motor for 4-wire on pins 2-5
//Stepper temp_stepper(STEPS, 6, 8, 7, 9);                          // Setup Temp motor for 4-wire on pins 6-9
//Stepper precip_stepper(STEPS, 10, 12, 11, 13);                    // Setup Precip motor for 4-wire on pins 10-13

AccelStepper icon_accel(4, 2, 4, 3, 5);                           // Setup Icon motor for 4-wire on pins 2-5
AccelStepper temp_accel(4, 6, 8, 7, 9);                           // Setup Temp motor for 4-wire on pins 6-9
AccelStepper precip_accel(4, 10, 12, 11, 13);                     // Setup Precip motor for 4-wire on pins 10-13

//unsigned long interval = 600000;                                  // Wait 10 minutes to update

long positions[3];

double freeRam()
//int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() {
  icon_accel.setMaxSpeed(500.0);                                  // AccelStepper setup for the 3 motors
  icon_accel.setAcceleration(1000.0);
  temp_accel.setMaxSpeed(500.0);
  temp_accel.setAcceleration(1000.0);
  precip_accel.setMaxSpeed(500.0);
  precip_accel.setAcceleration(1000.0);

  steppers.addStepper(icon_accel);                                // Add Multistepper motors
  steppers.addStepper(temp_accel);
  steppers.addStepper(precip_accel);
  
  Initialize();                                                   // Get the steppers to their homes

  double current_temp_int = atoi(current_temp);                      // Convert values to integers
  double current_precip_int = atoi(current_precip);        
  //int current_temp_int = atoi(current_temp);                      // Convert values to integers
  //int current_precip_int = atoi(current_precip);
  
  Bridge.begin();                                                 // Begin Bridge
  //Console.begin();                                                // Begin Console
  delay(5000);
  //Console.println(freeRam());

  Process bridge;                                                 // Get intitial values from the bridge
  bridge.runShellCommand("python /WeatherPaneScripts/WU_Bridge.py");

  Process variables;                                              // Get the variables
  variables.runShellCommand("python /WeatherPaneScripts/WU_Variables.py");

  Bridge.get("current_temp", current_temp, 7);                    // Get variables and convert to integers
  Bridge.get("current_weather", current_weather, 30);
  Bridge.get("current_precip", current_precip, 7);
  Bridge.get("location", location, 30);
  Bridge.get("time", update_time, 45);

  current_temp_int = atoi(current_temp);                          // Convert values to integers
  current_precip_int = atoi(current_precip);
  
  //Console.println(freeMemory());
  //ConsolePrint();

  Condition(current_weather);                                     // Display current weather condition
  Temperature(current_temp_int);                                  // Display current temperature
  Precipitation(current_precip_int);                              // Display current chance of precip
      steppers.moveTo(positions);                                 // MultiStepper move
      steppers.runSpeedToPosition();
  stopmotor();
}


void loop() {
  
    Process bridge;                                               // Get intitial values from the bridge
    bridge.runShellCommand("python /WeatherPaneScripts/WU_Bridge.py");

    Process variables;                                            // Get the variables
    variables.runShellCommand("python /WeatherPaneScripts/WU_Variables.py");
    
    Bridge.get("current_temp", current_temp, 7);                  // Get variables and convert to integers
    Bridge.get("current_weather", current_weather, 30);
    Bridge.get("current_precip", current_precip, 7);
    Bridge.get("location", location, 30);
    Bridge.get("time", update_time, 45);

    double current_temp_int = atoi(current_temp);                      // Convert values to integers
    double current_precip_int = atoi(current_precip);
    //int current_temp_int = atoi(current_temp);                    // Convert values to integers
    //int current_precip_int = atoi(current_precip);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Initialize();                                               // Find home (moves changes when commented out)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Console.println(freeMemory());        
    //ConsolePrint();

    Condition(current_weather);                                   // Display current weather condition
    Temperature(current_temp_int);                                // Display current temperature
    Precipitation(current_precip_int);                            // Display today's chance of precip
      steppers.moveTo(positions);                                 // MultiStepper move
      steppers.runSpeedToPosition();

    stopmotor();                                                  // Turn off motors
    delay(600000);
}


void Initialize() {
    Bridge.get("current_temp", current_temp, 7);                  
    Bridge.get("current_weather", current_weather, 30);
    Bridge.get("current_precip", current_precip, 7);

    double current_temp_int = atoi(current_temp);                      // Convert values to integers
    double current_precip_int = atoi(current_precip);
    //int current_temp_int = atoi(current_temp);                    
    //int current_precip_int = atoi(current_precip);

    IconCurrent = analogRead(IconPin);                            // Read the analog pin values
    TempCurrent = analogRead(TempPin);
    PrecipCurrent = analogRead(PrecipPin);

    if (IconCurrent < 600)  {                                     // Move the pointer away from the hall sensor
      icon_accel.move(500);}
    if (TempCurrent < 600)  {
      temp_accel.move(500);}
    if (PrecipCurrent < 600) {
      precip_accel.move(500); }
    while ((icon_accel.distanceToGo()!= 0)||
      (temp_accel.distanceToGo()!= 0)||
        (precip_accel.distanceToGo()!= 0)){
      icon_accel.run(); 
      temp_accel.run();
      precip_accel.run();}
  
  boolean iconHomed = false;
  boolean tempHomed = false;
  boolean precipHomed = false;
  
  icon_accel.setSpeed(500);                                       // Set speed for steppers
  temp_accel.setSpeed(500);
  precip_accel.setSpeed(500);

  while (!iconHomed || !tempHomed || !precipHomed) {              // While any one of the steppers are not homed
    if (!iconHomed) {                                             // Home Icon stepper
      IconCurrent = analogRead(IconPin);
      if (IconCurrent < 600) {                                    // Home found
        iconHomed = true;
        icon_accel.moveTo(icon_accel.currentPosition());          // Zero the target
        icon_accel.setSpeed(0);}                                  // Set speed to 0 to prevent movement
    }

    if (!tempHomed) {                                             // Home the Temp stepper
      TempCurrent = analogRead(TempPin);
      if (TempCurrent < 600) {
        tempHomed = true;
        temp_accel.moveTo(temp_accel.currentPosition());
        temp_accel.setSpeed(0);}
    }

    if (!precipHomed) {                                           // Home the Precip stepper
      PrecipCurrent = analogRead(PrecipPin);
      if (PrecipCurrent < 600) {
        precipHomed = true;
        precip_accel.moveTo(precip_accel.currentPosition());
        precip_accel.setSpeed(0);}
    }
    
    icon_accel.runSpeed();                                        // Run if not home
    temp_accel.runSpeed();
    precip_accel.runSpeed();
  }
  
  icon_accel.move(IconCorrection);                                // Add correction to get to "real" 0
  temp_accel.move(TempCorrection);
  precip_accel.move(PrecipCorrection);
      while ((icon_accel.distanceToGo()!= 0)||
      (temp_accel.distanceToGo()!= 0)||
      (precip_accel.distanceToGo()!= 0)){
      icon_accel.run(); 
      temp_accel.run();
      precip_accel.run();}
  stopmotor();
  icon_accel.setCurrentPosition(0);
  temp_accel.setCurrentPosition(0);
  precip_accel.setCurrentPosition(0);

}


void stopmotor() {                                                // Turns off the Stepper Motors
  digitalWrite(2, LOW);                                           // Icon motor
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  digitalWrite(6, LOW);                                           // Temp motor
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

  digitalWrite(10, LOW);                                          // Precipitation motor
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

//void Temperature (int t) {                                        // Temperature Function
void Temperature (double t) {                                        // Temperature Function
  if (t < -22) {                                                  // -25 or less Degrees
    positions[1] = 1109; }
  if (t >= -22 && t < -18) {                                      // -20 Degrees
    positions[1] = 1195; }
  if ((t >= -18) && (t < -13)) {                                  // -15 Degrees
    positions[1] = 1280; }
  if ( (t >= -13) && (t < -8)) {                                  // -10 Degrees
    positions[1] = 1365; }
  if ( (t >= -8) && (t < -3)) {                                   // -5 Degrees
    positions[1] = 1451; }
  if ( (t >= -3) && (t < 3)) {                                    // 0 Degrees
    positions[1] = 1536; }
  if ( (t >= 3) && (t < 8)) {                                     // 5 Degrees
    positions[1] = 1621; }
  if ( (t >= 8) && (t < 13)) {                                    // 10 Degrees
    positions[1] = 1707; }
  if ( (t >= 13) && (t < 18)) {                                   // 15 Degrees
    positions[1] = 1792; }
  if ( (t >= 18) && (t < 23)) {                                   // 20 Degrees
    positions[1] = 1877; }
  if ( (t >= 23) && (t < 28)) {                                   // 25 Degrees
    positions[1] = 1963; }
  if ( (t >= 28) && (t < 33)) {                                   // 30 Degrees
    positions[1] = 2048; }
  if ( (t >= 33) && (t < 38)) {                                   // 35 Degrees
    positions[1] = 85; }
  if ( (t >= 38) && (t < 43)) {                                   // 40 Degrees
    positions[1] = 171; }
  if ( (t >= 43) && (t < 48)) {                                   // 45 Degrees
    positions[1] = 256; }
  if ( (t >= 48) && (t < 53)) {                                   // 50 Degrees
    positions[1] = 341; }
  if ( (t >= 53) && (t < 58)) {                                   // 55 Degrees
    positions[1] = 440; }
  if ( (t >= 58) && (t < 63)) {                                   // 60 Degrees
    positions[1] = 512; }
  if ( (t >= 63) && (t < 68)) {                                   // 65 Degrees
    positions[1] = 597; }
  if ( (t >= 68) && (t < 73)) {                                   // 70 Degrees
    positions[1] = 683; }
  if ( (t >= 73) && (t < 78)) {                                   // 75 Degrees
    positions[1] = 768; }
  if ( (t >= 78) && (t < 83)) {                                   // 80 Degrees
    positions[1] = 853; }
  if ( (t >= 83) && (t < 88)) {                                   // 85 Degrees
    positions[1] = 939; }
  if (t >= 88 && (t < 110)) {                                     // 90+ Degrees
    positions[1] = 1030; }
}

void Precipitation (double p) {                                      // Precipitation Function
//void Precipitation (int p) {                                      // Precipitation Function
  if ( (p >= 0) && (p < 10)); {                                   // 0% Chance
    positions[2] = 1229; }
  if ( (p >= 10) && (p < 35)) {                                   // 25% Chance
    positions[2] = 1638; }
  if ( (p >= 35) && (p < 65)) {                                   // 50% Chance
    positions[2] = 2048; } 
  if ( (p >= 65) && (p < 85)) {                                   // 75% Chance
    positions[2] = 370; }
  if ( (p >= 85) && (p < 110)) {                                  // 100% Chance
    positions[2] = 808; }
}


void Condition (char icon[]) {                                    // Weather Icon Functions, 25 in Total
  if (strcmp(icon, "Clear") == 0) {                               // Clear
    positions[0] = 2048; }
  if (strcmp(icon, "Cloudy") == 0) {                              // Cloudy
    positions[0] = 530; }
  if (strcmp(icon, "Partly Cloudy") == 0) {                       // Partly Cloudy
      positions[0] = 1792; }
  if (strcmp(icon, "Mostly Cloudy") == 0) {                       // Mostly Cloudy
    positions[0] = 530; }
  if (strcmp(icon, "Rain") == 0) {                                // Rain
    positions[0] = 1536; }
  if (strcmp(icon, "Snow") == 0) {                                // Snow
    positions[0] = 1024; }
  if (strcmp(icon, "Snow Showers") == 0) {                        // Snow Showers
    positions[0] = 1024; }
  if (strcmp(icon, "Thunderstorm") == 0) {                        // Thunderstorm
    positions[0] = 768; }
  if (strcmp(icon, "Overcast") == 0) {                            // Overcast
    positions[0] = 530; }
  if (strcmp(icon, "Chance of Rain") == 0) {                      // Chance of Rain
    positions[0] = 256; }
  if (strcmp(icon, "Chance of Thunderstorms") == 0) {             // Chance of Thunderstorms
    positions[0] = 1280; }
  if (strcmp(icon, "Chance of a Thunderstorm") == 0) {            // Chance of a Thunderstorm
    positions[0] = 1280; }
  if (strcmp(icon, "Chance of Flurries") == 0) {                  // Chance of Flurries
    positions[0] = 1024; }
  if (strcmp(icon, "Chance Rain") == 0) {                         // Chance Rain
    positions[0] = 1536; }
  if (strcmp(icon, "Chance of Freezing Rain") == 0) {             // Chance of Freezing Rain
    positions[0] = 1024; }
  if (strcmp(icon, "Chance of Sleet") == 0) {                     // Chance of Sleet
    positions[0] = 1024; }
  if (strcmp(icon, "Flurries") == 0) {                            // Flurries (not furries)
    positions[0] = 1024; }
  if (strcmp(icon, "Fog") == 0) {                                 // Fog
    positions[0] = 530; }
  if (strcmp(icon, "Haze") == 0) {                                // Haze
    positions[0] = 1792; }
  if (strcmp(icon, "Mostly Sunny") == 0) {                        // Mostly Sunny
    positions[0] = 1792; }
  if (strcmp(icon, "Partly Sunny") == 0) {                        // Partly Sunny
    positions[0] = 1792; }
  if (strcmp(icon, "Freezing Rain") == 0) {                       // Freezing Rain
    positions[0] = 1024; }
  if (strcmp(icon, "Sleet") == 0) {                               // Sleet
    positions[0] = 1024; }
  if (strcmp(icon, "Thunderstorms") == 0) {                       // Thunderstorms
    positions[0] = 768; }
  if (strcmp(icon, "Unknown") == 0) {                             // Unknown
    positions[0] = 1280; }
  if (strcmp(icon, "Scattered Clouds") == 0) {                    // Scattered Clouds
    positions[0] = 1792; }
  if (strcmp(icon, "Ice Pellets") == 0) {                         // Ice Pellets
    positions[0] = 1024; }
  if (strcmp(icon, "Chance of Snow") == 0) {                      // Chance of Snow
    positions[0] = 1024; }
  if (strcmp(icon, "") == 0)  {                                   // No Reading, also used during setup
    positions[0] = -768;  }
}

/*
void ConsolePrint() {                                             // Print weather to Console to check the data
  Bridge.get("current_temp", current_temp, 7);           
  Bridge.get("current_weather", current_weather, 30);
  Bridge.get("current_precip", current_precip, 7);
  Bridge.get("location", location, 30);
  Bridge.get("time", update_time, 45);

  double current_temp_int = atoi(current_temp);             
  double current_precip_int = atoi(current_precip);
  //int current_temp_int = atoi(current_temp);             
  //int current_precip_int = atoi(current_precip);

  Console.print("In ");
  Console.print(location);
  Console.print(", it is currently ");
  Console.print(current_temp_int);
  Console.print(" degrees out and ");
  Console.print(current_weather);
  Console.println(".");
  Console.print("There is a ");
  Console.print(current_precip_int);
  Console.println("% chance of precip at the moment.");
  Console.println(update_time);
  Console.println();
  Console.println(positions[0]);
  Console.println(positions[1]);
  Console.println(positions[2]);
//  Console.println(previousMillis);
  Console.println(freeMemory());
  Console.println(freeRam());
  Console.println();
}


void SerialPrint() {                                              // Print weather to Serial to check the data
  Bridge.get("current_temp", current_temp, 7);           
  Bridge.get("current_weather", current_weather, 30);
  Bridge.get("current_precip", current_precip, 7);
  Bridge.get("location", location, 30);
  Bridge.get("time", update_time, 45);

  int current_temp_int = atoi(current_temp);                  
  int current_precip_int = atoi(current_precip);

  Serial.print("In ");                                        
  Serial.print(location);
  Serial.print(", it is currently ");
  Serial.print(current_temp_int);
  Serial.print(" degrees out and ");
  Serial.print(current_weather);
  Serial.println(".");
  Serial.print("There is a ");
  Serial.print(current_precip_int);
  Serial.println("% chance of precip today.");
  Serial.println(update_time);
  Serial.println();
  Serial.println(positions[0]);
  Serial.println(positions[1]);
  Serial.println(positions[2]);
  Serial.println();
}
*/

/* Fin */
