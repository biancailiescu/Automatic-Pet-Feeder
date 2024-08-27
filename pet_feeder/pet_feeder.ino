#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <EEPROM.h>

Servo servo_mot;
Time t;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DS3231 rtc(SDA, SCL);

int but1 = 2; //PORTII
int but3 = 5; //MANUAL
int but2 = 4; //ORA
int servo_pin = 6;


bool lastButton1State = HIGH;
bool lastButton2State = HIGH;
bool lastButton3State = HIGH;

int min_portions = 1;
int max_portions = 10;
int portions_crt = min_portions;

bool feed_active = true;
int Hour, Minute, Seconds;
int interval = 0;


int last_feed_hour = 1;
int next_feed_hour = 11;
int next_feed_minute = 37;
int next_feed_seconds = 30;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  servo_mot.attach(servo_pin);
  pinMode(but1, INPUT_PULLUP);
  pinMode(but2, INPUT_PULLUP);
  pinMode(but3, INPUT_PULLUP);
  rtc.begin();
 // last_feed_hour = EEPROM.read(2);  
  //next_feed_hour = EEPROM.read(3);  
}

void loop() {

  lcd.setCursor(0,0);
  lcd.print("Interval: ");
  lcd.print(interval);
  lcd.setCursor(0,1);
  lcd.print("Portii:");
  lcd.print(portions_crt);
  

  bool but1_state = digitalRead(but1);
  if(but1_state == LOW && lastButton1State == HIGH)
  {
    portions_crt++;
    if(portions_crt > max_portions)
    {
      portions_crt = min_portions;
    }
  lcd.clear();

  }
  lastButton1State = but1_state;

  bool but2_state = digitalRead(but2);
  if(but2_state == LOW && lastButton2State == HIGH)
  {
    interval++;
    if(interval > 23){
      interval = 1;
  }
  lcd.clear();
 
  } 
  lastButton2State = but2_state;                      

  
  bool but3_state = digitalRead(but3);
  if(but3_state == LOW && lastButton3State == HIGH)
  {
    feed_active = true;
  }
  lastButton3State = but3_state;
  
  t = rtc.getTime();                          
  Hour = t.hour;                              
  Minute = t.min;      
  Seconds = t.sec;                       
  Serial.print("Current Time: ");
  Serial.print(Hour);
  Serial.print(":");
  Serial.println(Minute);
  Serial.print(":");
  Serial.println(Seconds);
  

  Serial.print("Next Feeding Time: ");
  Serial.print(next_feed_hour);
  Serial.print(":");
  Serial.println(next_feed_minute);
  

 

  if( Hour== next_feed_hour && Minute == next_feed_minute && Seconds == next_feed_seconds){                 
      feed_active = true;
      last_feed_hour = Hour;
     // next_feed_hour = Hour + interval;
      next_feed_minute = Minute + interval;        
      if(next_feed_hour >= 23){                 
        next_feed_hour = next_feed_hour - 24;   
      }
     // EEPROM.write(2, last_feed_hour);          
      //EEPROM.write(3, next_feed_hour);     
      Serial.println(Hour);     

    }
    
  if(feed_active == true)
  {
    int i = 0;
    while(i < portions_crt){                        
      servo_mot.write(180); 
      delay(2000);
      servo_mot.write(0);
      lcd.clear();   
      lcd.setCursor(0,0);      
      lcd.print("FEEDING");       
            
      delay(1000);   
      i++;               
    }
    lcd.clear();
    feed_active = false;
  }

}
