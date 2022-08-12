
#include <ThingSpeak.h>
#include <WiFi.h>
const int analogIn = 34;
int RawValue= 0;
double Voltage = 0;
float tempC = 0;
float tempF = 0;
const int LM35 = A0;
//----------- Enter you Wi-Fi Details---------//
char ssid[] = ""; //SSID
char pass[] = ""; // Password
//-------------------------------------------//

WiFiClient  client;

unsigned long myChannelNumber = ; // Channel ID here
const int FieldNumber = 1;
const char * myWriteAPIKey = ""; // Your Write API Key here

void setup(){
Serial.begin(9600);
pinMode(14,OUTPUT);
pinMode(13,OUTPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}
 
void loop(){
 if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
RawValue = analogRead(analogIn);
Voltage = (RawValue / 4096.0) * 3300; // 5000 to get millivots.
tempC = Voltage * 0.1;
tempF = (tempC * 1.8) + 32; // conver to F
Serial.print("Raw Value = " ); // shows pre-scaled value
Serial.print(RawValue);
Serial.print("\t milli volts = "); // shows the voltage measured
Serial.print(Voltage,0); //
Serial.print("\t Temperature in C = ");
Serial.print(tempC,1);
Serial.print("\t Temperature in F = ");
Serial.println(tempF,1);
ThingSpeak.writeField(myChannelNumber, FieldNumber, tempC, myWriteAPIKey);
if(tempF>100){
delay(1000);
digitalWrite(13,HIGH);
delay(50);
digitalWrite(13,LOW);
delay(5000);
digitalWrite(14,HIGH);
delay(50);
digitalWrite(14,LOW);
}
delay(5000);

}
