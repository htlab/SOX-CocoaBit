#include <Nefry.h>
#include <Time.h>
#include <TimeLib.h>
#include "ThinSOXClient.h"

int sensorValue;
int sensorValueHistory;
int spanValue;

//String Event, SecretKey;
String SOXHostname = "sox.ht.sfc.keio.ac.jp";
int SOXPort = 5222;
String SOXNodeName = "testNode";
String SOXTransducerId = "temperature";

int counter = 0;

void setup() {
  Nefry.setProgramName("CocoaBit_SOX");
  Nefry.setConfHtmlStr("SOXHostname", 0); //Nefry DataStoreのタイトルを指定
  Nefry.setConfHtmlStr("SOXPort", 1);    //Nefry DataStoreのタイトルを指定
  Nefry.setConfHtmlStr("SOXNodeName", 2);
  Nefry.setConfHtmlStr("SOXTransducerId", 3);

  //SOX Server hostname
  SOXHostname = Nefry.getConfStr(0);     //Nefry DataStoreからデータを取得
  Nefry.println(String("SOXHostname: [") + SOXHostname + "]");

  //SOX Server portnumber
  SOXPort = atoi(Nefry.getConfStr(1));         //Nefry DataStoreからデータを取得
  Nefry.println(String("SOXPort: ") + SOXPort + "]");

  //SOX NodeName to publish data
  SOXNodeName = Nefry.getConfStr(2);
  Nefry.println(String("SOXNodeName: ") + SOXNodeName + "]");
  
  //SOX Node's target transducer ID
  SOXTransducerId = Nefry.getConfStr(3);
  Nefry.println(String("SOXTransducerId: ") + SOXTransducerId + "]");
  }

void loop() {
  sensorValue = analogRead(A0); //Read analog data
  char valueChar[32];
  itoa(sensorValue, valueChar, 10);

  Nefry.print("Read sensor data. value = "); Nefry.println(sensorValue);

  int spanValue = abs(sensorValueHistory - sensorValue);
  if ( spanValue > 50 ) {
    Nefry.setLed(255, 150, 0);        //LED orange
    Nefry.println("sending event to SOX... ");
    counter++;                        //increment the counter

    //Send data to SOX
    ThinSOXClient client;
    bool sendData = false;

    if ( client.connect(SOXHostname, SOXPort) ) {
      Nefry.println("Connected to SOX Server.");

      char timestamp[64] = {'\0'};
      time_t t = now();
      sprintf(timestamp, "%4d-%02d-%02dT%02d:%02d:%02d+09:00", year(t), month(t), day(t), hour(t), minute(t), second(t));

      if (client.publish(SOXNodeName,
                         SOXTransducerId,
                         String(timestamp),
                         String(sensorValue),
                         String(sensorValue)) ) {
        sendData = true;
        Nefry.println("Published to SOX Server. SUCCESS data = [" + (String)sensorValue);

      } else {
        Nefry.println("Published to SOX Server. FAILED data = [" + (String)sensorValue);
      }

    } else {
      Nefry.println("Connection to SOX Server: FAILED.");
    }

    if (!sendData) {//Was it successful?
      //Error case
      Nefry.setLed(255, 0, 0);
      Nefry.println("Error");
      Nefry.ndelay(3000);
    } else {
      //Success case
      Nefry.setLed(0, 255, 0);
      Nefry.println("Success");
      Nefry.ndelay(3000);
    }
  }

  Nefry.setLed(0, 0, 255);
  Nefry.println("--");

  sensorValueHistory = sensorValue;
  Nefry.ndelay(1000);
}


char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}
