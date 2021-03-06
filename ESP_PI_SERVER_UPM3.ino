#include <ArduinoJson.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>


WiFiMulti WiFiMulti;
WiFiServer server(80);
WiFiUDP udp;
HardwareSerial testing(2); //Second Serial object for ESP32 

const char * udpAddress = "192.168.43.255";  // This should be broadcast address of network.
const int udpPort = 44444; // Port to listen on

void setup()
{
  Serial.begin(115200);
  delay(10);
  Wire.begin(17, 5);
  testing.begin(115200, SERIAL_8N1, 23, 22);
  // We start by connecting to a WiFi network
  WiFiMulti.addAP("Gaurav", "alphaman");

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
// Begining every object
  server.begin();
  delay(500);
  udp.begin(udpPort);
  
}

int flag1 = 0; char zx; int flag2 = 0;

void loop() {
  
  WiFiClient client = server.available();   // listen for incoming clients
  
  // UDP for giving out ESP IP to RPI.
    uint8_t buffer[50] = "hello Pi, ESP here!"; //send hello to server
    udp.beginPacket(udpAddress, udpPort);
    udp.write(buffer, 19);
    udp.endPacket();
    memset(buffer, 0, 50);  

  // Client interaction
  if (client) {                             // if you get a client,
    Serial.println("New Client.");
    int value = 0;
    String json, dson;
    int i = 0;
    int flag = 0;
    int t = 0;
    int r = 0;
    long roundsX; long roundsY; 

    while (client.connected()) {           // loop while the client's connected

      if (client.available() && flag == 0) {          // Verifying connection with PI(by sending Q,P)
        char d = client.read();
        if (d == 'Q') {
          client.write('P');
          Serial.println(d);
          flag1 = 1;
        }
      }

      while (!client.available());

       if (client.available() && flag1 == 1) {      // Checking whether its File or not
        char f = client.read();
        if (f == 'F') {
          flag = 1;
        }
        else {
          flag = 2;
        }
      }

      while (!client.available());

      if (client.available() && flag == 1) {        //Code for file execution
        int i = 0;
        while (1) {
          char c = client.read();
          dson = dson + c;
          if (c == '}') {
//            Serial.println(dson);

//      Unpacking JSON format
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(dson);
            if (!root.success()) {
              Serial.println("parseObject() failed");
            }
            int index = root["index"];
            const char* Gcode = root["GCode"];
            int hash_value = root["Hash_value"];
            int hash_try = hashing(Gcode);

            if (index == i && hash_try == hash_value) {     // If lines are not missing then excute this
              //      Serial.print(index);
              //      Serial.print("\t");
                    Serial.println(Gcode);
              //      Serial.print("\t");
              //      Serial.println(hash_value);

              testing.println(Gcode);       //Sending Gcode to GRBL
              delay(10);
              CNCResponse();                // Response from CNC
              dson = "";
              client.print('Y');            // Responding to PI that process is done
              i++;
            }
            else {
              client.print('N');
              dson = "";
            }
          }
          else if (c == '@')
          { flag1 = 1;
            break;
          }
          //        dson="";
          while (!client.available());
        }
      }


      else if (client.available() && flag == 2)  // Execute this if Gcode is Line not File
      {
        while (1)
        {
          if (client.available())
          {
            zx = client.read();
            json += zx;
            if (zx == '}')
            { break;
            }
          }
        }
        //        Serial.println(json);
        String Gcode = ConvJsonLine(json);
        Serial.println(Gcode);
        if(Gcode=="$$")
        { testing.println(Gcode);
        delay(10);
          String Resp=testing.readString();
          Serial.println(Resp);
          client.println(Resp);
          }
        else
      {  testing.println(Gcode);
        delay(10);
        CNCResponse();
        client.print('Y');}
        json = "";
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
  delay(10);
}

// function for getting hashing value(encryption)
int hashing(String val)
{ int hash = 0, i = 0;
  while (val[i] != '\0') {
    hash += int(val[i]) % 101;
    ++i;
  }
  return hash;
}

//Converting JSON line into GCOde
String ConvJsonLine(String json)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseObject() failed");
  }
  const char* Gcode = root["GCode"];
  return Gcode;
}

// Sending Question mark after some time and checks CNC state
void CNCResponse()
  { while (testing.available())
    {
      testing.read();
    }

    while (1)
    {
      testing.println('?');
      delay(10);
      while (testing.read() != '<');
      String response = "<";
      while (!testing.available());
      while (1)
      { char resp_char = testing.read();
        if (resp_char != '>')
        { response += resp_char;
        }
        else
        { response += '>';
          break;
        }
      }
      Serial.println(response);
      response = response.substring(0, 10);
      if (response.indexOf("Idle") != -1)
      {.
        break;
      }
      else if (response.indexOf("Run") != -1)
      {}
    }

  }

//function asking enocder value from STM 32 on request.

void EncoderValue()
{
  Wire.requestFrom(8, 11);   // request 6 bytes from slave device #8
int i;byte buf[8];
  for (i = 0; i <= 11; i++)  {
    buf[i] = Wire.read();

  }
  // Serial.println(buf[i],HEX);

  double valX = 0;          double valY1 = 0;   double  valY2 = 0;
  valX += buf[3] << 24;  valY1 += buf[7] << 24; valY2 += buf[11] << 24;
  valX += buf[2] << 16;  valY1 += buf[6] << 16; valY2 += buf[10] << 16;
  valX += buf[1] << 8;   valY1 += buf[5] << 8;  valY2 += buf[9] << 8;
  valX += buf[0];        valY1 += buf[4];       valY2 += buf[8];

  valX = (valX) / 100;     valY1 = (valY1) / 100; valY2 = (valY2) / 100;
//  Serial.print(valX); Serial.print("  "); Serial.print(valY1);Serial.print("  "); Serial.println(valY2);
  delay(1);

}

