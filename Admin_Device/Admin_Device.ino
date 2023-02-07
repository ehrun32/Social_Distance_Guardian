/*************************************************
   BT_Admin
   Version: Lavia_Admin V4.0
   Date: Aug 8, 2020
   Author: Arun Cheriakara
   Company: Lavia Solutions Inc.
   
 *************************************************/
/*
 *   note  frequency
  c     262 Hz
  d     294 Hz
  e     330 Hz
  f     349 Hz
  g     392 Hz
  a     440 Hz
  b     494 Hz
  C     523 Hz
 */
// Tones frequency Hz
#define NOTE_C4  262
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247

//BAUD rate
#define BAUD 115200   // BAUD rate for both BT and Serial Port

// BT UART BROADCASTER
#define BTB_TXD_PIN 4
#define BTB_RXD_PIN 5

// BT UART OBSERVER
#define BTO_TXD_PIN 6
#define BTO_RXD_PIN 7

//Hallsensor
#define HALLSENSORPIN 3

//Vibrator
#define VIBRATOR 12

//Buttons
#define REGIS_BUTTON 8
#define INCIRCLE_BUTTON 9
#define CALIB_BUTTON 10
#define SLEEP_BUTTON 11
#define PHONE_BUTTON 12

// number of devices that can be in-circeld
#define MAX_INCIRCLE_DEVICE 5

// Buzzer 
#define ALARM_PIN  2


#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4);

SoftwareSerial BT_B(BTB_TXD_PIN, BTB_RXD_PIN); //TX|RX , Broadcaster
SoftwareSerial BT_O(BTO_TXD_PIN, BTO_RXD_PIN); //TX|RX , Observer

//Vectors for musical note and arrow
uint8_t note[8]  = {0x02, 0x03, 0x02, 0x0e, 0x1e, 0x0c, 0x00, 0x00};
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};

// Alarm
int numTones = 8;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

//hallsensor
int hallstate = 0; 
int ledPin =  13; 

//register id
String reg_device[MAX_INCIRCLE_DEVICE];
int reg_device_count=0;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD);  // set the data rate for the SoftwareSerial port
  
  BT_B.begin(BAUD);
  BT_O.begin(BAUD);
  
  pinMode(ledPin, OUTPUT);      
  pinMode(HALLSENSORPIN, INPUT);
  pinMode(ALARM_PIN, OUTPUT); 
  noTone(ALARM_PIN);
  pinMode(VIBRATOR,OUTPUT);
  
  while (BT_B.available()) BT_B.read(); // Clear input buffer
  
  lcd.begin();  //Init the LCD
  lcd.backlight(); //Activate backlight
  lcd.createChar(0, note);    //Create the note symbol
  lcd.createChar(1, arrow);   //Create the arrow symbol
  lcd.home();                 //Home the LCD

  pinMode(REGIS_BUTTON, INPUT_PULLUP);
  pinMode(INCIRCLE_BUTTON, INPUT_PULLUP);
  pinMode(SLEEP_BUTTON, INPUT_PULLUP);
  pinMode(CALIB_BUTTON, INPUT_PULLUP);
  pinMode(PHONE_BUTTON, INPUT_PULLUP);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(0);  
  lcd.print(" LAVIA SOLUTIONS ");
  lcd.write(0);
  lcd.setCursor(0,1);  
  lcd.print("  Social Distance   ");
  lcd.setCursor(0,2);  
  lcd.print("     Guardian    ");
  
  delay(2000);
} //void setup()


String ack; 
char inchar = 0;
int state = 0;
int ret_check=0;
bool first_display_flag=false;
bool first_admin_flag=false;
String myid;

