#include <SoftwareSerial.h>

/*********************************************************************
 * MACROS
 */
//#define DEBUG_MODE
#define NORMAL_MODE

#define DEBUG_PRINT 1 // Enables print/println into Serial Port
#define BAUD 115200   // BAUD rate for both BT and Serial Port

#define BT_BROADCASTER  1
#define BT_OBSERVER     0
 
//#define NEW_PCB_DEVICE
//#define UART_FLIP

#ifdef NEW_PCB_DEVICE
// BT RESET PINs
#define BT_RESET_A  2 // A RESET
#define BT_RESET_B  7 // B RESET
// BT UART
#define BT_A_RX_PIN 3 // A RX
#define BT_A_TX_PIN 2
#define BT_B_RX_PIN 5 // B RX
#define BT_B_TX_PIN 4
#else 
  #ifndef UART_FLIP // original type
    // BT UART
    #define BT_A_RX_PIN 2 // A RX // Broadcastor
    #define BT_A_TX_PIN 3
    #define BT_B_RX_PIN 4 // B RX // Observer
    #define BT_B_TX_PIN 5
  #else
    // BT UART
    #define BT_A_RX_PIN 3 // A RX
    #define BT_A_TX_PIN 2
    #define BT_B_RX_PIN 5 // B RX
    #define BT_B_TX_PIN 4
  #endif

#endif

// Buzzer
#define ALARM_PIN  10
//#define BUZZER_PASSIVE
#define BUZZER_ACTIVE

//Vibator
#define VIBATOR_PIN 6

//Hallsensor
#define HALLSENSOR_PIN 7

/*********************************************************************
 * CONSTANTS 
 */
#define ALARM_WINDOW 30 // 3sec

// my module list
#define MY_ID        "00011"

// AT Command
#define ADMIN_MODE    "ADMIN MODE"
// Observer
#define AT_AIN        "AT+A1"
#define AT_AIN_ACK    "A+xxxxxxOK"
#define AT_AOUT       "AT+A0"
#define AT_AOUT_ACK   "A+OK"
#define AT_SLEEP      "AT+D0"
#define AT_SLEEP_ACK  "D+OK"

#define AT_C          "AT+C1"
#define AT_C_STOP     "AT+C0"
#define AT_C_ACK      "C+-xxxOK"

#define AT_E_ENABLE   "AT+E1"
#define AT_E_ACK      "E+OK"

#define AT_INCIRCLE_I "AT+I?"
#define AT_INCIRCLE_S "AT+IS"
#define AT_INCIRCLE_E "AT+IE"
#define AT_INCIRCLE_C "AT+IC"
#define AT_I_ACK      "I+OK"

#define AT_ISTART     "SRART"
#define AT_IEND       "End  "

// Broadcaster
#define AT_L_CLEAR    "AT+LCLEAR"
#define AT_L_ACK      "L+OK"
#define AT_S          "AT+Sxxxxx"
#define AT_S_ACK      "S+OK"
#define AT_W_READY    "AT+WREADY"
#define AT_W_ACK      "W+OK"

#define AT_C_DONE     "AT+CDONE "
#define AT_C_DONE_ACK "C+OK"

#define ALARM_TIMEOUT       500
#define AT_TIMEOUT          2000
#define AT_C_TIMEOUT        10000
#define AT_INCIRCLE_TIMEOUT 30000
/*********************************************************************
 * LOCAL VARIABLES & LOCAL FUNCTIONS
 */

bool LED_Toggle = HIGH;
bool Hall_state = LOW;

bool Hall_inform_state = false;
bool cmd_inform_state = false;
unsigned int alarm_tic_count = 0;
unsigned int tic_count = 0;
unsigned char alarm_state = 0;
unsigned char alarm_cnt = 0;


// SoftwareSerial(RX, TX) 형식으로 블루투스 모듈과 교차하여 연결
SoftwareSerial BT_B_Serial(BT_A_RX_PIN,BT_A_TX_PIN);  // Broadcaster
SoftwareSerial BT_O_Serial(BT_B_RX_PIN,BT_B_TX_PIN); // Observer
//SoftwareSerial BT_BROADCAST_Serial(BT_A_RX_PIN, BT_A_TX_PIN);

char *uptime() // Function made to millis() be an optional parameter
{
 return (char *)uptime(millis()); // call original uptime function with unsigned long millis() value
}

