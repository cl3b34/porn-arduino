void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  char str1[64] = "test with spaces";
  char str2[32] = "test with spaces";

  Serial.println(str1);
  Serial.println(str2);

  if (str1 == str2) Serial.println("Equals");
  else Serial.println("Different");

  if (strcmp(str1, str2) == 0) Serial.println("Equals");
  else Serial.println("Different");

//  if (strcoll(str1, str2)) Serial.println("Equals"); // <-- this won't compile for Arduino UNO { why ??? }
//  else Serial.println("Different");

  if (memcmp(str1, str2, sizeof(str2))== 0) Serial.println("Equals");
  else Serial.println("Different");

  if (areEqual(str1, str2)) Serial.println("Equals");
  else Serial.println("Different");

  strcpy(str1, "test+with+spaces");
  strcpy(str2, "test with spaces");

  Serial.println();
  Serial.println(str1);
  Serial.println(str2);

  if (str1 == str2) Serial.println("Equals");
  else Serial.println("Different");

  if (strcmp(str1, str2) == 0) Serial.println("Equals");
  else Serial.println("Different");

  if (memcmp(str1, str2, sizeof(str2)) == 0) Serial.println("Equals");
  else Serial.println("Different");

  if (areEqual(str1, str2)) Serial.println("Equals");
  else Serial.println("Different");

  strcpy(str1, "test+with++++++++++++++++++++++++++++++++++++spaces");
  strcpy(str2, "test with spaces");

  Serial.println();
  Serial.println(str1);
  Serial.println(str2);

  if (str1 == str2) Serial.println("Equals");
  else Serial.println("Different");

  if (strcmp(str1, str2) == 0) Serial.println("Equals");
  else Serial.println("Different");

  if (memcmp(str1, str2, sizeof(str2))== 0) Serial.println("Equals");
  else Serial.println("Different");

  if (areEqual(str1, str2)) Serial.println("Equals");
  else Serial.println("Different");

}

void loop() {
  // put your main code here, to run repeatedly:

}


bool areEqual(char* string1, char* string2) {
  return ((strlen(string1) == strlen(string2)) && (strstr(string1, string2)));
}
