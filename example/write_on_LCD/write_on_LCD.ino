#include <LiquidCrystal.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
 LiquidCrystal lcd(12,11,5,4,3,2);       // put your pin numbers here
 LiquidCrystal lcd1(53,52,51,50,49,48);       // put your pin numbers here


void setup()
  {

   
    lcd1.begin(20, 4);                          // put your LCD parameters here
    for (char i=47; i<127; i++)                // send 80 consecutive displayable characters to the LCD
      {
        lcd1.print(i);
        delay(100);                            // this delay allows you to observe the addressing sequence
      }
      
    lcd.begin(16, 2);                          // put your LCD parameters here
    for (char i=47; i<127; i++)                // send 80 consecutive displayable characters to the LCD
      {
        lcd.print(i);
        delay(100);                            // this delay allows you to observe the addressing sequence
      }


  }


void loop()
  { 
  }
