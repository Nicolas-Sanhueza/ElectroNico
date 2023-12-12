#include <ArduinoJson.h>
#include <CTBot.h>
#include <Servo.h>

Servo miservo; 
CTBot miBot;

static const int ServoPin = 13;
static const int LedPin = 12;

const char* ssid     = "Nombre conexión WiFi";
const char* password = "Contraseña conexión WiFi";

#define miBotToken "Token del bot creado" 
#define ChatID 1111111111

void setup() {

  Serial.begin(115200);

  miservo.attach(ServoPin);
  pinMode(LedPin, OUTPUT);

  // Red WiFi con SSID y password
  miBot.wifiConnect(ssid, password);
  miBot.setTelegramToken(miBotToken);
  
  if (miBot.testConnection()) {
    Serial.println("WiFi conectado.");
  }

  miBot.sendMessage(ChatID, "Bot iniciado");
}

void loop() {
  TBMessage mensaje;

  if (CTBotMessageText == miBot.getNewMessage(mensaje)) {

    if (mensaje.text == "/alimentar") {
      miservo.write(90);             
      delay(500);                       
      miservo.write(0);          
      miBot.sendMessage(ChatID, "Alimentando");    
    }

    if (mensaje.text == "/alimentar") {
      digitalWrite(LedPin, HIGH);
      delay(1000);                      
      digitalWrite(LedPin, LOW);   
      delay(1000);
    }
  }
}
