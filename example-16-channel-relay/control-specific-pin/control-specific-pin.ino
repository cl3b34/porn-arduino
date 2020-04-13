

/*
 * Arduino code to control 16 channel relay with Arduino UNO
 * Control more than 1 relay
 * Written by Ahmad Shamshiri for Robojax.com on Sunday Oct 08, 2018 
 * at 10:35 in Ajax, Ontario, Canada
 * Watch video instruction for this code: https://youtu.be/Q9aBI4ELKC4
 * 
 * This code is "AS IS" without warranty or liability. Free to be used as long as you keep this note intact.* 
 * This code has been download from Robojax.com
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


const int controlPin[13] = {2,3,4,5,6,7,8,9,10,11,12,13,23}; // define pins

const int triggerType = LOW;// your relay type
int loopDelay = 1000;// delay in loop
int tmpStat =1;


void setup() {
  for(int i=0; i<13; i++)
  {
    pinMode(controlPin[i], OUTPUT);// set pin as output
    if(triggerType ==LOW){
      digitalWrite(controlPin[i], HIGH); // set initial state OFF for low trigger relay
    }else{
       digitalWrite(controlPin[i], LOW); // set initial state OFF for high trigger relay     
    }
  }
  
  Serial.begin(9600);// initialize serial monitor with 9600 baud
}

void loop() {

 Serial.println("Solenoid on");
 digitalWrite(controlPin[1], LOW); 
 Serial.println("Pump on");
 digitalWrite(controlPin[0], LOW); 
 delay(5000);
 Serial.println("Pump off");
 digitalWrite(controlPin[0], HIGH); 
 delay(500);
 Serial.println("solenoid off");
 digitalWrite(controlPin[1], HIGH); 
 delay(2000);         
}
  
