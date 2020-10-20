
//int solenoidPowerPin[] = {3, 4, 5 , 6 , 7 , 8 , 9 , 10 , 11, 12, 13, 22, 23, 24, 25};       // Digital pin providing power to the solenoids in the plant
int solenoidPowerPin[] = { 8 , 9 , 10 , 11};       // Digital pin providing power to the solenoids in the plant


void setup() {
  Serial.begin(9600);
  Serial.println("initializing");
  for (byte i = 0; i < (sizeof(solenoidPowerPin) / sizeof(solenoidPowerPin[0])); i++)
  {
    pinMode(solenoidPowerPin[i], OUTPUT);
    digitalWrite(solenoidPowerPin[i], HIGH);        // All valves off
  }
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);        // All valves off
}

void loop() {
 
   for (byte i = 0; i < (sizeof(solenoidPowerPin) / sizeof(solenoidPowerPin[0])); i++)
  {
      Serial.print("Testing pin "); Serial.println(solenoidPowerPin[i]);
      digitalWrite(solenoidPowerPin[i], LOW);           // First open valve
      digitalWrite(2, LOW);                            // Then turn pump on
      Serial.println(solenoidPowerPin[i]);

      delay(5000);                               // Water for the defined period
      digitalWrite(solenoidPowerPin[i], HIGH); // Close the valve
      digitalWrite(2, HIGH);                            // Then turn pump on
  }

}
