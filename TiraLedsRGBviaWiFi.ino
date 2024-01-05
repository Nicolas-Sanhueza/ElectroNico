#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// pines para Wemos D1 mini
int Azul = 4;
int Rojo = 13;
int Verde = 14;
String contrasena = "contraseña de acceso a visita"; // Contraseña de acceso a visita
String contrasena_ingresada = ""; // Contraseña ingresada por el usuario visita

// Credenciales conexión WiFi
const char* ssid = "Nombre conexión WiFi";
const char* password = "Contraseña conexión WiFi";

// Token del Bot e ID personal 
#define botToken "Token del bot creado"
#define ChatID "1111111111"

// Tiempo de escaneo del chat en milisegundos
const unsigned long tiempoEscaneo = 1000;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// Último escaneo del chat (tiempo)
unsigned long tiempoUltimoEscaneo; 

void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  // Estableciendo conexión WiFi
  Serial.print("Conectando");
  WiFi.begin(ssid, password);
  client.setTrustAnchors(&cert);
  
  // Espera la conexión
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());

  // Config del tiempo
  Serial.print("Tiempo: ");
  configTime(0, 0, "pool.ntp.org"); // UTC-time
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  pinMode(Rojo, OUTPUT);
  pinMode(Verde, OUTPUT);
  pinMode(Azul, OUTPUT);
} 


// Función auxiliar
void ModoLucesLed(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    // Almacenamos el mensaje y el ID del ususario que lo envía
    String texto_usuario = bot.messages[i].text;
    String chat_id = bot.messages[i].chat_id;
    
    // Inicialización del bot con comando /start
    if (texto_usuario == "/start" && (chat_id != ChatID || chat_id == ChatID)) {
      if (contrasena_ingresada == contrasena || chat_id == ChatID) {
        // Usuario invitado ya ingresó la contraseña correcta anteriormente, o bien, soy yo
        String descripcion = "Comandos disponibles:\n\n";
        descripcion += "/rojo : Para tira led de color rojo \n";
        descripcion += "/verde : Para tira led de color verde \n";
        descripcion += "/azul : Para tira led de color azul \n";
        descripcion += "/blanco : Para tira led de color banco \n";
        descripcion += "/apagar : Para apagar la tira de leds \n";
        bot.sendMessage(chat_id, descripcion, "Markdown");
      }
      else {
        // Mensaje de bienvenida a invitado/a
        String visita = "Bienvenido/a,\ncontrola las luces leds del dormitorio de Nicolás.\n\nPor favor, indica la contraseña";
        bot.sendMessage(chat_id, visita);
      }
    }

    // Comandos para manipular los colores de la tira Led
    else if (texto_usuario == "/rojo" && (chat_id == ChatID || contrasena_ingresada == contrasena)) {
      digitalWrite(Rojo, HIGH);
      bot.sendMessage(chat_id, "Estado de Leds, rojo.");
    }
    else if (texto_usuario == "/verde" && (chat_id == ChatID || contrasena_ingresada == contrasena)) {
      digitalWrite(Verde, HIGH);
      bot.sendMessage(chat_id, "Estado de Leds, verde.");
    }
    else if (texto_usuario == "/azul" && (chat_id == ChatID || contrasena_ingresada == contrasena)) {
      digitalWrite(Azul, HIGH);
      bot.sendMessage(chat_id, "Estado de Leds, azul.");
    }
    else if (texto_usuario == "/blanco" && (chat_id == ChatID || contrasena_ingresada == contrasena)) {
      digitalWrite(Rojo, HIGH);
      digitalWrite(Verde, HIGH);
      digitalWrite(Azul, HIGH);
      bot.sendMessage(chat_id, "Estado de Leds, blanco.");
    }
    else if (texto_usuario == "/apagar" && (chat_id == ChatID || contrasena_ingresada == contrasena)) {
      analogWrite(Rojo, 0);
      digitalWrite(Verde, LOW);
      digitalWrite(Azul, LOW);
      bot.sendMessage(chat_id, "Estado de Leds, apagado.");
    }

    // Caso en que texto_usuario no corresponde a un comando para el bot
    else {
      if (contrasena_ingresada != contrasena && chat_id != ChatID) { 
        // No se ha ingresado ninguna contraseña válida anteriormente
        contrasena_ingresada = texto_usuario;
        if (contrasena_ingresada != contrasena) {
          bot.sendMessage(chat_id, "Contraseña incorrecta.");
        }
        else {
          String descripcion_inv = "Contraseña correcta!\n";
          descripcion_inv += "Inicializa con /start si deseas revisar lo comandos disponibles.";
          bot.sendMessage(chat_id, descripcion_inv, "Markdown");
        }
      }  
      else {
        // Ya se había ingresado la contraseña correcta anteriormente, o bien, soy yo
        bot.sendMessage(chat_id, "El comando: " + texto_usuario + " no está disponible.");
      }
    }
  }
}

void loop() {
  if (millis() - tiempoUltimoEscaneo > tiempoEscaneo) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      ModoLucesLed(numNewMessages);    
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    tiempoUltimoEscaneo = millis();
  }
}
