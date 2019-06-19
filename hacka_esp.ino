#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

unsigned long last_time;

String Rfull;

//char json[] = "{\"ID\":10203029062019}\r\n";
char json[] = "{\"ID\":10203029062019,\"battery\":{\"current\":30,\"voltage\":24,\"energy\":90},\"parameter\":{\"status\":\"run\",\"direction\":\"forward\",\"location\":{\"X\":1,\"Y\":3}},\"panel\":{\"location\":{\"X\":2,\"Y\":4},\"current\":30,\"voltage\":24}}\r\n";
static void replyToServer(void* arg) {
  AsyncClient* client = reinterpret_cast<AsyncClient*>(arg);

  // send reply
  if (client->space() > 32 && client->canSend()) {
    char message[32];
    sprintf(message, "this is from %s \r\n", WiFi.localIP().toString().c_str());
    client->add(message, strlen(message));
    client->send();
  }
}

static void replyData(void* arg) {
  AsyncClient* client = reinterpret_cast<AsyncClient*>(arg);

  // send reply
  if (client->space() > 200 && client->canSend()) {
    char message[200];
    sprintf(message, json);
    client->add(message, strlen(message));
    client->send();
  }
}


/* event callbacks */
static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
  Serial.printf("\n data received from %s \n", client->remoteIP().toString().c_str());
  Serial.write((uint8_t*)data, len);

}

void onConnect(void* arg, AsyncClient* client) {
  Serial.printf("\n client has been connected to %s on port %d \n", "192.168.43.177", 9999);
  replyToServer(client);
}

AsyncClient* client = new AsyncClient;
void setup() {
  Serial.begin(115200);
  delay(20);

  // connects to access point
  WiFi.mode(WIFI_STA);
  WiFi.begin("FreeWifi", "freecaicc");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  client->onData(&handleData, client);
  client->onConnect(&onConnect, client);
  client->connect("192.168.0.102", 9999);  //192.168.43.177;

}

void loop() {
  if ( (unsigned long) (millis() - last_time) > 2000)
  {
    replyToServer(client);
    delay(1000);
    replyData(client);
    last_time = millis();
  }
}
