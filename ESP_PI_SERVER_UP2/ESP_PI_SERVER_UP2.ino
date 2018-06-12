#include <ArduinoJson.h>

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

HardwareSerial testing(2);

void setup()
{
  Serial.begin(115200);
  delay(10);
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

  server.begin();
  delay(500);

  //////Jason Setup
}



void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("New Client.");
    int value = 0;
   String json;
   int i = 0;
   int flag = 0;
    //     while (client.connected()) {           // loop while the client's connected
    //            client.print("Y");
    //            if (client.available()) {              // if there's bytes to read from the client,
    // make a String to hold incoming data from the client
    //        char d = client.read();
    //        Serial.println(d);
    //        if (d == 'Y') {
    while (client.connected()) {           // loop while the client's connected
      //              if(flag==0)
      //              {client.print("P");}
      if (client.available() && flag == 0) {           // if there's bytes to read from the client,
        char d = client.read();
        if (d == 'Q') {
          client.write('P');
          Serial.println(d);
          flag = 1;
        }
      }

      if (client.available() && flag == 1) {
        char c = client.read();
        // read a byte, then
        json = json + c;
         if (c == '}') {
          Serial.println(json);
          DynamicJsonBuffer jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(json);
          if (!root.success()) {
            Serial.println("parseObject() failed");
          }
          int index = root["index"];
          const char* Gcode = root["GCode"];
          int hash_value = root["Hash_value"];
          int hash_try = hashing(Gcode);

          if (index == i && hash_try == hash_value) {
            Serial.print(index);
            Serial.print("\t");
            Serial.print(Gcode);
            Serial.print("\t");
            Serial.println(hash_value);
            if (1)
            { testing.print(Gcode);
              //                    Serial.println("Delivered");
              delay(50);
              while (1)
              {
                testing.println( "?" );      /////GRBL code
                String pos = testing.readString();
                String state = pos.substring(1, 5);
                //                        Serial.println(pos);
                Serial.println(pos);
                if (state == "Idle")
                { break;
                }
              }
            }
            json = "";
            client.print('Y');
            //                  Serial.println("Reached");
            i++;
          }
          else {
            client.print('N');
            json = "";
          }
        }
      }
    }


    //    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
  delay(10);
}


int hashing(String val)
{ int hash = 0, i = 0;
  while (val[i] != '\0') {
    hash += int(val[i]) % 101;
    ++i;
  }
  return hash;
}



