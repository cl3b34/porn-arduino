/*
  Automatically gives water to plant with humidity sensors, solenoid valves and a aquarium pump.
  The humidity sensor is turned off between measurements to avoid galvanic corrosion.
  This is build for a 16 relay board and an Arduino Mega driving it. Since we dedicate one of the relays to the Pump we can take care of 15 plant maximum
*/

#include <LiquidCrystal.h>

const int relayON = LOW;                  //  Our relay activates when it gets a LOW signal from arduino
const int relayOFF = HIGH;                //  Our relay activates when it gets a LOW signal from arduino
const int pumpPowerPin = 2;
const int moistureSensorsPowerPin = 26;    // all moisture sensors are powered from the same pin
const int waterLevelSensorPowerPin = 27; 
const int waterLevelSensorPin = A15;
//const int samplesToTake = 30;             // Take a few sample measurements and average them for better precision
const int samplesToTake = 5;
const int timeBetweenSamples =  300;
long previousTime = 0;                   // The last time we run
const long sleepTime = 8000000;            // Time between runs to check if plant need water. 4000000 = 66 minutes. 8.000.000 = 133 minutes (time is in miliseconds)
const int lcdRows = 2;
const int lcdCols = 16;

/*
  The plant we are caring for, If the plant doesn't have a name, we will not look after her, so give her a name!
  The order is important since they match the moisture sensors and solenoids installed in the plant
*/
String plant[] = {"Maracuja", "Limoeiro", "Tamarindeiro", "Espada de Sao Jorge", "Hortela de Fora", "", "", "", "", "", "", "", "", "", ""}; 
int moistureSensorPin[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};  // Analog Pins where the moisture sensors are connected
int solenoidPowerPin[] = {3, 4, 5 ,6 ,7 ,8 ,9 ,10 , 11, 12, 13, 22, 23, 24, 25};            // Digital pin providing power to the solenoids in the plant