void loop() {
  // put your main code here, to run repeatedly:
  hallstate = digitalRead(HALLSENSORPIN);
   
  if (hallstate == LOW) { //if magnet is placed
    if(first_admin_flag==false) 
    {
      digitalWrite(ledPin, HIGH); 
      first_display();
      ret_check=0;
      first_display_flag = false;
      bool whof= false;
      
      BT_O.begin(BAUD);
      while (BT_O.available()) BT_O.read(); // Clear input buffer

      unsigned long timer;
      int timeout = 3500; // 3 seconds
      timer = millis(); 

      Serial.println("AT+W");
      BT_O.write("AT+W");  // to BT observer

      String ret="";
      
      while(millis()-timer < timeout){
       while (BT_O.available()&& ret_check<1)   // WHO ARE YOU
       {
        char byteIn = BT_O.read(); // from BT observer
        //String ret;
       
        String myidadded;
        ret += byteIn;
        // Serial.println(byteIn);
        ret += BT_O.readString();  // from BT observer
        //Serial.println(ret.substring(2));
        Serial.print("The device ID is: ");
        myid=ret.substring(2,7);
        Serial.println(myid);
        Serial.println(ret);
        int ret_length;
        ret_length=ret.length();
        //Serial.println(ret_length);
        String devicecmd;
        devicecmd=ret;
        
          if(byteIn=='W')
          {

         if(ret_length>=9)
         {   
           if (ret == devicecmd)
            {
              lcd.clear();
              lcd.write(1);
              lcd.print("DEVICE ID IS");
              lcd.setCursor(14,0);
              lcd.print(myid); 
              Serial.println("DEVICE FOUND");
              whoalarm();
              ret_check ++;
              whof= true;
              break;  
            }
         }
        
        else if (ret == "W+NOK")
            {
              Serial.println("NOPE");
              //Serial.println(ret);
              ret_check ++;
              whof = false;
              break;
            }
          }//if(byteIn=='W')
          else
          {
            Serial.println("No response");
            ret_check ++;
            //Serial.println(ret);
            whof = false;
            break;
          }
        } //while (BT_O.available()&& ret_check<1)
      } //while(millis()-timer < timeout)

       
   if (whof == false)
    {
      // Serial.println(ret);
      //ret_check ++;
      fail(); //Alarm sound
      lcd.clear();
      lcd.write(1);
      lcd.print("NO DEVICE");
      Serial.println(">NO DEVICE");
      delay(1000);
    }
      Serial.println("ADMIN MODE"); 
      //first_display();

} //if(first_admin_flag==false) 

     
 if (!digitalRead(INCIRCLE_BUTTON))  //incircle button
  {
    incircle(myid);
    updateMenu();
    delay(100);
    while(!digitalRead(INCIRCLE_BUTTON));
  }
  
  if (!digitalRead(SLEEP_BUTTON))  // sleep button
  {
    sleep(myid);
    updateMenu();
    delay(100);
    while(!digitalRead(SLEEP_BUTTON));
  }
  
   if (!digitalRead(CALIB_BUTTON))  //calib button
  {
    calibration(myid); 
    updateMenu();
    delay(100);
    while(!digitalRead(CALIB_BUTTON));
  }
  
   if (!digitalRead(REGIS_BUTTON))  // register button
  {
    registration(myid);
    updateMenu();
    delay(100);
    while(!digitalRead(REGIS_BUTTON));
  }

#if 1
    while(ret_check==1)  //mainmenu
      {
        updateMenu();
        ret_check++;
      }
      first_admin_flag=true;
#endif 

   
  
}// if (hallstate == LOW)
  
else 
 {
  if (first_display_flag==false)
    {
      
       
        digitalWrite(ledPin, LOW);
        Serial.println("---------------------"); 
        Serial.println("NO MAGNET DETECTED");
        Serial.println("---------------------");
        phone_display();
        first_display_flag = true;
        first_admin_flag=false;
  
   
        
      }
  if (!digitalRead(REGIS_BUTTON))  // register button
  {
    phone_register();
    phone_display();
    delay(100);
    while(!digitalRead(REGIS_BUTTON));
  }

  if (!digitalRead(SLEEP_BUTTON))  // sleep button
  {
    sleep(myid);
    phone_display();
    delay(100);
    while(!digitalRead(SLEEP_BUTTON));
  }
      
 }
} // loop()



