#include <LiquidCrystal.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
 LiquidCrystal lcd(53,52,51,50,49,48);       // put your pin numbers here


void setup()
  {

    Serial.begin(9600);
    Serial.println("starting");
    
    lcd.begin(20, 4);                          // put your LCD parameters here
    lcd.setCursor(0, 0); // first line
    lcd.print("line 0");
    lcd.setCursor(0, 1); // 2nd line
    lcd.print("line 1"); 
    lcd.setCursor(0, 2); // 3rd line
    lcd.print("line 2");
    lcd.setCursor(0, 3); // 4th line
    lcd.print("line 3");
    delay(1000);
    write_from_loop(4);

  }


void write_from_loop(int lcdRows){
  int lcdLines[lcdRows];

  if(lcdRows == 2){
    lcdLines[0] = 0;
    lcdLines[1] = 2;
  } else if(lcdRows == 4){
    for(byte i=0; i<4; i++){
      lcdLines[i]=i;
    }
  }
  lcd.clear();
  lcd.print("size of lcdLines ");
  lcd.print(sizeof(lcdLines));
  delay(2000);
  lcd.clear();
  lcd.print("elements on lcdLines ");
  lcd.print(sizeof(lcdLines)/sizeof(lcdLines[0]));
  delay(2000);

    
}


void loop()
  { 
  }
