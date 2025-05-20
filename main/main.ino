#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <cstdlib>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <secret.h>

#define PIN_LED 12
#define BOT_TOKEN "7618978665:AAHuhrlPt6Uz2XXOc9dn2Ba3SmibbUfoy5w"
#define CHAT_ID "-4924904468"
const int TEMP_MIN = 20;
const int TEMP_MAX = 30;


int temp_ini = 15;
bool estado_vent = false;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

int status = WL_IDLE_STATUS;

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

unsigned long lastNotificationTime = 0; // ultima notificacion
const unsigned long notificationInterval = 300000; // Tiempo de intervalo entre notificaciones = 5 minutos

unsigned long lastMeasurement = 0;
const unsigned long measurementInterval = 60000; // Tiempo entre mediciones = 1 minuto

int temp = 0;
void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);

    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    
    if (text == "/start") {
      String welcome = "Hola, " + from_name + ".\n";
      welcome += "Usar los siguientes comandos para controlar.\n\n";
      welcome += "/abrir para abrir ventanas \n";
      welcome += "/cerrar para cerrar ventanas \n";
      welcome += "/activar_automatico para activar modo automático de manejo \n";
      welcome += "/desactivar_automatico para desactivar modo automático de manejo \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/abrir") {
      if (abrir_ventanas())
        bot.sendMessage(chat_id, "Ventanas abiertas", "");
      else 
        bot.sendMessage(chat_id, "Las ventanas ya se encontraban abiertas", "");
    }

    if (text == "/cerrar") {
      if (cerrar_ventanas())
        bot.sendMessage(chat_id, "Ventanas cerradas", "");
      else
        bot.sendMessage(chat_id, "Las ventanas ya se encontraban cerradas", "");
    }

    if (text == "/activar_automatico" || text == "/desactivar_automatico"){
      bot.sendMessage(chat_id, "Todavia no implementado", "");
    }
  }
}

bool abrir_ventanas() {
  if (estado_vent == false){
    Serial.println("Abriendo ventana...");
    digitalWrite(PIN_LED, HIGH);
    delay(10000);
    digitalWrite(PIN_LED, LOW);
    estado_vent = true;
    return true;
  }
  return false;
}

bool cerrar_ventanas() {
  if (estado_vent == true){
    Serial.println("Cerrando ventana...");
    digitalWrite(PIN_LED, HIGH);
    delay(10000);
    digitalWrite(PIN_LED, LOW);
    estado_vent = false;
    return true;
  }
  return false;
  
}

void actuar_cambio_temp(){
  if (temp > TEMP_MAX)
    abrir_ventanas();
  else
    if (temp < TEMP_MIN)
      cerrar_ventanas();
    else
      Serial.println("No accion ante cambio temp");
}
//Iniciamos los pines del ESP32
void setup() {

  Serial.begin(115200);

    //Initialize serial and wait for port to open:
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());


  configTime(-3 * 3600, 0, "pool.ntp.org");  // UTC-3 para Argentina, por ejemplo
  // Esperar a que se sincronice
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(1000);
    Serial.println("Esperando hora...");
  }

  pinMode(PIN_LED, OUTPUT);
}

String calcular_proximo_mensaje() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Hora no disponible";
  }

  // Convertir a timestamp
  time_t now = mktime(&timeinfo);
  now += notificationInterval / 1000; // Sumar 5 minutos (en segundos)

  // Convertir de nuevo a estructura de tiempo
  struct tm* future = localtime(&now);

  char buffer[6]; // HH:MM
  sprintf(buffer, "%02d:%02d", future->tm_hour, future->tm_min);
  return String(buffer);
}

String estado_vent_to_str(){
  if (estado_vent)
    return "Abiertas";
  else
    return "Cerradas";
}
//Iniciamos la funcion bucle que se repetira indefinidamente
void loop(){
  //Encendemos el PIN_LED
  Serial.println(temp);

  if ((millis() - lastMeasurement) > measurementInterval){
    temp = random(15, 35);
    actuar_cambio_temp();
    lastMeasurement = millis();
  }

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.print("mensajes:");
    Serial.println(numNewMessages);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  //aviso cada cinco minutos
  unsigned long now = millis();
  if ((now - lastNotificationTime) > notificationInterval) {
    String prox_msg = calcular_proximo_mensaje();
    String estado_vent_str = estado_vent_to_str();
    temp = random(10,35);
    String temp_str = String(temp);

    String msg = "Temperatura actual: "+ temp_str + "\n";
    msg += "Estado de ventanas: "+ estado_vent_str + "\n\n";
    msg += "Próxima actualización: "+prox_msg;
    
    bot.sendMessage(CHAT_ID, msg, "");
    
    
    Serial.println("Msg enviado!!!!");
    lastNotificationTime = now; // Update last notification time
  }
  delay(1000);
}