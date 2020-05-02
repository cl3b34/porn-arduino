
// https://www.instructables.com/id/noobs-guide-to-ESP8266-with-Arduino-Mega-2560-or-U/


const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void setup() {
  Serial2.begin(115200);
  Serial2.setTimeout(10000); // 10s
  Serial.begin(9600);
  connectToWiFi();
  httpGet();
}


void loop() {

}

const char SSID_WIFI[] = "DNA-WLAN-2G-B310";        // wifi id
const char PASS_WIFI[] = "97960799949";             // wifi pass
const char HOST[] =  "coolhead.fi";


// Keywords on the response
char keyword_OK[] = "OK";
char keyword_got_ip[] = "WIFI GOT IP";


boolean connectToWiFi(){
  Serial2.println("AT");  // Check if the module is responsive
  if(Serial2.find(keyword_OK)){
    Serial.println("AT Command OK");
    Serial2.println("AT+RST"); // Reset the module to start from a blank slate
    if(Serial2.find(keyword_got_ip)){
      Serial.println("RST OK");
      // Set mode to 'station' and connect
      Serial2.println("AT+CWMODE=1");
      String cmd = "AT+CWJAP=\"";
      cmd += SSID_WIFI;
      cmd += "\",\"";
      cmd += PASS_WIFI;
      cmd += "\"";
      Serial.println("Connecting");
      Serial2.println(cmd);
//      Serial.println(cmd);
      if(Serial2.find(keyword_OK)){
        Serial2.println("AT+CIFSR");
        Serial.println("ip address:");
        Serial.println(Serial2.readStringUntil(keyword_OK));
        // single connection mode
        Serial2.println("AT+CIPMUX=0");
        if(Serial2.find(keyword_OK)){
          Serial.println("ESP Ready");          
          return true;
        }
        Serial.println("failed setting up single connection mode"); 
      }
      Serial.println("failed connecting to the wi-fi");
    }
    Serial.println("failed reseting the board");
  }  
  Serial.println("failed getting attention (AT)");
  return false;
}



void httpGet(){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += HOST;
  cmd += "\",80";
  Serial.println(cmd);
  Serial2.println(cmd);
  if (Serial2.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  Serial2.print("AT+CIPSEND=");
  Serial2.println(cmd.length());
  if (Serial2.find(">"))
  {
    Serial.print(">");
  } else
  {
    Serial2.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    delay(1000);
    return;
  }
  Serial2.print(cmd);
  delay(2000);
  //Serial.find("+IPD");
  while (Serial2.available())
  {
    char c = Serial2.read();
    //dbgSerial.write(c);
    Serial.write(c);
    //if(c=='\r') dbgSerial.print('\n');
    if (c == '\r') Serial.print('\n');
  }
  //dbgSerial.println("====");
  Serial.println("====");
  delay(1000);
  
}




void clearInputBuffer() {
  while (Serial2.available() > 0) {
    Serial2.read();
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
