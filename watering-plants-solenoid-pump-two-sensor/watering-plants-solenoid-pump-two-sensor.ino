 /*
 Automatic plant watering with solenoid valves and pump.
 Goes to one plant at the time, reads a plant moisture, if its below StartwateringPlant moisture it will set a flag that this plant is dry and it will water for the WateringtimePlant period.
 Then it stops watering and goes on to the next plant in line an does the same, when its finished with the last plant the program waits globalWaittime before going further.
 When a plant starts watering, it will keep watering until StopwateringPlant is reached.

 When adding and removing plants one need to edit: numberOfPlants, StopwateringPlant, StopwateringPlant, SensorpinPlant, SolenoidoutputPlant and WateringtimePlant
 
 Use defined output pin to power measurement sensor to maximize moisture measuement sensor life so its turned off when not in use.

solenoid - sensor
2 - 8
3 - 9 
4 - 10
5 - 11
6 - 12

20190821 - Clean up logging
20190813 - Gave some plants away, got new plants
20180308 - Espada de sao jorge leaves are getting yellow? Diminish total amount of humidity. Give 5s of water at a time to plants in big pots

*/
 
//Define warning LED
//#define led 15 // Not currently in use.

//Define power pin for all powering all measurement devices. This is used with an P-channel mosfet so it turns output LOW when measuring.
//#define sensorMeasurementpoweron 9

// Define power pin for activating the relay to the pump
#define pumpPowerOn 13

//Define how many measurement one takes average of, and wait time between each sample
#define averageMeasuringsensor 10 // How many samples
#define averageWaittimesensor 300 // Wait time between each sample
#define globalWaittime 8000000 // Wait time after checked all plants and watered every plant. 4000000 = 66 minutes. 8.000.000 = 133 minutes (time is in miliseconds)

//Plants setup
#define numberOfPlants 5 // Enter how many plants you have  

// Add a value to each array when adding a new plant
String plants[numberOfPlants] = {"Abacaxi Pequeno", "Espada de Sao Jorge", "Tamarindeiro", "Limoeiro Grande", "Abacaxi Grande"}; 
int sensorPower[numberOfPlants] = {8,9,10,11,12}; // each sensor is controlled individually. Probably will change this later to be a single power signasl
int StartwateringPlant[numberOfPlants] = {700,1000,525,550,700}; // When to start watering
int StopwateringPlant[numberOfPlants] = {675,925,500,525,675}; // When to stop watering
int SensorpinPlant[numberOfPlants] = {A8,A9,A10,A11,A12}; // Analoge pin where each sensor is read
int SolenoidoutputPlant[numberOfPlants] = {2,3,4,5,6}; // Digital pin, power to the solenoid
int WateringtimePlant[numberOfPlants] = {2000,30000,15000,2000,2000}; // How long to water a plant for. Divide by 1000 and get seconds

//Setup ends here
 
// Store average of the sensor readings
int moisturePlant[numberOfPlants] = {0}; 
 
//the sum of all the samples we got on the moisture sensor
long int moistureSumPlant[numberOfPlants] = {0}; 

boolean keepWateringPlant[numberOfPlants] = {false};

int howmanyPlants = numberOfPlants - 1;

// Global wait time in minutes (for use in debug, not currently working)
int globalWaitMinutes = globalWaittime/1000/60;

bool debug = false;
 
void setup() {
  Serial.begin(9600);
  for(int x = 0; x <= howmanyPlants; x++)
    {
      log(debug, "Initializing plant " + plants[x]);
      pinMode(SolenoidoutputPlant[x], OUTPUT);  // Makes the outputs to the solenoids
      // Make sure the solenoids are closed
      log(debug, "turning solenoid for " + plants[x] + " OFF");
      digitalWrite(SolenoidoutputPlant[x], HIGH);
      log(debug, "Setting Sensor to output");
      pinMode(sensorPower[x], OUTPUT); 
    }
 
 // pinMode(led, OUTPUT);
 // pinMode(sensorMeasurementpoweron, OUTPUT);
  log(debug, "Initializing output and turning pump OFF");
  pinMode(pumpPowerOn, OUTPUT);
  digitalWrite(pumpPowerOn, HIGH);
}
 
