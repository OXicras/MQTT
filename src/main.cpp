//? Projeto: MQTT
//? Descrição: Conectando MQTT
//? Autor: Guilherme Pereira Neves dos Santos
//? Data: 03/11/2026
//? Versão: 0.1


#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

void aoReceber(char*, byte*, unsigned int);
void tratarJson(String);

const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* BROKER = "broker.hivemq.com"; //* endereço do servidor mqtt
const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/guity/comando";
const char* TOPICO_PUB = "senai134/esp32/guity/status";

WiFiClient rihannaClient; //* Instancia um cliente para que outros objetos utilizem a conexão WiFi
PubSubClient mqttAriana(rihannaClient); //* Instancia um objeto para criar conexão com o broker

void setup() 
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Conectando na rede %s\n\r", SSID);

  WiFi.begin(SSID, SENHA);

    while(WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(500);
    }

    delay(1000);

    Serial.printf("\n\rConectado a rede %s com sucesso\n\r", SSID);

    Serial.printf("Conectado ao Broker %s\n\r", BROKER);

    mqttAriana.setServer(BROKER, PORTA);
    mqttAriana.setCallback(aoReceber); //TODO criar essa função 

    String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

    bool conectadoAoBroker = false;
    conectadoAoBroker = mqttAriana.connect(id.c_str());

    if(conectadoAoBroker)
      {
        Serial.printf("Conectado com sucesso ao Broker %s\n\r", BROKER);
        mqttAriana.subscribe(TOPICO_SUB);
        Serial.printf("Inscrito no tópico: %s\n\r", TOPICO_SUB);
      }

    else
      {
        Serial.print("Falha ao conectar ao broker %s\n\r");
        Serial.print("Falha: "); Serial.println(mqttAriana.state());
      }

}

void loop() 
{
  mqttAriana.loop();
}

void aoReceber(char* topico, byte* payload, unsigned int tamanho)
{
  String mensagem;
  mensagem.reserve(tamanho); //* evita realocações enquanto a string cresce.

  for(unsigned int i = 0; i < tamanho; i++)
  {
    mensagem += (char)payload[i];
  }

  Serial.printf("Mensagem Recebida no tópico: %s \n\r mensagem: %s\n\r", topico, 
  mensagem.c_str());

  tratarJson(mensagem);
}

void tratarJson(String mensagem)
{
  JsonDocument docAdele;

  DeserializationError codigoErro = deserializeJson(docAdele, mensagem);

  bool estadoLed = docAdele["estadoLed"].as<bool>();

  if(estadoLed)
  {
    Serial.println("LED LIGADO");
    rgbLedWrite(48, 0, 255, 0);
  }

  else{
    Serial.println("LED DESLIGADO");
    rgbLedWrite(48, 0, 0, 0);
  }
}