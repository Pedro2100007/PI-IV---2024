//Arquivo:   PI003.ino   23/05/2024
//Tipo:      Codigo-fonte para NodeMCU atraves da IDE do Arduino
//Autor:     Pedro Otávio Sampaio Torres
//Descricao: Projeto integrador - Automacao aquaponia HTTP

#include <ESP8266WiFi.h>    //Biblioteca REDE WIFI
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "ThingSpeak.h"      //Biblioteca do API

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
Adafruit_BMP280 bme; // I2C

IPAddress ip(192, 168, 10, 50);      //IP definido ao NodeMCU
IPAddress gateway(192, 168, 10, 114);      //IP do roteador
IPAddress subnet(255, 255, 255, 0);      //Mascara da rede

WiFiServer server(80);      //Criando o servidor web na porta 80

const char* ssid = "ALGAR_Torres2023";      //Nome do WIFI
const char* password = "t0rres!001";      //Senha WIFI

const int pina = 0;      //Pino conectado ao LED 2 - D3
const int pinb = 2;      //Pino conectado ao LED 1 - D4
unsigned long channel = 2533413; //Channel = variável com ID no ThingSpeak
unsigned int field1 = 1;     //field1 = variavel indica qual field (1)
unsigned int field2 = 2;     //field1 = variavel indica qual field (1)
WiFiClient  client;
const char * myWriteAPIKey = "BY3NQR5RTECHYXQ5";
String myStatus = "";

void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(pina, OUTPUT); //--> declarando pina como saida
  pinMode(pinb, OUTPUT); //--> declarando pina como saida
  digitalWrite(pina, LOW); //--> tornando zero a saida pina
  digitalWrite(pinb, LOW); //--> tornando zero a saida pinb

  WiFi.begin(ssid, password);     //Aguardando conexão
  Serial.println("");
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado em : ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

Serial.println(F("BMP280 test"));
  
if (!bme.begin(0x76)) {  
Serial.println("Sensor BMP280 não encontrado!");
while (1);
}
  //----------------------------------------ThingSpeak.begin(client)
  ThingSpeak.begin(client);
}

void loop() {

Serial.print("Temperatura = ");
Serial.print(bme.readTemperature());
Serial.println(" *C");

Serial.print("Pressão = ");
Serial.print(bme.readPressure());
Serial.println(" Pa");

Serial.print("Altitude = ");
Serial.print(bme.readAltitude(1013.25)); // ajustar para a altirude do local da instalação
Serial.println(" m");

   // set the fields with the values
  ThingSpeak.setField(3, bme.readTemperature());
  ThingSpeak.setField(4, bme.readPressure());
  ThingSpeak.setField(5, bme.readAltitude(1013.25));
  // set the status
  ThingSpeak.setStatus(myStatus);  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(channel, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }


Serial.println();
delay(2000);

  int statusCode = 0;
  int est_bba = ThingSpeak.readFloatField(channel, field1);  //--> Pega ultimo estado field1
  int est_aqu = ThingSpeak.readFloatField(channel, field2);  //--> Pega ultimo estado field2

  statusCode = ThingSpeak.getLastReadStatus();  //--> Verifica se leitura do status foi bm sucedida
  if(statusCode == 200){
    if(est_bba == 1){
      digitalWrite(pina, HIGH);
      Serial.println("Bomba ligada");
    }
    else if(est_bba == 0){
      digitalWrite(pina, LOW);
      Serial.println("Bomba desligada");
    }
    Serial.print("O último estado da bomba no API é : ");
    Serial.println(est_bba);    
    if(est_aqu == 1){
      digitalWrite(pinb, HIGH);
      Serial.println("Aquecedor ligado");
    }
    else if(est_aqu == 0){
      digitalWrite(pinb, LOW);
      Serial.println("Aquecedor desligado");
    }
    Serial.print("O último estado do aquecedor no API é : ");
    Serial.println(est_bba);    
  }
  else {
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode));
  }
  delay(15000);
}
