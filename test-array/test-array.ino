void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);// initialize serial monitor with 9600 baud


  int myValues[] = {123, 456, 789};

// this for loop works correctly with an array of any type or size
for (byte i = 0; i < (sizeof(myValues) / sizeof(myValues[0])); i++) {
  // do something with myValues[i]
  Serial.print("running " + i );
  Serial.print("myValues size" + sizeof(myValues));
}

}

void loop() {
  // put your main code here, to run repeatedly:

}
