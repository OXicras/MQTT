#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

void aoReceber(char*, byte*, unsigned int);
void tratarJson(String);

const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* BROKER = "broker.emqx.io"; //* endereço do servidor mqtt
const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/bruno/comando";
const char* TOPICO_PUB = "senai134/esp32/bruno/status";

WiFiClient wifiClient; //* Instancia um cliente para que outros objetos utilizem a conexão WiFi
PubSubClient mqtt(wifiClient); //* Instancia um objeto para criar conexão com o broker

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

    mqtt.setServer(BROKER, PORTA); //! 1883 ou 8883
    mqtt.setCallback(aoReceber);

    String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

    bool conectadoAoBroker = false;
    conectadoAoBroker = mqtt.connect(id.c_str());

    if(conectadoAoBroker)
      {
        Serial.printf("Conectado com sucesso ao Broker %s\n\r", BROKER);
        mqtt.subscribe(TOPICO_SUB);
        Serial.printf("Inscrito no tópico: %s\n\r", TOPICO_SUB);
      }

    else
      {
        Serial.print("Falha ao conectar ao broker %s\n\r");
        Serial.print("Falha: "); Serial.println(mqtt.state());
      }

}

void loop() 
{
  mqtt.loop();

  //* Dados:
  float temperatura = 50.0f;
  bool estado = true;
  unsigned int timestamp = 1793796662;
  String mensagemErro = "Sem Erro";
  int posicaoDisplay = 3;

  static unsigned long ultimoEnvio = 0;
  const unsigned long tempoEntreEnvios = 5000;
  if (millis() - ultimoEnvio > tempoEntreEnvios)
  {
    ultimoEnvio = millis();

    JsonDocument doc;
    doc["temperatura"] = temperatura;
    doc["estado"] = estado;
    doc["timestamp"] = timestamp;
    doc["mensagemErro"] = mensagemErro;
    doc["posicaoDisplay"] = posicaoDisplay;

    String mensagem;
    serializeJson(doc, mensagem);
    mqtt.publish(TOPICO_PUB, mensagem.c_str());
    Serial.printf("Mensagem a ser enviada no tópico %s:\n\r%s\n\r", TOPICO_PUB, mensagem.c_str());
  }

}

void aoReceber(char* topico, uint8_t* payload, unsigned int tamanho)
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
  JsonDocument doc;

  DeserializationError codigoErro = deserializeJson(doc, mensagem);

  bool estadoLed = doc["estadoLed"].as<bool>();

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