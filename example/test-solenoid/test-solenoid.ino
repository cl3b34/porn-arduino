const int relayON = LOW;                  //  Our relay activates when it gets a LOW signal from arduino
const int relayOFF = HIGH;                //  Our relay activates when it gets a LOW signal from arduino
int solenoidPowerPin = 10;           // Digital pin providing power to the solenoids in the plant


void setup() {
    // start the serial
    Serial.begin(9600);
    Serial.println("Initializing...");
    pinMode(solenoidPowerPin, OUTPUT);  
    digitalWrite(solenoidPowerPin, relayOFF);        // valve off
}
  

void loop() {
  // put your main code here, to run repeatedly:
      Serial.println("Solenoid ON");
    digitalWrite(solenoidPowerPin, relayON);        // valve on
    delay(5000);
      Serial.println("Solenoid OFF");
    digitalWrite(solenoidPowerPin, relayOFF);        // valve off
        delay(5000);



}
