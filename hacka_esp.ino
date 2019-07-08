#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial MasterSerial(D7, D8); // RX, TX

#define DEBUGER

/* Set these to your desired credentials. */
const char *ssid = "FreeWifi";  //ENTER YOUR WIFI SETTINGS
const char *password = "freecaicc";
const char *ip = "192.168.43.167";
const uint16_t port = 9999;


unsigned long last_time;
int i = 0;
/*--------------------UPDATE TO CLIENT------------------------------*/
//int updateMode =1;
//int changeSpeed =2;
const int movingSpeed = 3;
const int chargingThreshold = 4;
const int spinnerSpeed = 5;
const int maxPower = 6;
const int minPower = 7;

/*--------------------UPDATE TO SERVER------------------------------*/
int battery[] = {0, 0, 0};
const int updateBattery = 30;
const int updateMachineStatus = 31;
const int updatePanel = 32;

/*-----------------------------------------------------------------*/

//char jsonBattery[] = "{\"Type\":30,\"Data\":{\"current\":%d,\"voltage\":%d,\"energy\":%d}}\r\n";
char jsonBattery[] = "{\"Type\":30,\"Data\":\"{'current':%d,'voltage':%d,'energy':%d}\"}\r\n";
char jsonParameter[] = "{\"Type\":31,\"Data\":{\"status\":\"run\",\"direction\":\"forward\",\"location\":{\"X\":1,\"Y\":3}}}\r\n";
char jsonPanel[] = "{\"Type\":32,\"Data\":{\"location\":{\"X\":2,\"Y\":4},\"current\":30,\"voltage\":24}}\r\n";

/* event callbacks */
static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
#ifdef DEBUGER
  Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
#endif

  //  Serial.write((uint8_t*)data, len);
  const char *char_pointer = (char*)((uint8_t*)data);


#ifdef DEBUGER
  Serial.println(char_pointer);
#endif
  // Allocate the JsonDocument
  StaticJsonDocument<500> doc;

  // Parse the JSON input
  DeserializationError err = deserializeJson(doc, char_pointer);

  if (err) {
#ifdef DEBUGER
    Serial.println("parseObject() failed");
#endif

    return;
  } else {
    /*****************************************************/
    int type = doc["Type"];
    const char* data = doc["Data"];
    switch (type) {
      /*
        case updateMode:
          string mode = doc["Data"];
          setMode(mode);
          -------------
        case changeSpeed:
          string speed = doc["Data"];
          int speedNumber = toInt(speed);
          setSpeed(speedNumber);
      */
      case movingSpeed:

        //Serial.println(String("0x01") + String(data));
        //MasterSerial.println(String("0x01") + String(data));
        break;

      case chargingThreshold:
        break;

      case spinnerSpeed:
        break;

      case maxPower:

        break;

      case minPower:
        break;

      default:
        Serial.println("Unknown receive cmd");
    }
    /*****************************************************/
    //const char* ID = doc["ID"];
    //Serial.println(ID);
    int current = doc["battery"]["current"];
    Serial.println(String("0x01") + String(current));
    MasterSerial.println(String("0x01") + String(current));

    int voltage = doc["battery"]["voltage"];
    Serial.println(String("0x02") + String(voltage));
    MasterSerial.println(String("0x02") + String(voltage));
    //MasterSerial.println(char_pointer);
  }
}

void onConnect(void* arg, AsyncClient* client) {
#ifdef DEBUGER
  Serial.printf("\n client has been connected to %s on port %d \n", ip, port);
#endif
  RegisterClient(client);
}
void RegisterClient(void* arg) {
  //Get client ID;
  char clientID[] = "clientidtest";
  char connectJson[] = "{\"SocketClientType\":2,\"Code\":5,\"TargetID\":\"\",\"SendID\":\"102140064\",\"CmdToken\":null,\"FirstParameter\":null,\"SecondParameter\":null}\r\n";
  AsyncClient* client = reinterpret_cast<AsyncClient*>(arg);
  if (client->space() > 300 && client->canSend()) {
    char message[300];
    sprintf(message, connectJson, random(99), random(99));
    client->add(message, strlen(message));
    client->send();
  }
}

void SendClient(void* arg, int type) {
  char message[300];
  switch (type) {
    case updateBattery:
      sprintf(message, jsonBattery, battery[0], battery[1], battery[2]);
      break;
    default:
      Serial.println("Unknown send client cmd");
      return;
  }
  AsyncClient* client = reinterpret_cast<AsyncClient*>(arg);

  // send reply
  if (client->space() > 300 && client->canSend()) {


    client->add(message, strlen(message));
    client->send();
  }
}

AsyncClient* client = new AsyncClient;
void setup() {
  Serial.begin(9600);
  MasterSerial.begin(9600);
  delay(20);

  // connects to access point
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
#ifdef DEBUGER
    Serial.print('.');
#endif
    delay(500);
  }

  client->onData(&handleData, client);
  client->onConnect(&onConnect, client);
  client->connect(ip, port);
}

void loop() {
  if ( (unsigned long) (millis() - last_time) > 2000)
  {
    battery[0] = random(9);
    battery[1] = random(10, 24);
    battery[2] = random(99);
    //replyData(client, json);
    SendClient(client, updateBattery);
    last_time = millis();
  }
}