char *uptime(unsigned long milli)
{
  static char _return[32];
  unsigned long secs=milli/1000, mins=secs/60;
  unsigned int hours=mins/60, days=hours/24;
  milli-=secs*1000;
  secs-=mins*60;
  mins-=hours*60;
  hours-=days*24;
  sprintf(_return,"Uptime %d days %2.2d:%2.2d:%2.2d.%3.3d", (byte)days, (byte)hours, (byte)mins, (byte)secs, (int)milli);
  return _return;
}

void setup()
{

#ifdef NEW_PCB_DEVICE
  pinMode(BT_RESET_A, OUTPUT); 
  digitalWrite(BT_RESET_A, LOW);
  pinMode(BT_RESET_B, OUTPUT);
  digitalWrite(BT_RESET_B, LOW);
#endif

  Serial.begin(BAUD); // 컴퓨터와의 시리얼 통신 초기화
  BT_O_Serial.begin(BAUD); // 블루투스 모듈 초기화
  BT_B_Serial.begin(BAUD); // 블루투스 모듈 초기화

  delay(1000);

#ifdef NEW_PCB_DEVICE
  digitalWrite(BT_RESET_A, HIGH);
  digitalWrite(BT_RESET_B, HIGH);
#endif

  pinMode(HALLSENSOR_PIN, INPUT); 
  pinMode(ALARM_PIN, OUTPUT);

    // uptime
  Serial.println("UPTIME");
  Serial.println("");
  Serial.print("Zero = ");
  Serial.println(uptime(0));
  Serial.print("End of first day = ");
  Serial.println(uptime(86400000-1));
  Serial.print("Second day 86400000 = ");
  Serial.println(uptime((unsigned long)24*60*60*1000)); // 24hours * 60min * 60sec * 1000mili = 86400000
  Serial.print("Last millis() complete day = ");
  Serial.println(uptime((unsigned long) -1 + ((byte)(4294967295/86400000)) * 86400000 )); // -1 + 49 * 86400000 = 4233599999
  Serial.print("Absolutely MAX millis() unsigned long is 4294967295 = ");
  Serial.println(uptime(4294967295));
  Serial.println("If millis() > 4294967295 then Arduino reset millis() counter to zero!!!");
  Serial.println("");

}

void active_buzz_siren(int period)
{
  if (period <= 200)
    period == 200;
  digitalWrite(ALARM_PIN, HIGH);
  delay(period);
  digitalWrite(ALARM_PIN, LOW);
}


bool sendCommand(SoftwareSerial * BTModuleSerial, char *command, String ret, int timeout)
{
  char inChar;
  unsigned long timer;
//  int timeout = 2000;
  char reply[50];
  int i = 0;
  String temp;
  int ret_length = ret.length();
  BTModuleSerial->begin(BAUD);
  while(BTModuleSerial->available()) BTModuleSerial->read(); // Clear input buffer
  timer = millis();

  Serial.println(command);
  BTModuleSerial->write(command);
  //wait some time
  
  while (millis()-timer < timeout) {
      if((String)command==AT_C) {
        Hall_state = digitalRead(HALLSENSOR_PIN);
        if (Hall_state) {
          Serial.println(AT_C_STOP);
          BTModuleSerial->write(AT_C_STOP);
          return false;
        }
      }
    if (BTModuleSerial->available()) {
      inChar = BTModuleSerial->read();
      temp += inChar;
      i++;
      if (i==ret_length)
        break;

    }
  }

  if ((String)command==AT_C)
  {
      Serial.println(ret_length);
      Serial.println(temp);
      return true;
  }
  else
    if (temp.equals(ret))
    {
      Serial.println(temp);
      return true;
    }

  Serial.println(temp);
  Serial.println("false");
  return false;
}

bool In_Circle_Mode = false;
unsigned long previousMillis = 0; 