void loop() {
  // Turn the sensors on
  log(debug, "Turning Sensors ON");
  for(int PlanteLoop = 0; PlanteLoop <= howmanyPlants; PlanteLoop++)
  {
    log(debug, "Sensor for " + plants[PlanteLoop] + " ON");
    digitalWrite(sensorPower[PlanteLoop], HIGH); 
  }
   delay(3000);

  for(int PlanteLoop = 0; PlanteLoop <= howmanyPlants; PlanteLoop++)
  {
      log(debug,"moisturePlant " + plants[PlanteLoop] + " " + moisturePlant[PlanteLoop] );
      log(debug,"StopWateringPlant " + plants[PlanteLoop] + " " + StopwateringPlant[PlanteLoop] );
      log(debug,"StartWateringPlant " + plants[PlanteLoop] + " " + StartwateringPlant[PlanteLoop] );
      moistureSamplingPlant(PlanteLoop);
      if (moisturePlant[PlanteLoop] >= StartwateringPlant[PlanteLoop]){
       Serial.println("moisture in " + plants[PlanteLoop] + " is LOW " + moisturePlant[PlanteLoop] + " WATERING. Start: " + StartwateringPlant[PlanteLoop] + " Stop: " + StopwateringPlant[PlanteLoop] );
       keepWateringPlant[PlanteLoop] = true; // Turn on watering plant
     }
    if (moisturePlant[PlanteLoop] < StopwateringPlant[PlanteLoop]) {
       Serial.println("moisture in " + plants[PlanteLoop] + " is HIGH " + moisturePlant[PlanteLoop] + " WATER OFF. Start: " + StartwateringPlant[PlanteLoop] + " Stop: " + StopwateringPlant[PlanteLoop] );
      keepWateringPlant[PlanteLoop] = false; // Turn off watering on plant
   //   digitalWrite(pumpPowerOn, HIGH);
      digitalWrite(SolenoidoutputPlant[PlanteLoop], HIGH);

    }
    if (keepWateringPlant[PlanteLoop] == true) {
      log(debug,"turning pump ON");
      digitalWrite(pumpPowerOn, LOW); // turn pump on
      log(debug,"moisture in " + plants[PlanteLoop] + " is low, watering");
      log(debug,"turning solenoid in " + plants[PlanteLoop] + " ON");
      digitalWrite(SolenoidoutputPlant[PlanteLoop], LOW); // Open valve
      delay(WateringtimePlant[PlanteLoop]); // How long it will water
      log(debug,"turning solenoid in " + plants[PlanteLoop] + " OFF");
      digitalWrite(SolenoidoutputPlant[PlanteLoop], HIGH);  // turn solenoid off, will leave the pump on until the end of the cycle
     // digitalWrite(pumpPowerOn, HIGH);
    }
    Serial.println();    
  }

  //Turn sensors off
   for(int PlanteLoop = 0; PlanteLoop <= howmanyPlants; PlanteLoop++)
  {
    digitalWrite(sensorPower[PlanteLoop], LOW); 
  }
  
  // Turn pump off
  log(debug,"turning pump OFF");
  digitalWrite(pumpPowerOn, HIGH);
  Serial.print("Waiting for ");
  Serial.print (globalWaitMinutes);
  Serial.println (" minutes");
  delay(globalWaittime);
}
 
void moistureSamplingPlant(int PlantNr) //Read value of plant moisture
{
// digitalWrite(sensorMeasurementpoweron, HIGH); // Turns output low when measuring
 //delay(3000); // Gives the sensor an chance to power up and get stable before starting measuring
  for(int i = 0; i < averageMeasuringsensor; i++)// sampling predefined.
  {
    delay(averageWaittimesensor);
    moistureSumPlant[PlantNr] = moistureSumPlant[PlantNr] + analogRead(SensorpinPlant[PlantNr]);
  }
//  digitalWrite(sensorMeasurementpoweron, LOW); // Turn off power to measurement probes
  moisturePlant[PlantNr] = moistureSumPlant[PlantNr] / averageMeasuringsensor; //Divide to get correct reading
 // Serial.println("Plant " + PlantNr + " of " + howmanyPlants + " is a " + plants[PlantNr] + " and have an moisture of " + moisturePlant[PlantNr]);
 log(debug,"Plant " + plants[PlantNr] + " have an moisture of " + moisturePlant[PlantNr]);
  moistureSumPlant[PlantNr] = 0; // Reset counting variable
}

void log(bool debug, String msg){
  if(debug){
    Serial.println(msg);
  }
 }