void updateMenu() //menu
 {  
  if(state == LOW)
    {
        lcd.clear();
        lcd.write(1);
        lcd.print("REGISTER");
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.print("IN-CIRCLE");
        lcd.setCursor(0,2);
        lcd.write(1);
        lcd.print("CALIBRATION");
        lcd.setCursor(0,3);
        lcd.write(1);
        lcd.print("SLEEP MODE");
        Serial.println("---------------------");
        Serial.println(" REGISTER");
        Serial.println(" IN-CIRCLE");
        Serial.println(" CALIBRATION");
        Serial.println(" SLEEP MODE");
        Serial.println("---------------------");
    }
 }

void incircle(String deviceid)  //incircle
{
  //Serial.println(deviceid);
  BT_B.begin(BAUD);
  char in_length = 0;
  char inchar = 0; 
  unsigned long timer;
  int timeout = 3000; // 3 seconds
  timer = millis();
  // String devicecmd;
  // devicecmd= "AT+I";
  //devicecmd+=deviceid;
  bool ret = false;
  
  while(BT_B.available()) BT_B.read(); // Clear input buffer

   Serial.println("AT+ISTART"); 
   BT_B.write("AT+ISTART");
   delay(10);
   ack="";
   
   while(millis()-timer < timeout)
    {
      if (BT_B.available())
      {
         inchar = BT_B.read();
         
         ack += inchar;
         in_length++;
         if (in_length>=4)
           if(ack.equals("I+OK"))
           {
             Serial.println("OK messeage received"); 
              lcd.clear();
              lcd.write(1);
              lcd.print("STARTING");
              Serial.println(">STARTING");
              //success();  // Alarm sound 
              delay(500);
              command(myid);
              
              ret = true;
              break;
             
           }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NOT RECEIVED");
      //vibration();
      fail(); //Alarm sound
      delay(1500);
    }

} //void incircle()

void command(String deviceid) //finding how many devices
{
  BT_B.begin(BAUD);
  bool ret = false;
  int i;

    for(i=0; i<reg_device_count; i++)
    {
      //Serial.println(i);
      char in_length = 0;
      char inchar = 0; 
      unsigned long timer;
      int timeout = 3000; // 3 seconds
      timer = millis();
      String devicecmd;
      devicecmd= "AT+I";
      devicecmd+=reg_device[i];
      //delay(500);
           
      while(BT_B.available()) BT_B.read(); // Clear input buffer
    
       Serial.println(devicecmd);  
       BT_B.print(devicecmd);
       
  
       ack="";
   
   while(millis()-timer < timeout)
    {
     if (BT_B.available())
      {
         inchar = BT_B.read();
         
         ack += inchar;
         in_length++;
         if (in_length>=4)
           
           if(ack.equals("I+OK"))
           {
             Serial.println("OK messeage received"); 
              lcd.clear();
              lcd.write(1);
              lcd.print("IN-CIRCLING DEVICE ");
              lcd.setCursor(10,1);  
              lcd.print(i+1);
              Serial.print(">IN-CIRCLING DEVICE ");
              Serial.println(i+1);
              success();  // Alarm sound 
              delay(100);
              
              ret = true;
             break;
             
           }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)

} //for(i=0; i<reg_device_count; i++)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NO DEVICE REGISTERED");
      //vibration();
      fail(); //Alarm sound
      delay(1500);
    }
 //delay(1000);
stopincircle();
delay(500);
reg_device_count=0;
//Serial.println(reg_device_count);
  
} //void command( String deviceid)

void stopincircle() // stop function
{ 
  BT_B.begin(BAUD);
  char in_length = 0;
  char inchar = 0; 
  unsigned long timer;
  int timeout = 3000; // 3 seconds
  timer = millis();
  bool ret = false;
  
  while(BT_B.available()) BT_B.read(); // Clear input buffer
    
   Serial.println("AT+IEnd  "); 
   BT_B.write("AT+IEnd  ");

   ack="";
   
   while(millis()-timer < timeout)
    {
      if (BT_B.available())
      {
         inchar = BT_B.read();
         ack += inchar;
         in_length++;
         if (in_length>=4)
         
           if(ack.equals("I+OK"))
           {
              lcd.clear();
              lcd.write(1);
              lcd.print("ALL DONE");
              Serial.println(">ALL DONE");
              //success();
              delay(500);
              clearfunction();
              ret = true;
              break;
             
           }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NOT RECEIVED");
      fail(); //Alarm sound
      delay(1500);
    }
} //void stopincircle()




void sleep(String deviceid) //sleep function
{
  BT_B.begin(BAUD);
  char in_length = 0;
  char inchar = 0; 
  unsigned long timer;
  int timeout = 3000; // 3 seconds
  timer = millis();
  String devicecmd;
  devicecmd= "AT+S";
  devicecmd+=deviceid;
  bool ret = false;
  
  while(BT_B.available()) BT_B.read(); // Clear input buffer

   Serial.println(devicecmd);  
   BT_B.print(devicecmd);

   ack="";
   
   
   while(millis()-timer < timeout)
    {
      if (BT_B.available())
      {
         inchar = BT_B.read();
         ack += inchar;
         in_length++;
         if (in_length>=4)
         
           if(ack.equals("S+OK"))
           {
             Serial.println("OK messeage received"); 
              lcd.clear();
              lcd.write(1);
              lcd.print("SLEEPING");
              Serial.println(">SLEEPING");
              reg_device_count=0;
              success();  // Alarm sound 
              delay(1000);
              ret = true;
             break;
             
           }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NOT RECEIVED");
       reg_device_count=0;
      //vibration();
      fail(); //Alarm sound
      delay(1500);
    }
 clearfunction();
} // void sleep()

void calibration(String deviceid)
{
  BT_B.begin(BAUD);
  char in_length = 0;
  char inchar = 0; 
  unsigned long timer;
  int timeout = 3000; // 3 seconds
  timer = millis();
  String devicecmd;
  devicecmd= "AT+C";
  devicecmd+=deviceid;
  bool ret = false;
  
  while(BT_B.available()) BT_B.read(); // Clear input buffer

   Serial.println(devicecmd);  
   BT_B.print(devicecmd);

   ack="";
   
   while(millis()-timer < timeout)
    {
      if (BT_B.available())
      {
         inchar = BT_B.read();
         ack += inchar;
         in_length++;
         if (in_length>=4)
           if(ack.equals("C+OK"))
           {
             Serial.println("OK messeage received"); 
              lcd.clear();
              lcd.write(1);
              lcd.print("CALIBRATION IN");
              lcd.setCursor(0,1);
              lcd.print(" PROGRESS");
              Serial.println(">CALIBRATING");
              reg_device_count=0;
              delay(5000);
              ackcalib();
              //success();  // Alarm sound 
              
              ret = true;
             break;
             
           }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NOT RECEIVED");
      reg_device_count=0;
      //vibration();
      fail(); //Alarm sound
      delay(1500);
    }
   

} //void calibration()

#if 1
void ackcalib()
{
      int ret_check;
      ret_check=0;
      //first_display_flag = false;
      bool cali= false;
      int in_length=0;
      
      BT_O.begin(BAUD);
      while (BT_O.available()) BT_O.read(); // Clear input buffer

      unsigned long timer;
      int timeout = 4000; // 3 seconds
      timer = millis(); 

      Serial.println("AT+C");
      BT_O.write("AT+C");  // to BT observer

       String ret ="";
       while(millis()-timer < timeout){
       //String ret ="";
       while (BT_O.available()&& ret_check<1)   //calibration ack
       {
        char byteIn = BT_O.read(); // from BT observer
        ret+= byteIn;
        in_length++;
        if (in_length>=6)
        {
          Serial.println(ret); 
          if (ret == "C+DONE")
          {
           
            Serial.println("DONE");
            ret_check ++;
            success();
            cali= true;
            delay(1000);
            break;  
          }
       else
          {
            Serial.println("No response");
            ret_check ++;
            cali = false;
            break;
          }
        }
      } //while (BT_O.available()&& ret_check<1)
    } //while(millis()-timer < timeout)

       
   if (cali == false)
    {
      // Serial.println(ret);
      //ret_check ++;
      lcd.clear();
      lcd.write(1);
      lcd.print("CALIBRATION FAILED");
      Serial.println(">CALIBRATION FAILED");
      fail(); //Alarm sound
      delay(1000);
    }
  clearfunction();
}

#endif


void registration(String deviceid)
{
  BT_B.begin(BAUD);
  char in_length = 0;
  char inchar = 0; 
  unsigned long timer;
  int timeout = 3000; // 3 seconds
  timer = millis();
  String devicecmd;
  devicecmd= "AT+R";
  devicecmd+=deviceid;
  bool ret = false;
  
 while(BT_B.available()) BT_B.read(); // Clear input buffer

   Serial.println(devicecmd);  
   BT_B.print(devicecmd);

   ack="";
   
   while(millis()-timer < timeout)
    {

      if (BT_B.available())
      {
         inchar = BT_B.read();
         
         ack += inchar;
         in_length++;
         if (in_length>=4)
           if(ack.equals("R+OK"))
           {
            if(reg_device_count< MAX_INCIRCLE_DEVICE)
            {
              Serial.println("OK messeage received"); 
              lcd.clear();
              lcd.write(1);
              lcd.print("REGISTERING");
              Serial.print(">REGISTERING DEVICE ");
              reg_device[reg_device_count] = deviceid;
              reg_device_count++;
              Serial.println(reg_device_count);
              Serial.print("The Device ID is ");
              Serial.println(deviceid);
              success();  // Alarm sound 
              ret = true;
              delay(1000);
              break;
           }
         }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NOT RECEIVED");
      //vibration();
      fail(); //Alarm sound
      delay(1500);
    }
}//void registration()

void clearfunction() // stop function
{ 
  BT_B.begin(BAUD);
  char in_length = 0;
  char inchar = 0; 
  unsigned long timer;
  int timeout = 3000; // 3 seconds
  timer = millis();
  bool ret = false;
  
  while(BT_B.available()) BT_B.read(); // Clear input buffer
    
   Serial.println("AT+LCLEAR"); 
   BT_B.write("AT+LCLEAR");

   ack="";
   
   while(millis()-timer < timeout)
    {
      if (BT_B.available())
      {
         inchar = BT_B.read();
         ack += inchar;
         in_length++;
         if (in_length>=4)
         
           if(ack.equals("L+OK"))
           {
              lcd.clear();
              lcd.write(1);
              lcd.print("CLEARING");
              Serial.println(">CLEARING");
              //success();
              delay(500);
              ret = true;
              break;
             
           }
           else
           {
              ret = false;
              break;
           }
      } //if (BT_B.available())
    } //   while(millis()-timer < timeout)


    if (ret == false)
    {
      lcd.clear();
      lcd.write(1);
      lcd.print("NOT RECEIVED");
      Serial.println(">NOT RECEIVED");
     // fail(); //Alarm sound
      delay(1500);
    }
} //void clearfunction() 


void phone_register()
{
      int ret_check;
      ret_check=0;
      //first_display_flag = false;
      bool phone= false;
      
      
      
      BT_O.begin(BAUD);
      while (BT_O.available()) BT_O.read(); // Clear input buffer

      unsigned long timer;
      int timeout = 4000; // 3 seconds
      timer = millis(); 

      Serial.println("AT+W");
      BT_O.write("AT+W");  // to BT observer
     
       while(millis()-timer < timeout){
       String ret ="";
       while (BT_O.available()&& ret_check<1)   //calibration ack
       {
        char byteIn = BT_O.read(); // from BT observer
        ret+= byteIn;
        ret += BT_O.readString(); 
        Serial.print("The device ID is: ");
        myid=ret.substring(2,7);
        Serial.println(myid);
        Serial.println(ret);
        int ret_length;
        ret_length=ret.length();
        //Serial.println(ret_length);
        String devicecmd;
        devicecmd=ret;
        
        
    if(byteIn=='W')
      {
        if (ret_length>=9)
        {
          
          if (ret == devicecmd)
          {
            lcd.clear();
            lcd.write(1);
            lcd.print("REGISTERING PHONE");
            Serial.println("REGISTERING PHONE");            
            reg_device[reg_device_count] = myid;
            reg_device_count++;
            Serial.println(reg_device_count);
            ret_check ++;
            success();
            phone= true;
            delay(1000);
            break;  
          }
        }
          
          else if (ret == "W+NOK")
            {
              Serial.println("NOPE");
              //Serial.println(ret);
              ret_check ++;
              phone = false;
              break;
            }
          }//if(byteIn=='W')
       else
          {
            Serial.println("No response");
            ret_check ++;
            phone = false;
            break;
          }
        
      } //while (BT_O.available()&& ret_check<1)
    } //while(millis()-timer < timeout)

       
   if (phone == false)
    {
      
      lcd.clear();
      lcd.write(1);
      lcd.print("NO PHONE FOUND");
      Serial.println(">NO PHONE FOUND");
      fail(); //Alarm sound
      delay(1000);
    }
}

void  first_display()  //magnet detected
{
 lcd.clear();
 lcd.print("       MAGNET  ");
 lcd.setCursor(0, 1);
 lcd.print("      DETECTED   ");
 delay(1000);

}

void phone_display()  //display when no magnet is found
{
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.write(0);  
  lcd.print(" PHONE MODE ");
  lcd.write(0);
  lcd.setCursor(0,1);  
  lcd.write(1);
  lcd.print("REGISTER PHONE");
  lcd.setCursor(0,2);  
  lcd.write(1);
  lcd.print("SLEEP MODE");
  Serial.println("---------------------");
  Serial.println(" REGISTER PHONE");
  Serial.println(" SLEEP MODE");
  Serial.println("---------------------");
}

void success()
{                             // sound 
  for(int hz = 440; hz < 1000; hz+=25)
  tone(ALARM_PIN,440,200);
  delay(300);
  noTone(ALARM_PIN);
  tone(ALARM_PIN,494,500);
  delay(300);
  noTone(ALARM_PIN);
  tone(ALARM_PIN,523,300);
  delay(200);
  noTone(ALARM_PIN);
}

#if 1
void fail()     // sound
{
  for(int hz = 200; hz < 1000; hz+=25)
  {
    tone(ALARM_PIN, hz, 50);
    delay(5);
  }
  loopF(3,12,20);
  loopR(12,3,20);
  // Whoop down
  for(int hz = 1000; hz > 200; hz-=25)
  {
    tone(ALARM_PIN, hz, 50);
    delay(5);
  }
  noTone(ALARM_PIN);
}
#endif

// SOME EXTRA FUNCTIONS OTHER THAN THE SIREN TONES
void loopF(int spin,int epin,int dela)
{    //loopF can blink the led in forward direction so spin must be lower than epin.
  for(int i=spin;i<=epin;i++)
  {
    delay(dela);
    if(spin==epin)
    {
      spin=3;
      epin=12;
    }
  }
}
void loopR(int epin,int spin,int dela)
{      //loopR can blink the led in reverse/backward direction so epin must be lower than spin.
  for(int i=epin;i>=spin;i--)
  {
    delay(dela);
    if(spin==epin)
    {
      spin=3;
      epin=12;
    }
  }
}

void whoalarm()
{
  for (int i = 0; i < numTones; i++)
  {
    tone(ALARM_PIN, melody[i]);
    delay(160);
  }
  noTone(ALARM_PIN);
}

void vibration()
{
  digitalWrite(VIBRATOR,HIGH);
  delay(2000);
  digitalWrite(VIBRATOR,LOW);
  delay(600);
}
