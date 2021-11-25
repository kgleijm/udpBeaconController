#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// connection variables
const char* ssid = "ssid";
const char* password =  "pass";
const uint16_t tcpPort = 8090;
const uint16_t udpPort = 4210;
const char * serverIp = "192.168.1.37";
WiFiClient client;
WiFiUDP UDP;

// frequency variables
int tFreq = 0;
int msgCount = 0;

// timing variables
unsigned long tcpStartTime;
unsigned long tcpTimeElapsed;
unsigned long tcpSnoozeTime;
unsigned long udpStartTime;
unsigned long udpTimeElapsed;
unsigned long udpSnoozeTime;

int getTransmissionFrequency(){

  // try to connect to host
  while (!client.connect(serverIp, tcpPort)) {
    Serial.println("Connection to host failed");
    delay(1000);
  }
  Serial.println("Connected to server successful!");

  // send message
  client.print("{\"request\": \"messageFrequency\"}");
  String line = client.readStringUntil('\n');
  Serial.println("Response from host: ");
  Serial.println(line);
  client.stop();
  return line.toInt();
}


void setup()
{

  // initalize Timing
  tcpSnoozeTime = 60000;
  udpSnoozeTime = 1000;
  udpStartTime = tcpSnoozeTime;
  tcpStartTime = millis();

  // Initialize WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  // get initial transmission frequency
  tFreq =  getTransmissionFrequency();
  udpSnoozeTime = ((double)60/tFreq)*1000;
  Serial.println("udpSnooze:");
  Serial.println(udpSnoozeTime);
  Serial.println("sending per minute:");
  Serial.println(tFreq);
}

void loop()
{

  tcpTimeElapsed = millis() - tcpStartTime;
  udpTimeElapsed = millis() - udpStartTime;


  if (tcpTimeElapsed > tcpSnoozeTime)
  {
    // calculate time between messages
    tFreq = getTransmissionFrequency();
    udpSnoozeTime = ((double)60/tFreq)*1000;
    tcpStartTime = millis();
    msgCount = 0;

    // show debug
    Serial.println("udpSnooze:");
    Serial.println(udpSnoozeTime);
    Serial.println("sending per minute:");
    Serial.println(tFreq);
    Serial.println("");
  }


  if (udpTimeElapsed > udpSnoozeTime)
  {

    // variable administration
    msgCount++;
    udpStartTime = millis();

    // send udp message
    String outgoing = ("{\"message\": " + String(msgCount) + ", \"of\": " + String(tFreq) + " }");
    char reply[255];
    outgoing.toCharArray(reply, 255);
    UDP.beginPacket(serverIp, udpPort);
    UDP.write(reply);
    UDP.endPacket();

    // show debug
    Serial.println("Sent message:");
    Serial.println(outgoing);
    Serial.println("message number:");
    Serial.println(msgCount);
    Serial.println("");
  }

}