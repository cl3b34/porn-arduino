/*
 * Arduino code to control 16 channel relay with Arduino UNO
 * 
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


const int controlPin[16] = {2,3,4,5,6,7,8,9,10,11,12,A0,A1,A2,A3,A4}; // define pins

const int triggerType = LOW;// your relay type
int loopDelay = 1000;// delay in loop
int tmpStat =1;


void setup() {
  for(int i=0; i<16; i++)
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

  for(int i=0; i<16; i++)
  {

   channelControl(i,tmpStat,200);// turn each relay ON for 200ms

    
  }
  if(tmpStat)
  {
    tmpStat=0;
  }else{
    tmpStat=1;
  }
 Serial.println("===============");
 //channelControl(6,1, 2000); // turn relay 7 ON for 2 seconds
 //channelControl(6,0, 5000); // turn relay 7 OFF for 5 seconds
 //channelControl(9,1, 3000); // turn relay 10 OFF for ever

 delay(loopDelay);// wait for loopDelay ms
          
}

/*
 * funciton: channelControl
 * turns ON or OFF specific relay channel
 * @param relayChannel is integer value channel from 0 to 15
 * @param action is 1 for ON or 0 for OFF
 * @param t is time in melisecond
 */
void channelControl(int relayChannel, int action, int t)
{
  int state =LOW;
  String statTXT =" ON";
  if(triggerType == LOW)
  {    
    if (action ==0)// if OFF requested
    {
      state = HIGH;
      statTXT = " OFF";
    }
    digitalWrite(controlPin[relayChannel], state);
    if(t >0 )
    {
      delay(t);
    }
       Serial.print ("Channel: ");
       Serial.print(relayChannel); 
       Serial.print(statTXT);
       Serial.print(" - "); 
       Serial.println(t);        
  }else{
    if (action ==1)// if ON requested
    {
      state = HIGH;     
    }else{
      statTXT = " OFF";    
    }
    digitalWrite(controlPin[relayChannel], state);
    if(t >0 )
    {
      delay(t);
    }
       Serial.print ("Channel: ");
       Serial.print(relayChannel); 
       Serial.print(statTXT);
       Serial.print(" - "); 
       Serial.println(t);    
  }

}

 
