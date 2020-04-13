 /*
  Automatically gives water to plant with humidity sensors, solenoid valves and a aquarium pump.
  The humidity sensor is turned off between measurements to avoid galvanic corrosion.
  This is build for a 16 relay board and an Arduino Mega driving it. Since we dedicate one of the relays to the Pump we can take care of 15 plant maximum
*/

const int relayON = LOW;                  //  Our relay activates when it gets a LOW signal from arduino
const int relayOFF = HIGH;                //  Our relay activates when it gets a LOW signal from arduino
const int pumpPowerPin = 2;
const int moistureSensorsPowerPin = 25;    // all moisture sensors are powered from the same pin
const int waterLevelSensorPowerPin = 26; 
const int waterLevelSensorPin = A15;
const int samplesToTake = 30;             // Take a few sample measurements and average them for better precision
const int timeBetweenSamples =  300;
const long sleepTime = 8000000;            // Time between runs to check if plant need water. 4000000 = 66 minutes. 8.000.000 = 133 minutes (time is in miliseconds)

/*
  The plant we are caring for, If the plant doesn't have a name, we will not look after her, so give her a name!
  The order is important since they match the moisture sensors and solenoids installed in the plant
*/
// String plant[] = {"Abacaxi Pequeno", "Espada de Sao Jorge", "Tamarindeiro", "Limoeiro Grande", "Maracuja", "", "", "", "", "", "", "", "", "", ""}; 
String plant[] = {"Maracuja", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}; 
int moistureSensorPin[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};  // Analog Pins where the moisture sensors are connected
int solenoidPowerPin[] = {3, 4, 5 ,6 ,7 ,8 ,9 ,10 , 11, 12, 13, 21, 22, 23, 24};            // Digital pin providing power to the solenoids in the plant

/* 
   Moisture level to start and stop watering.
   A value of 1200 (very dry) is likely out of the range of the sensor so we use that for empty slots.
   When defining a new plant, put a proper value here. 
   plant that like it wet should hover close to 500, dry should remain close to 1000 
*/
//int startWatering[] = {550, 1000, 525, 550, 550, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};
//int stopWatering[] = {525, 925, 500, 525, 525, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200}; // When to stop watering  Be conservative, it is easy to get it too wet before the sensor measurement changes (water takes time to soak in)
//int wateringTime[] = {2000, 30000, 15000, 2000, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                        // How long to water for ( ms )

int startWatering[] = {550, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200};
int stopWatering[] = {525, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200}; // When to stop watering  Be conservative, it is easy to get it too wet before the sensor measurement changes (water takes time to soak in)
int wateringTime[] = {4000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                        // How long to water for ( ms )

// Flags to identify which plants need water
boolean shouldWater[15] = {false};

// Sets the digital channels to output and make sure the relays are all OFF
void setup() {
  // start the serial
  Serial.begin(9600);
  Serial.println("Initializing...");
  for(byte i = 0; i < (sizeof(solenoidPowerPin) / sizeof(solenoidPowerPin[0])); i++) 
  {
      pinMode(solenoidPowerPin[i], OUTPUT);  
      digitalWrite(solenoidPowerPin[i], relayOFF);        // All valves off
  }
  
  pinMode(moistureSensorsPowerPin, OUTPUT); 
  digitalWrite(moistureSensorsPowerPin, relayOFF);        // moisture sensor OFF
  pinMode(pumpPowerPin, OUTPUT); 
  digitalWrite(pumpPowerPin, relayOFF);                // Pump Off

  pinMode(waterLevelSensorPowerPin, OUTPUT); 
  digitalWrite(waterLevelSensorPowerPin, relayOFF);    // water level sensor OFF


}
 
void loop() {
  // Turn sensors on and give them some time to stablilize
  digitalWrite(moistureSensorsPowerPin, HIGH); 
  delay(3000);
  
  // Check the moisture
  for(byte i = 0; i < (sizeof(moistureSensorPin) / sizeof(moistureSensorPin[0])); i++) 
  {
    // The moisture number is counter intuitive. A big number indicates dry ground
    Serial.print("Sampling moisture from sensor ");
    Serial.print(i, DEC);
    Serial.println();
    int moistureAveraged = sampleMoisture(i);
    Serial.print("Sampled moisture avg on " + plant[i] + " is: ");
    Serial.print(moistureAveraged);
    Serial.println();
    if (moistureAveraged > startWatering[i]){
      Serial.println("Moisture is low in " + plant[i] + " needs watering");
      // Set a flag for this plant, we will water all at the same time later
      shouldWater[i] = true; 
    }else if (moistureAveraged < stopWatering[i]) {
      Serial.println("Moisture is high in " + plant[i] + " do NOT need watering");
      // remove the flag
      shouldWater[i] = false; 
    }
  }
  
  // Do the watering
  for(byte i = 0; i < (sizeof(shouldWater) / sizeof(shouldWater[0])); i++) 
  {
    if (shouldWater[i] == true) {
      Serial.println("Watering plant " + plant[i] + " needs watering");
      digitalWrite(solenoidPowerPin[i], relayON);           // First open valve
      digitalWrite(pumpPowerPin, relayON);                  // Then turn pump on
      delay(wateringTime[i]);                               // Water for the defined period
      digitalWrite(solenoidPowerPin[i], relayOFF); // Close the valve
    }
  }

  // Finally turn everything off before sleeping until next cycle
  digitalWrite(pumpPowerPin, relayOFF);                     // Pump off
  digitalWrite(moistureSensorsPowerPin, LOW);               // Moisture sensor off

  int sleepTimeInMin = sleepTime/1000/60; 
  Serial.print( "Global wait (minutes): ");
  Serial.print(sleepTimeInMin, DEC);
  Serial.println();
  Serial.print("Sleep time (ms): ");
  Serial.print(sleepTime);
  Serial.println();
  
  delay(sleepTime);
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