/* 
   Moisture level to start and stop watering.
   A value of 1023 is a dry sensor, 1200 is out of the range of the sensor so we use that for empty slots.
   When defining a new plant, put a proper value here. 
   plant that like it wet should hover close to 500, dry should remain close to 1000 
*/
//int startWatering[] = {575, 550, 525, 800, 550, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};
int startWatering[] = {251, 550, 525, 800, 550, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};
int stopWatering[] = {550, 525, 500, 775, 525, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};       // When to stop watering  Be conservative, it is easy to get it too wet before the sensor measurement changes (water takes time to soak in)
//int wateringTime[] = {15000, 8000, 15000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                            // How long to water for ( ms )
int wateringTime[] = {1000, 8000, 15000, 30000, 30000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                            // How long to water for ( ms )
int sensorSafetyUpperLimit = 850;  // Any measurement above this is considered a faulty sensor, broken or disconnected
int sensorSafetyLowerLimit = 250;  // Any measurement bellow this is considered a faulty sensor, broken or disconnected
int sensorSafetyShorted = 1; // A reading of 1 means the sensor is shorted
// Flags to identify which plants need water
boolean shouldWater[15] = {false};
// Fautly sensors
boolean isBroken[15] = {false};

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(53,52,51,50,49,48);       // put your pin numbers here
String lastWatered = "";
String hasError = "";

// Sets the digital channels to output and make sure the relays are all OFF
void setup() {
  // start the serial and the LCD
  Serial.begin(9600);
  lcd.begin(lcdCols, lcdRows); 
  Serial.println("Initializing...");
  lcdWrite("Initializing...");   
            
  
  for(byte i = 0; i < (sizeof(solenoidPowerPin) / sizeof(solenoidPowerPin[0])); i++) 
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
  * TODO: Protection logic. Track for how long water has been given to the plants and if the moisture level remains low send some alert?
  * - Could be a leaking hose
  * - Could be a broken sensor
  * 
  * TODO: Connect a PT100 sensor outside. only add the plants outside to the watering protocol if the temperature is above 5C
  * 
  * TODO: Connect a water level sensor to the water reservoir
  * 
  * TODO: Connect a LCD screen to show the watering status: Last plants watered and measured moisture level.
  *       Bonus: Temperature inside and outside, uptime
  */
void loop() {
  unsigned long currentTime = millis();
  if(previousTime == 0 or currentTime - previousTime > sleepTime){
    // Turn sensors on and give them some time to stablilize
    digitalWrite(moistureSensorsPowerPin, HIGH); 
    delay(3000);
  
    // Check the moisture
    for(byte i = 0; i < (sizeof(moistureSensorPin) / sizeof(moistureSensorPin[0])); i++) 
    {
      if( plant[i] != "" && wateringTime[i] > 0){
        // The moisture number is counter intuitive. A big number indicates dry ground
        int moistureAveraged = sampleMoisture(i);
        Serial.println("Sampled " + plant[i] + ": " + moistureAveraged);
        lcdWrite(plant[i] + ": " + moistureAveraged);

        // Safety checks
        if (moistureAveraged >= sensorSafetyUpperLimit or moistureAveraged <= sensorSafetyLowerLimit or moistureAveraged == sensorSafetyShorted ){        
          // We got a broken sensor? A shorted connection? an empty tank? a leaky hose? in any case, will not water this one
          isBroken[i] = true;
          shouldWater[i] = false; 
          Serial.println("Sensor on " + plant[i] +  " gave a reading out of range (" + moistureAveraged + "), please check if it is broken");
          hasError = "Error: ";
        }
  
        if (moistureAveraged > startWatering[i] && ! isBroken[i]){
          Serial.println("Moisture is low in " + plant[i] + " needs watering");
          // Set a flag for this plant, we will water all at the same time later
          shouldWater[i] = true; 
          lastWatered = "Last Watered: ";
        }else if (moistureAveraged < stopWatering[i]) {
          Serial.println("Moisture is high in " + plant[i] + " do NOT need watering");
          // remove the flag
          shouldWater[i] = false; 
        }
      }else{
        Serial.print("No plant installed on sensor ");
        Serial.print(i, DEC);
        Serial.println(" . Skipping.");
        shouldWater[i] = false;
      }
    }
  
    // Don't need the sensors anymore, turn them OFF
    digitalWrite(moistureSensorsPowerPin, LOW);               // Moisture sensor off
    
    // Do the watering
    for(byte i = 0; i < (sizeof(shouldWater) / sizeof(shouldWater[0])); i++) 
    {
      if (shouldWater[i] == true) {
        Serial.println("Watering " + plant[i] + " for " + wateringTime[i]/1000 + "s");
        lcdWrite("Watering " + plant[i] + " for " + wateringTime[i]/1000 + "s");
        digitalWrite(solenoidPowerPin[i], relayON);           // First open valve
        digitalWrite(pumpPowerPin, relayON);                  // Then turn pump on
        delay(wateringTime[i]);                               // Water for the defined period
        digitalWrite(solenoidPowerPin[i], relayOFF); // Close the valve
        Serial.println("adding " + plant[i] + " to last watered list");
        lastWatered += plant[i];
        lastWatered += " ";
      }
    }

    // Collect the errors for displaying
    for(byte i = 0; i < (sizeof(isBroken) / sizeof(isBroken[0])); i++) 
    {
      if (isBroken[i] == true) {
        hasError += plant[i] + "sensor gave a strange reading";
      }
    }
  
    // Turn the pump off before sleeping until next cycle
    digitalWrite(pumpPowerPin, relayOFF);   
    // Save the time of the last run
    previousTime = millis();            
  }

  if (lastWatered != ""){
    lcdWrite(lastWatered);
    Serial.println(lastWatered);
    delay(2000);
  }
  unsigned int timeLeft = (sleepTime - (currentTime - previousTime))/1000/60;
  Serial.print("Minutes to next run: ");
  Serial.println(timeLeft);
  lcd.clear();
//  lcd.setCursor(0,0);
  lcd.print("Next run: " );
  lcd.print(timeLeft);
  lcd.print("m");

  delay(5000);
  if(hasError != ""){
    lcdWrite(hasError);
    delay(20000);
  }
}


// samples the moisture of the plant the defined amount of times and return an average
int sampleMoisture(int moistureSensor) 
{
  unsigned int moistureSampleSum = 0;
  for(int i = 0; i < samplesToTake; i++)
  {
    delay(timeBetweenSamples);
    moistureSampleSum += analogRead(moistureSensorPin[moistureSensor]);
  }
  // Return the average of the readings 
  return moistureSampleSum / samplesToTake;
 }

 void lcdWrite(String text){
  // A 2 lines LCD writes on line 0 and 2
  // A 4 lines will write on lines 0, 2, 1 and finally 3
  int lcdLines[lcdRows];
  if(lcdRows == 2){
    lcdLines[0] = 0;
    lcdLines[1] = 2;
  } else if(lcdRows == 4){
    for(byte i=0; i<4; i++){
      lcdLines[i]=i;
    }
  }   

  lcd.clear();
  if(text.length() > lcdCols){
    char buffer[text.length()+1];
    text.toCharArray(buffer, text.length()+1);
    byte lin = 0;
    for(byte i=0; i<text.length(); i++){
      if( (i+1) % ((lcdCols * lcdRows)+1) != 0){  // the screen is not full yet
          if( (i+1) % (lcdCols+1)  != 0  ){  // the line is not full yet
            lcd.print(buffer[i]); 
          }else{
            Serial.print("Moving to next line: ");
            if(lin < sizeof(lcdLines)/sizeof(lcdLines[0])){
              lin += 1;              
            }else{
              // back to first line
              lin = 0;
            }
              lcd.setCursor(0,lcdLines[lin]);  // move to next line
              lcd.print(buffer[i]);
          }
      }else{  // move to next 'page'
        Serial.println("Moving to next page");
        delay(2000);
        lcd.clear();
        lcd.print(buffer[i]);
      }
   }   
  }else{
    lcd.print(text);
  }  
}
