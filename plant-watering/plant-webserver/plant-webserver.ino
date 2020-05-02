// https://www.instructables.com/id/noobs-guide-to-ESP8266-with-Arduino-Mega-2560-or-U/

const char SSID_ESP[] = "DNA-WLAN-2G-B310";        // wifi id
const char SSID_KEY[] = "97960799949";             // wifi pass

const byte numChars = 32;
//char receivedChars[numChars];
String receivedChars;

boolean newData = false;

void setup() {
  Serial2.begin(115200);
  Serial.begin(9600);
}


void loop() {

  sendATCommand("AT\r\n", "OK");
  showNewData();
//  receivedChars[0]='\0';
  delay(5000);
  sendATCommand("AT+RST", "WIFI GOT IP");
    showNewData();
//  receivedChars[0]='\0';
  delay(5000);

}


void sendATCommand(char * cmd, char * endOfXcg ){

  Serial2.println(cmd);
  receivedChars = Serial2.readStringUntil(endOfXcg);
  newData = true;

//  while (strcmp(endOfXcg, receivedChars) !=  0) {
//
//    newData = false;
//    recvWithEndMarker();
//    Serial.println("");
//  }
}


bool isEqual(char* str1, char* str2){
   if(strncmp(str1, str2, 32) == 0){
    Serial.println("strings match");
  }
}


bool areEqual(char* string1, char* string2) {
  return ((strlen(string1) == strlen(string2)) && (strstr(string1, string2)));
}



void clearInputBuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}


void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial2.available() > 0 && newData == false) {
    rc = Serial2.read();
//    Serial.println(rc);

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial2.available() > 0 && newData == false) {
    rc = Serial2.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    newData = false;
  }
}
