#define pumpPowerOn 13
#define sensorMeasurementpoweron 8
#define SolenoidoutputPlant 2

void setup() {
    Serial.begin(9600);

  // put your setup code here, to run once:
  Serial.println("turning solenoid OFF");
  pinMode(SolenoidoutputPlant, OUTPUT);
  digitalWrite(SolenoidoutputPlant, HIGH);

 // Serial.println("turning sensor OFF");
  pinMode(sensorMeasurementpoweron, OUTPUT);
  digitalWrite(sensorMeasurementpoweron, LOW);


  Serial.println("turning pump OFF");
  pinMode(pumpPowerOn, OUTPUT);
  digitalWrite(pumpPowerOn, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("turning pump ON");
      digitalWrite(pumpPowerOn, LOW); // turn pump on

// wait 
Serial.println("waiting 5S");
          delay(5000);

// solenoid on
 Serial.println("turning solenoid ON");
      digitalWrite(SolenoidoutputPlant, LOW); 
         

// wait 10s
Serial.println("Water for 5S");
delay(5000);

//Serial.println("solenoid Off");
digitalWrite(SolenoidoutputPlant, HIGH);
Serial.println("Pump Off");
digitalWrite(pumpPowerOn, HIGH);

Serial.println("Wait 5S ang go again");
delay(5000);


}
