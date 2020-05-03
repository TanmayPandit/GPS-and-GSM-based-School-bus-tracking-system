#include <DFRobot_sim808.h>// including package for SIM 808
#include <SoftwareSerial.h>// including software serial library
#include <Stdlib.h>//including standard library
char inChar = 0;// defining all variables and methods
String str; // create a string
float schoolLat = 19.045433; // set reference latitude.
float schoolLon = 72.889351; //set reference longitude.
float thresholdDistance = 5.00; // set radius of geofence.
#define MESSAGE_LENGTH 160 // define message_length as 160.
char message[MESSAGE_LENGTH]; // create arrays for data storage.
int messageIndex = 0;
char MESSAGE[300];
char lat[12];
char lon[12];
char wspeed[12];
char distance[12];
char phone[16];
char datetime[24];
#define PIN_TX 10 //set pin 10 as TX
#define PIN_RX 11 //set pin 9 as RX
SoftwareSerial mySerial(PIN_TX,PIN_RX); // create a software serial object with TX and RX pins
DFRobot_SIM808 sim808(&mySerial);//create sim808 object.
void sendSMS();// define functions.
void getgps();;
void extractCoordinates();
void setup()
{
  mySerial.begin(9600);// setting baud rate at 9600 for serial transmission.
  Serial.begin(9600);

  //******** Initialize sim808 module *************
  while(!sim808.init())// Check if SIM 808 is intialized.
  {
      Serial.print("Sim808 init error\r\n");
      delay(1000);
  }
  delay(3000);

  Serial.println("SIM Init success");
      
  Serial.println("Init Success, please send SMS message to me!");
}

void loop()
{
  //*********** Detecting unread SMS ************************
   messageIndex = sim808.isSMSunread(); // send no. of unread messages to message index variable.

   //*********** At least, there is one UNREAD SMS ***********
   if (messageIndex > 0)
   { 
      readSMS();
       getGPS();
       extractCoordinates();
      //************* Turn off the GPS power ************
      sim808.detachGPS();

      Serial.println("Please send SMS message to me!");
   }
}

void readSMS()
{
  Serial.print("messageIndex: ");
  Serial.println(messageIndex);
  
  sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
             
  //***********In order not to full SIM Memory, is better to delete it**********
  sim808.deleteSMS(messageIndex);
  Serial.print("From number: ");
  Serial.println(phone);  
  Serial.print("Datetime: ");
  Serial.println(datetime);        
  Serial.print("Recieved Message: ");
  Serial.println(message);
}
void getGPS()
{ 
  while(!sim808.attachGPS())
  {
    Serial.println("Open the GPS power failure");// if GPS is not initialzed 
    delay(1000);
  }
  delay(3000);

  Serial.println("Open the GPS power success");// else 
    
  while(!sim808.getGPS())
  {
    
  }
  while(!sim808.getGPS())
    {
      
    }

  Serial.print(sim808.GPSdata.year); // display all gps data.
  Serial.print("/");
  Serial.print(sim808.GPSdata.month);
  Serial.print("/");
  Serial.print(sim808.GPSdata.day);
  Serial.print(" ");
  Serial.print(sim808.GPSdata.hour);
  Serial.print(":");
  Serial.print(sim808.GPSdata.minute);
  Serial.print(":");
  Serial.print(sim808.GPSdata.second);
  Serial.print(":");
  Serial.println(sim808.GPSdata.centisecond);
  Serial.print("latitude :");
  Serial.println(sim808.GPSdata.lat);
  Serial.print("longitude :");
  Serial.println(sim808.GPSdata.lon);
  Serial.print("speed_kph :");
  Serial.println(sim808.GPSdata.speed_kph);
  Serial.print("heading :");
  Serial.println(sim808.GPSdata.heading);
  Serial.println();
}

void extractCoordinates()
{
   float la = sim808.GPSdata.lat;
  float lo = sim808.GPSdata.lon;
  float ws = sim808.GPSdata.speed_kph;

   dtostrf(la, 4, 6, lat); //put float value of la into char array of lat. 4 = number of digits before decimal sign. 6 = number of digits after the decimal sign.
  dtostrf(lo, 4, 6, lon); //put float value of lo into char array of lon
  dtostrf(ws, 6, 2, wspeed);  //put float value of ws into char array of wspeed
   
   checkDist(la,lo); // call check distance function for geofence.
}
void checkDist(float la , float lo)
{
  float dist = HaverSine(schoolLat,schoolLon,la,lo);

  if (dist >= thresholdDistance ) // if bus is out of reference, display bus is out of area
  {
  float diest= dist-thresholdDistance;
  dtostrf(diest, 4, 6, distance);
  
    sprintf(MESSAGE, "outofrange:%s km bus is beyond threshold. Please contact the bus driver form emergency contact. ?%s?%s ",distance,lat, lon);
   
    Serial.println("Start to send message ...");
  
    Serial.println(MESSAGE);
    Serial.println(phone);
  
  sim808.sendSMS(phone,MESSAGE);

    delay(500);
    
    
  }  else   {
     sprintf(MESSAGE,"Bus is within threshold, no worries! ? %s? %s", lat, lon);
    Serial.println("Start to send message ...");
  
    Serial.println(MESSAGE);
    Serial.println(phone);
  
  sim808.sendSMS(phone,MESSAGE);
    
  }
}


float HaverSine(float lat1, float lon1, float lat2, float lon2)
{
  float ToRad = PI / 180.0;
  float R = 6371;   // radius earth in Km

  float dLat = (lat2-lat1) * ToRad;
  float dLon = (lon2-lon1) * ToRad; 

  float a = sin(dLat/2) * sin(dLat/2) +
    cos(lat1 * ToRad) * cos(lat2 * ToRad) * 
    sin(dLon/2) * sin(dLon/2); 

  float c = 2 * atan2(sqrt(a), sqrt(1-a)); 

  float d = R * c;
  return d;
}

