#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Pines para Esp32
int infrarrojo = 27;
int buzzer = 12;
int valor = 0;
String mensajeActivacion = ""; // para activar la alarma

// Credenciales conexión WiFi
const char* ssid = "Nombre conexión WiFi";
const char* password = "Contraseña conexión WiFi";

// Token del bot de telegram e ID personal
#define botToken "Token del bot creado"
#define ChatID "1111111111"

// Tiempo de escaneo del chat
const unsigned long tiempoEscaneo = 500;
unsigned long tiempoUltimoEscaneo;

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  
  // Estableciendo conexión WiFi
  Serial.print("Conectando");
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  // Espera la conexión e imprime la IP
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());

  pinMode(infrarrojo, INPUT);
  pinMode(buzzer, OUTPUT);
}


// Función auxiliar
void Alarma(int numNewMessages) 
{
  for (int i = 0; i < numNewMessages; i++) {
    // Almacenamos el mensaje y el ID del ususario que lo envía
    String texto_usuario = bot.messages[i].text;
    String chat_id = bot.messages[i].chat_id;

    if (texto_usuario == "/start") {
      String descripcion = "Hola Nicolás,\n¿deseas activar la alarma anti-intrusos?\n\n";
      descripcion += "/activar : Para activar la alarma\n";
      descripcion += "/desactivar : Para desactivar la alarma\n";
      bot.sendMessage(chat_id, descripcion);
    }

    if (texto_usuario == "/activar" && chat_id == ChatID) {
      mensajeActivacion = texto_usuario;
      bot.sendMessage(chat_id, "Alarma activada");
    }

    if (texto_usuario == "/desactivar" && chat_id == ChatID) {
      mensajeActivacion = texto_usuario;
      bot.sendMessage(chat_id, "Alarma desactivada");
    }  
  }
}


void loop() 
{
  if (millis() - tiempoUltimoEscaneo > tiempoEscaneo) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    String chat_id = bot.messages[0].chat_id;
    if (mensajeActivacion == "/activar") {
      valor = digitalRead(infrarrojo);
      if (valor == LOW) {
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        bot.sendMessage(chat_id, "¡¡¡Alerta!!!\n\nHay intrusos entrando a la habitación.");
      }
    }

    while (numNewMessages) {
      Alarma(numNewMessages);    
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    tiempoUltimoEscaneo = millis();
  }  
}  