void loop()
{
  unsigned char i = 0;
  char inChar;

  char ret_length;
  bool ret = false;
  char ack[10];
  char cmd = 0;
  char device_uuid[5];

  unsigned long timer;
//  int timeout = 3000;
  timer = millis();
  
#ifdef DEBUG_MODE
  // 블루투스 모듈 → 아두이노 → 시리얼 모니터
  if (BT_B_Serial.available())
    Serial.write(BT_B_Serial.read());
    
  // 시리얼 모니터 → 아두이노 → 블루투스 모듈
  if (Serial.available())
    BT_B_Serial.write(Serial.read());
#endif

#ifdef NORMAL_MODE
  Hall_state = digitalRead(HALLSENSOR_PIN);
  if (Hall_state == LOW) {
    // **Admin mode**
    digitalWrite(LED_BUILTIN, HIGH); 
    if (!Hall_inform_state) {
      Hall_inform_state = true;
      In_Circle_Mode = false;
      
      Serial.println(ADMIN_MODE);
      active_buzz_siren(50);
      // 1. Broadcaster send to Admin what WREADY response 
      ret = sendCommand(&BT_B_Serial, AT_W_READY, AT_W_ACK, AT_TIMEOUT);
      if (ret)
        Serial.println("TRUE");
      else
        Serial.println("FALSE");

      ret = false;
      // 2. To observer receive from Admin message wait. 
      BT_O_Serial.begin(BAUD); // Connect Arduino to BT observer with UART
      Serial.println(AT_AIN);
      BT_O_Serial.write(AT_AIN);
      
    }

    i=0;
    ret_length = 10;
    
    while (BT_O_Serial.available() > 0) {
      inChar = BT_O_Serial.read();
      ack[i] = inChar;
      i++;
      if (i==ret_length)
      {
        //Serial.println(ack);
        ret = true;
        
      }
      //Serial.write(BT_O_Serial.read());
    }

    if (ret)
    {
      Serial.println("&TRUE");
      cmd = ack[2];
      memset(device_uuid,0x00,sizeof(device_uuid));
      memcpy(device_uuid,&ack[3],sizeof(device_uuid));
      Serial.println(cmd);
      Serial.println(device_uuid);

      switch(cmd)
      {
        case 'R':
          if ((String)device_uuid == MY_ID)
          {
            In_Circle_Mode = true;
            //ret = sendCommand(&BT_O_Serial, AT_INCIRCLE_S, AT_I_ACK, AT_TIMEOUT);
            Serial.println("In Circle mode");
            active_buzz_siren(100);
          }
/*
          // 2. To observer receive from Admin message wait. 
          BT_O_Serial.begin(BAUD); // Connect Arduino to BT observer with UART
          ret = false;
          timer = millis();
          while (millis()-timer < AT_INCIRCLE_TIMEOUT) {
            i=0;
            ret_length = 5;
            while (BT_O_Serial.available() > 0) {
              inChar = BT_O_Serial.read();
              ack[i] = inChar;
              i++;
              if (i==ret_length)
              {
                Serial.println(ack);
                ret = true;
              }
              //Serial.write(BT_O_Serial.read());
            }

            if (ret)
            {
              Serial.println("&&TRUE");
              cmd = ack[2];
              memset(device_uuid,0x00,sizeof(device_uuid));
              memcpy(device_uuid,&ack[3],sizeof(device_uuid));
              Serial.println(cmd);
              Serial.println(device_uuid);
              if ((String)device_uuid == AT_ISTART)
              {
                 ret = sendCommand(&BT_O_Serial, AT_INCIRCLE_S, AT_I_ACK, AT_TIMEOUT);
                 Serial.println(MY_ID);
                 active_buzz_siren(50);
              }
              else if ((String)device_uuid == AT_IEND)
              {
                 ret = sendCommand(&BT_O_Serial, AT_INCIRCLE_E, AT_I_ACK, AT_TIMEOUT);
                 Serial.println(MY_ID);
                 In_Circle_Mode = false;
                 active_buzz_siren(200);
              }
              else
              {
                active_buzz_siren(50);
              }
            }
          }
*/
        break;
        case 'S':
          active_buzz_siren(50);
        /*
          ret = sendCommand(&BT_O_Serial, AT_SLEEP, AT_SLEEP_ACK, AT_TIMEOUT);
          Serial.println(ret);
          if (ret)
            active_buzz_siren(50);
          else {
            active_buzz_siren(50);
            active_buzz_siren(200);
          }
        */
        break;
        case 'C':
          // Broadcaster send to Admin what Cal Progress response 
          //ret = sendCommand(&BT_B_Serial, AT_C_PROGRESS, AT_C_ACK, AT_TIMEOUT);

           unsigned char cal_cnt = 0;
           ret = false;
           while (millis()-timer < 10000){
            if (BT_O_Serial.available() > 0)
            {
              inChar = BT_O_Serial.read();
              if (inChar == '+')
              {
                cal_cnt++;
                active_buzz_siren(20);
                if (cal_cnt>=4)
                {
                  ret = true;
                  break;
                }
              }
            } // if (BTSerial.available()>0)
          }

          if (ret)
          {
            ret = sendCommand(&BT_B_Serial, AT_C_DONE, AT_C_DONE_ACK, AT_TIMEOUT);
            active_buzz_siren(20);
            active_buzz_siren(50);
            Serial.println("TRUE");
            delay(1000);
            
            // 1. Broadcaster send to Admin what WREADY response 
            ret = sendCommand(&BT_B_Serial, AT_W_READY, AT_W_ACK, AT_TIMEOUT);
            if (ret)
              Serial.println("TRUE");
            else
              Serial.println("FALSE");
      
            ret = false;
            // 2. To observer receive from Admin message wait. 
            BT_O_Serial.begin(BAUD); // Connect Arduino to BT observer with UART
//            Serial.println(AT_AIN);
//            BT_O_Serial.write(AT_AIN);

            //Hall_state = digitalRead(HALLSENSOR_PIN);
            //if (Hall_state == LOW)
            //  Hall_inform_state = false;
            
          }
          else
          {
            active_buzz_siren(200);
            Serial.println("FALSE");
          }

        break;
        case 'I':
            if (In_Circle_Mode)
              if ((String)device_uuid == AT_ISTART)
              {
                 //ret = sendCommand(&BT_O_Serial, AT_INCIRCLE_S, AT_I_ACK, AT_TIMEOUT);
                 Serial.println(AT_ISTART);
                 active_buzz_siren(50);
              }
              else if ((String)device_uuid == AT_IEND)
              {
                 //ret = sendCommand(&BT_O_Serial, AT_INCIRCLE_E, AT_I_ACK, AT_TIMEOUT);
                 Serial.println(AT_IEND);
                 In_Circle_Mode = false;
                 active_buzz_siren(200);
              }
              else
              {
                active_buzz_siren(50);
              }
        break;
        case 'D':
        break;
        default:
        
        break;
      }
    }
  }
  else {  
    // ** Normal mode **
    digitalWrite(LED_BUILTIN, LOW); 

    // Broadcaster
    if (Hall_inform_state)
    {
      ret = sendCommand(&BT_O_Serial, AT_AOUT, AT_AOUT_ACK, AT_TIMEOUT);
      ret = sendCommand(&BT_B_Serial, AT_L_CLEAR, AT_L_ACK, AT_TIMEOUT);
      if (ret)
      {
        Serial.println("*TRUE");
        Hall_inform_state = false;
      }
      else
        Serial.println("*FALSE");
      Hall_inform_state = false;
      active_buzz_siren(50);
      BT_O_Serial.begin(BAUD); // Connect Arduino to BT observer with UART

    }

    while (millis()-timer < ALARM_TIMEOUT){
      if (BT_O_Serial.available() > 0)
      {
        inChar = BT_O_Serial.read();
        if (inChar == '+')
        {
          if (tic_count > ALARM_WINDOW)
          {
            tic_count = 0;
            Serial.print(inChar);
          }
          else
          {
            alarm_state = true;
            alarm_cnt++;
            digitalWrite(LED_BUILTIN, HIGH);
            digitalWrite(ALARM_PIN, HIGH);
            Serial.print(inChar);
    
            break;
          }
        }
      } // if (BTSerial.available()>0)
    } // while (scanning_once_done)
    //end the string
    
    if(alarm_state>0)
    {
      alarm_tic_count++; 
      tic_count = 0;   
    }
    else
    {
      tic_count++;
    }
  
    // 1 min
    if (tic_count>600)
      tic_count = 600;
  
    if (alarm_tic_count==2)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(ALARM_PIN, HIGH);
    }
    
    if (alarm_tic_count>=3)
    {
      alarm_tic_count=0;
      alarm_cnt = 0;
      tic_count=0;
      alarm_state = 0;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(ALARM_PIN, LOW);
    }
    
  }
#endif

}
