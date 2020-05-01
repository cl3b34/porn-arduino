/*
  Automatically gives water to plant with humidity sensors, solenoid valves and a aquarium pump.
  The humidity sensor is turned off between measurements to avoid galvanic corrosion.
  This is build for a 16 relay board and an Arduino Mega driving it. Since we dedicate one of the relays to the Pump we can take care of 15 plant maximum
*/

#include <LiquidCrystal.h>

/*
    Scaffolding for debug
*/

//#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank and a bunch of variables are set with values adequate for debugging

#ifdef DEBUG                                            //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)      //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DELAY(...)    delay(__VA_ARGS__)
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)
  #define DELAY(...)
#endif

const int relayON = LOW;                  //  Our relay activates when it gets a LOW signal from arduino
const int relayOFF = HIGH;               
const int pumpPowerPin = 2;
const int moistureSensorsPowerPin = 26;    // all moisture sensors are powered from the same pin
const int waterLevelSensorPowerPin = 27;
const int waterLevelSensorPin = A15;

#ifdef DEBUG // Development rig
  const int samplesToTake = 5;
  const unsigned long timeBetweenSamples =  100;
  const int lcdRows = 4;
  const int lcdCols = 20;
#else
  const int samplesToTake = 30;             // Take a few sample measurements and average them for better precision
  const unsigned long timeBetweenSamples =  300;
  const int lcdRows = 2;
  const int lcdCols = 16;
#endif


/*
  The plant we are caring for, If the plant doesn't have a name, we will not look after her, so give her a name!
  The order is important since they match the moisture sensors and solenoids installed in the plant
*/
String plant[] = {"Maracuja", "Limoeiro", "Tamarindeiro", "Espada de Sao Jorge", "Hortela de Fora", "", "", "", "", "", "", "", "", "", ""};
// Moisture sensors (http://www.circuitstoday.com/arduino-soil-moisture-sensor) can be connected either to analog or digital pins, however digital pins are only 'wet/dry' not very useful...
int moistureSensorPin[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14}; 
int solenoidPowerPin[] = {3, 4, 5 , 6 , 7 , 8 , 9 , 10 , 11, 12, 13, 22, 23, 24, 25};       // Digital pin providing power to the solenoids in the plant

/*
   Moisture level to start and stop watering.
   A value of 1023 is a dry sensor, 1200 is out of the range of the sensor so we use that for empty slots.
   When defining a new plant, put a proper value here.
   plant that like it wet should hover close to 500, dry should remain close to 800
*/
#ifdef DEBUG  // Development
  const unsigned long sleepTime = 120000;
  int startWatering[] = {100, 600, 600, 800, 100, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};  // first plant always is watering
  int stopWatering[] = {600, 575, 575, 775, 575, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};
  unsigned long wateringTime[] = {3000, 8000, 15000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};               // water for just enough time so we can inspect ( UL = unsigned long )
  int sensorSafetyUpperLimit = 850;
  int sensorSafetyLowerLimit = 250;  // force at least one plant sensor to be 'defective'
  int sensorSafetyShorted = 1;
