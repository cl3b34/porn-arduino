/*
 Automatic water with solenoid valves.
 This goes to one plant at the time.
 So it works like this: Reads a plant moisture, if its below StartwateringPlant moisture it will set a flag that this plant is dry, and then it will water for the WateringtimePlant period.
 Then it stops watering and goes on to the next plant i line an does the same, when its finished with the last plant the program waits globalWaittime before going further.
 This time as long as one plant has started watering, it will keep watering untill StopwateringPlant is reached.

 When adding and removing plants one need to edit: numberOfPlants, StopwateringPlant, StopwateringPlant, SensorpinPlant, SolenoidoutputPlant and WateringtimePlant
 
 Also it will print the value of the soil moisture, and if its watering is turned on on serial 9600 baud. 0 means its not watering and 1 is that is watering.

 Use defined output pin to power measurement sensor to maximize moisture measuement sensor life so its turned off when not in use.
*/
 
/* Define Plants, this list is just to remember yourself what is what.
plant0 is an  Espada de sao jorge  Start: 800, stop 775, water amount: 3000
plant1 is an  Jukka                Start: 800, stop 775, water amount: 3000
plant2 is an  tamarindeiro        Start: 425, stop 400, water amount: 3000
plant3 is an  Limoeiro grande      Start: 450, stop 425, water amount: 3000
plant4 is an  Abacaxi grande     Start: 450, stop 425, water amount: 3000

solenoid - sensor
2 - 8
3 - 9 
4 - 10
5 - 11
6 - 12

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

int sensorPower[numberOfPlants] = {8,9,10,11,12}; // each sensor is controlled individually. Probably will change this later to be a single power signasl
int StartwateringPlant[numberOfPlants] = {1000,850,525,550,550}; // Add a new value for each plant, this is when it starts watering
int StopwateringPlant[numberOfPlants] = {975,825,500,525,525}; // Add a new value for each plant, this is when it stop watering.
int SensorpinPlant[numberOfPlants] = {A8,A9,A10,A11,A12}; // Analoge pin, Add a new value for each plant
int SolenoidoutputPlant[numberOfPlants] = {2,3,4,5,6}; // Digital pin, Add a new value for each plant.
int WateringtimePlant[numberOfPlants] = {30000,8000,15000,2000,2000}; // Divide by 1000 and get seconds. Add a new value for each  plant.

//Setup ends here
 
//the value read from each moisture sensor
int moisturePlant[numberOfPlants] = {0}; // Value to store average reading in
 
//the sum of the 30 times sampling
long int moistureSumPlant[numberOfPlants] = {0}; // This variable is used to store the sum of all sampled value
 
boolean keepWateringPlant[numberOfPlants] = {false};

int howmanyPlants = numberOfPlants - 1;
 
void setup() {
  Serial.begin(9600);
  for(int x = 0; x <= howmanyPlants; x++)
    {
      pinMode(SolenoidoutputPlant[x], OUTPUT);  // Makes the outputs to the solenoids
      // Make sure the solenoids are closed
      Serial.println("turning solenoid OFF");
      digitalWrite(SolenoidoutputPlant[x], HIGH);
      Serial.println("Setting Sensors to output");
      pinMode(sensorPower[x], OUTPUT); 
    }
 
 // pinMode(led, OUTPUT);
 // pinMode(sensorMeasurementpoweron, OUTPUT);
  pinMode(pumpPowerOn, OUTPUT);
  // make sure pump is off
  Serial.println("turning pump OFF");
  digitalWrite(pumpPowerOn, HIGH);

}
 
void loop() {

  // Turn the sensors on
        Serial.println("Turning Sensors ON");
  for(int PlanteLoop = 0; PlanteLoop <= howmanyPlants; PlanteLoop++)
  {
    digitalWrite(sensorPower[PlanteLoop], HIGH); 
  }
   delay(3000);

  for(int PlanteLoop = 0; PlanteLoop <= howmanyPlants; PlanteLoop++)
  {
    Serial.print("moisturePlant ");
      Serial.println(moisturePlant[PlanteLoop]);
      Serial.print("StopWateringPlant ");
      Serial.println(StopwateringPlant[PlanteLoop]);
            Serial.print("StartWateringPlant ");
      Serial.println(StartwateringPlant[PlanteLoop]);
    moistureSamplingPlant(PlanteLoop);
    if (moisturePlant[PlanteLoop] > StartwateringPlant[PlanteLoop]){
      Serial.println("moisture is low, watering");
      keepWateringPlant[PlanteLoop] = true; // Turn on watering plant
    }
    if (moisturePlant[PlanteLoop] < StopwateringPlant[PlanteLoop]) {
      Serial.println("Plant is wet, STOP watering");
      keepWateringPlant[PlanteLoop] = false; // Turn off watering on plant
   //   digitalWrite(pumpPowerOn, HIGH);
      digitalWrite(SolenoidoutputPlant[PlanteLoop], HIGH);

    }
    if (keepWateringPlant[PlanteLoop] == true) {
      Serial.println("turning pump ON");
      digitalWrite(pumpPowerOn, LOW); // turn pump on
      Serial.println("moisture is low, watering");
      Serial.println("turning solenoid ON");
      digitalWrite(SolenoidoutputPlant[PlanteLoop], LOW); // Open valve
      delay(WateringtimePlant[PlanteLoop]); // How long it will water
      Serial.println("turning solenoid OFF");
      digitalWrite(SolenoidoutputPlant[PlanteLoop], HIGH);  // turn pump and solenoid off
     // digitalWrite(pumpPowerOn, HIGH);
    }
  }

  //Turn sensors off
   for(int PlanteLoop = 0; PlanteLoop <= howmanyPlants; PlanteLoop++)
  {
    digitalWrite(sensorPower[PlanteLoop], LOW); 
  }
  
  // Turn pump off
  Serial.println("turning pump OFF");
  digitalWrite(pumpPowerOn, HIGH);
  Serial.println("Global wait");
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
  Serial.print("Plant ");
  Serial.print(PlantNr);
  Serial.print(" of ");
  Serial.print(howmanyPlants);
  Serial.print(" have an moisture of " );
  Serial.print(moisturePlant[PlantNr]);
  Serial.print(". Plant watering status is ");
  if(keepWateringPlant[PlantNr] == true){
   Serial.print("Dry");
  }else{
    Serial.print("Wet");
  }
 // Serial.print(keepWateringPlant[PlantNr]);
  Serial.println();
  moistureSumPlant[PlantNr] = 0; // Reset counting variable
}
