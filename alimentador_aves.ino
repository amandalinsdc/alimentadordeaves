
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

const char* ssid = "ifce-alunos";
const char* password = "ifce4lun0s";

const char* mqttServer = "ifce.sanusb.org";
const int mqttPort = 1883;

const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

int h1, m1, h2, m2, h3, m3, porcao;
boolean horario1, horario2, horario3;

int timezone = -3;
int dst = 0;

// variáveis motor de passo
int direcao = 1;
int pino_step = 4;
int pino_direcao = 5;
int p1, p1_2, p1_8, h_p1, h_p2 , h_p3 = 0;

void jsonParserStatic(char json[]){

  StaticJsonDocument<500> doc;
  Serial.println("Json Parser Static");

  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  JsonObject root = doc.as<JsonObject>();

  if(root["porcao"] != ""){
    porcao = root["porcao"];
  }else {
    porcao = 100;
  }
  if(root["h1"] != ""){
    h1 = root["h1"];
    m1 = root["m1"];
    horario1 = true;
  }else{
    h1 = 100;
    horario1 = false;
  }
  if(root["h2"] != ""){
    h2 = root["h2"];
    m2 = root["m2"];
    horario2 = true;
  }else{
    h2 = 100;
    horario2 = false;
  }
  if(root["h3"] != ""){
    h3 = root["h3"];
    m3 = root["m3"];
    horario3 = true;
  }else{
    h3 = 100;
    horario3 = false;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  char json[length];
  
  for (int i = 0; i < length; i++) {
     json[i] = payload[i];
  }

  Serial.print("Json: ");
  for (int i = 0; i < length; i++) {
    Serial.print(json[i]);
  }
  
  jsonParserStatic(json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  configTime(timezone * 3600, dst * 0, "3.br.pool.ntp.org","0.south-america.pool.ntp.org", "1.south-america.pool.ntp.org");
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP82Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe("alimentador");

  pinMode(4, OUTPUT); // pino_step
  pinMode(5, OUTPUT); // pino_direcao

}

void girar_motor(){ //girar motor de passo
  // Define a direcao de rotacao
  direcao = 1;

  digitalWrite(pino_direcao, direcao);
  for (int p=0 ; p < 5000; p++) {
    digitalWrite(pino_step, 1);
    delayMicroseconds(160);
    digitalWrite(pino_step, 0);
    delayMicroseconds(160);
  }
  delay(1000);

  // Inverte a direcao de rotacao
  direcao = 0;
  
  digitalWrite(pino_direcao, direcao);
  for (int p=0 ; p < 5000; p++) {
    digitalWrite(pino_step, 1);
    delayMicroseconds(160);
    digitalWrite(pino_step, 0);
    delayMicroseconds(160);
  }
}

void loop() {
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  Serial.print("Hora: ");
  Serial.print(timeinfo->tm_hour);
  Serial.print(":");
  Serial.print(timeinfo->tm_min);
  Serial.print(":");
  Serial.println(timeinfo->tm_sec);
  delay(2000);
  client.loop();
  
  // Alimentar seu bichinho agora! 
  // Porcao 1
  if (porcao==1 && p1 < 6) {
    girar_motor();
    p1 = p1 + 1;
  }
  // Porcao 1/2
  if (porcao==12 && p1_2 < 4) {
     girar_motor();
     p1_2 = p1_2 + 1;
  }
  // Porcao 1/8
  if (porcao==18 && p1_8 < 2) {
     girar_motor();
     p1_8 = p1_8 + 1;
  }
  
  //Agende a alimentação do seu bichinho!
  // Primeiro horário
  if(horario1) {
    if(((timeinfo->tm_hour) == h1) && ((timeinfo->tm_min) == m1) && h_p1 < 4){
        girar_motor();
        h_p1 = h_p1 + 1;
    }
  }
  // Segundo horário
  if(horario2) {
    if(((timeinfo->tm_hour) == h2) && ((timeinfo->tm_min) == m2) && h_p2 < 4){
      girar_motor();
      h_p2 = h_p2 + 1;
    }
  }
  // Terceiro horário
  if(horario3) {
    if(((timeinfo->tm_hour) == h3) && ((timeinfo->tm_min) == m3) && h_p3 < 4){
      girar_motor();
      h_p3 = h_p3 + 1;
    }
  }
}