#else
  const unsigned long sleepTime = 8000000;            // Time between runs to check if plant need water. 4.000.000 = 66 minutes. 8.000.000 = 133 minutes (time is in miliseconds)
  int startWatering[] = {625, 600, 600, 800, 675, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};
  int stopWatering[] = {600, 575, 575, 775, 655, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};       // When to stop watering. Be conservative, it is easy to get it too wet before the sensor measurement changes (water takes time to soak in)
  unsigned long wateringTime[] = {15000, 8000, 15000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                  // How long to water for ( ms )
  // Any measurement above or bellow those is considered a faulty sensor, broken or disconnected. A reading of 1 means the sensor is shorted
  int sensorSafetyUpperLimit = 850;  
  int sensorSafetyLowerLimit = 250;  
  int sensorSafetyShorted = 1;
#endif

boolean shouldWater[15] = {false};  // Flags to identify which plants need water
boolean isBroken[15] = {false};     // Fautly sensors


//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(53, 52, 51, 50, 49, 48);  // put your pin numbers here
String lastWatered = "";
String hasError = "";

unsigned long currentTime = 0;      // track the current time
unsigned long previousTime = 0;     // The last time we run



// Sets the digital channels to output and make sure the relays are all OFF
void setup() {
  // start the serial and the LCD
  Serial.begin(9600);
  lcd.begin(lcdCols, lcdRows);
  DPRINTLN("Initializing...");
  lcdWrite("Initializing...");

  for (byte i = 0; i < (sizeof(solenoidPowerPin) / sizeof(solenoidPowerPin[0])); i++)
  {
    pinMode(solenoidPowerPin[i], OUTPUT);
    digitalWrite(solenoidPowerPin[i], relayOFF);        // All valves off
  }

  pinMode(moistureSensorsPowerPin, OUTPUT);
  digitalWrite(moistureSensorsPowerPin, LOW);        // moisture sensor OFF
  pinMode(pumpPowerPin, OUTPUT);
  digitalWrite(pumpPowerPin, relayOFF);                // Pump Off
  pinMode(waterLevelSensorPowerPin, OUTPUT);
  digitalWrite(waterLevelSensorPowerPin, LOW);    // water level sensor OFF
}

/*
   TODO: Protection logic. Track for how long water has been given to the plants and if the moisture level remains low send some alert?
   - Could be a leaking hose
   - Could be a broken sensor

   TODO: Connect a PT100 sensor outside. only add the plants outside to the watering protocol if the temperature is above 5C
   TODO: Connect a water level sensor to the water reservoir

*/
void loop() {
  currentTime = millis();

  if (previousTime == 0 or currentTime - previousTime > sleepTime) {
    // Turn sensors on and give them some time to stablilize
    digitalWrite(moistureSensorsPowerPin, HIGH);
    wait(3000UL);

    // Check the moisture
    for (byte i = 0; i < (sizeof(moistureSensorPin) / sizeof(moistureSensorPin[0])); i++)
    {
      if ( plant[i] != "" && wateringTime[i] > 0) {
        // The moisture number is counter intuitive. A big number indicates dry ground
        int moistureAveraged = sampleMoisture(i);
        DPRINTLN("Sampled " + plant[i] + ": " + moistureAveraged);
        lcdWrite(plant[i] + ": " + moistureAveraged);
        wait(500UL);

        // Make sure there isn't anything strange on the sensor readings before allowing watering
        if (moistureAveraged >= sensorSafetyUpperLimit or moistureAveraged <= sensorSafetyLowerLimit or moistureAveraged == sensorSafetyShorted ) {
          // We got a broken sensor? A shorted connection? an empty tank? a leaky hose? in any case, will not water this one
          isBroken[i] = true;
          shouldWater[i] = false;
          DPRINTLN("Sensor on " + plant[i] +  " gave a reading out of range (" + moistureAveraged + "), please check if it is broken");
        }

        if (moistureAveraged > startWatering[i] && ! isBroken[i]) {
          DPRINTLN("Moisture is low in " + plant[i] + " needs watering");
          // Set a flag for this plant, we will water all at the same time later
          shouldWater[i] = true;
        } else if (moistureAveraged < stopWatering[i]) {
          DPRINTLN("Moisture is high in " + plant[i] + " do NOT need watering");
          // remove the flag
          shouldWater[i] = false;
        }
      } else {
        DPRINT("No plant installed on sensor ");
        DPRINT(i, DEC);
        DPRINTLN(" . Skipping.");
        shouldWater[i] = false;
      }
    }

    digitalWrite(moistureSensorsPowerPin, LOW);          // Don't need the sensors anymore, turn them OFF

    // water the plants
    doWatering();

    digitalWrite(pumpPowerPin, relayOFF);      // Turn the pump off before sleeping until next cycle
    previousTime = millis();                   // Save the time of the last run
  }

  // show the current watering status
  showStatus();
}

/*
   Wait for an specified amount of time without blocking the CPU (as delay() do)
*/
unsigned long endOfWait = 0;
void wait(unsigned long timeToWait) {
  //  DPRINTLN("Wait called");
  endOfWait = millis() + timeToWait;
  while (true) {
    currentTime = millis();
    if (currentTime >= endOfWait) {
      break;
    }
  }
  //  DPRINTLN("end of wait");
}


void showStatus() {
  
  showLastWatered();
  wait(3000UL);

  showNextRun();
  wait(3000UL);

  showUptime();
  wait(3000UL);

  showErrors();
  wait(3000UL);

  // if there is no error condition, turn display off for 5 minutes
  if (hasError == "") {
    lcd.noDisplay();
    wait(5*60*1000); 
    lcd.display();
  }
}


void showErrors(){
  // Collect the errors for displaying
  boolean somethingBroken = false;
  for (byte i = 0; i < (sizeof(isBroken) / sizeof(isBroken[0])); i++)
  {
    if (isBroken[i] == true) {
      if (hasError == "") {
        hasError += "Error, these sensors gave an strange reading:";
      }
      hasError += " " + plant[i];
      isBroken[i] = false;
      somethingBroken = true;
    }
  }
  if (hasError != "") {
    lcdWrite(hasError);
  }
  if(!somethingBroken){
    hasError = "";
  }
}


void showLastWatered() {
  if (lastWatered != "") {
    lcdWrite(lastWatered);
    DPRINTLN(lastWatered);
  }
}


void showNextRun() {
  unsigned int timeLeft = (sleepTime - (currentTime - previousTime)) / 1000 / 60;
  DPRINT("Minutes to next run: ");
  DPRINTLN(timeLeft);
  lcd.clear();
  lcd.print("Next run: " );
  lcd.print(timeLeft);
  lcd.print("m");
}

void showUptime() {
  unsigned int uptimeMin = currentTime / 1000 / 60;    // Time since last reset, in minutes
  unsigned int uptimeHr = uptimeMin / 60;              // in hours
  unsigned int uptimeDay = uptimeHr / 24;              // in days

  DPRINT("Uptime Min: ");
  DPRINTLN(uptimeMin);
  DPRINT("Uptime Hr: ");
  DPRINTLN(uptimeHr);
  DPRINT("Uptime Day: ");
  DPRINTLN(uptimeDay);

  lcd.clear();
  if (uptimeMin < 1440) { // up to a day, show in minutes
    lcd.print("Uptime: " );
    lcd.print(uptimeMin);
    lcd.print("m");
  } else if (uptimeHr < 240) { // up to 10 days, show in hours
    lcd.print("Uptime: " );
    lcd.print(uptimeHr);
    lcd.print("h");
  } else {                 // show in days
    lcd.print("Uptime: " );
    lcd.print(uptimeDay);
    lcd.print(" Days");
  }
}


/*
   Do the actual watering on the plants
*/
void doWatering() {
  lastWatered = "";
  for (byte i = 0; i < (sizeof(shouldWater) / sizeof(shouldWater[0])); i++)
  {
    if (shouldWater[i] == true) {
      if (lastWatered == "") {
        lastWatered = "Last Watered: ";
      }
      DPRINTLN("Watering " + plant[i] + " for " + wateringTime[i] / 1000 + "s");
      lcdWrite("Watering " + plant[i] + " for " + wateringTime[i] / 1000 + "s");
      digitalWrite(solenoidPowerPin[i], relayON);           // First open valve
      digitalWrite(pumpPowerPin, relayON);                  // Then turn pump on
      wait(wateringTime[i]);                               // Water for the defined period
      digitalWrite(solenoidPowerPin[i], relayOFF); // Close the valve
      DPRINTLN("adding " + plant[i] + " to last watered list");
      lastWatered += plant[i];
      lastWatered += " ";
      shouldWater[i] = false;
    }
  }
}



// samples the moisture of the plant the defined amount of times and return an average
int sampleMoisture(int moistureSensor)
{
  unsigned int moistureSampleSum = 0;
  for (int i = 0; i < samplesToTake; i++)
  {
    wait(timeBetweenSamples);
    moistureSampleSum += analogRead(moistureSensorPin[moistureSensor]);
  }
  // Return the average of the readings
  return moistureSampleSum / samplesToTake;
}


/*
   Handles changes of line and pagination for different sizes of LCD Screens
   Currently supports only 16x2 and 20x4
*/
void lcdWrite(String text) {
  // A 2 lines LCD writes on line 0 and 2
  // A 4 lines will write on lines 0, 2, 1 and finally 3
  int lcdLines[lcdRows];
  if (lcdRows == 2) {
    lcdLines[0] = 0;
    lcdLines[1] = 2;
  } else if (lcdRows == 4) {
    for (byte i = 0; i < 4; i++) {
      lcdLines[i] = i;
    }
  }

  DPRINT("Text size to print ");
  DPRINTLN(text.length());

  lcd.clear();
  if (text.length() > lcdCols) {
    char buffer[text.length() + 1];
    text.toCharArray(buffer, text.length() + 1);
    byte lin = 0;
    for (byte i = 0; i < text.length(); i++) {
      //       DPRINT("calculating remainder of ");
      //       DPRINT( i+1);
      //       DPRINT(" and " );
      //       DPRINTLN(lcdCols * lcdRows);
      //       DPRINT("Remainder screen " );
      //       DPRINTLN((i+1) % (lcdCols * lcdRows));

      if ( (i + 1) % (lcdCols * lcdRows) != 0) { // the screen is not full yet
        //        DPRINT("Remainder line of ");
        //        DPRINT( i + 1);
        //        DPRINT(" and ");
        //        DPRINTLN(lcdCols);
        //        DPRINT("Remainder line " );
        //        DPRINTLN( (i + 1) % (lcdCols) );
        if ( (i + 1) % (lcdCols)  != 0  ) {     // the line is not full yet
          lcd.print(buffer[i]);
          //  DPRINT(buffer[i]);
          wait(100UL);
        } else {
          // We are at the last character on the line, let's print and move to the next line
          lcd.print(buffer[i]);
          //            int lines = (sizeof(lcdLines)/sizeof(lcdLines[0]))-1;
          //            DPRINT("lines " );
          //            DPRINTLN(lines);
          if (lin < (sizeof(lcdLines) / sizeof(lcdLines[0])) - 1) {
            //            DPRINTLN("adding to lin ");
            lin += 1;
          } else {
//            DPRINTLN("Cursor moved to line zero");
            lin = 0; // back to first line
          }
          //            DPRINT("lin variable ");
          //            DPRINTLN(lin);
          //            DPRINT("Moving to next line: ");
          //            DPRINTLN(lcdLines[lin]);
          //            for(byte x=0; x<sizeof(lcdLines)/sizeof(lcdLines[0]); x++){
          //              DPRINT("contents of lcdLines ");
          //              DPRINTLN(lcdLines[x]);
          //            }
//          DPRINTLN("Moving cursor to next line ");
          //          DPRINTLN(lcdLines[lin]);
          lcd.setCursor(0, lcdLines[lin]);        // move cursor to next line
          DELAY(100);
        }
      } else { // we are at the last character of the page, print it a and move to next page
        lcd.print(buffer[i]);
        DPRINTLN("Moving to next page");
        wait(2000UL);
        lcd.clear();
//        DPRINTLN("Cursor moved to line zero");
        lin = 0;
        //        DPRINTLN("rolling lin back to zero ");
        DELAY(100);
      }
    }
  } else {
    lcd.print(text);
  }
}
