#include <cstdlib>
#include <secret.h>
#include <ArduinoJson.h>
#include <CTBot.h>



int pin_dos = 2;
int temp_ini = 15;
bool estado_vent = false;

CTBot tel_bot ;
String ssid = "Wokwi-GUEST" ;
String pass = "" ;


void connect_wifi(){
  tel_bot.wifiConnect(ssid, pass);
  tel_bot.setTelegramToken(BOT_TOKEN);

  if(tel_bot.testConnection())
  Serial.println("conectado");
  else
  Serial.println("no");
}

// void handleNewMessages(int numNewMessages) {
  
//   Serial.println("handleNewMessages");
//   Serial.println(String(numNewMessages));

//   for (int i=0; i<numNewMessages; i++) {
//     // Chat id of the requester
//     String chat_id = String(bot.messages[i].chat_id);

//     if (chat_id != CHAT_ID){
//       bot.sendMessage(chat_id, "Unauthorized user", "");
//       continue;
//     }
//     // Print the received message
//     String text = bot.messages[i].text;
//     Serial.println(text);
//     String from_name = bot.messages[i].from_name;
    
//     if (text == "/start") {
//       String welcome = "Hola, " + from_name + ".\n";
//       welcome += "Usar los siguientes comandos para controlar.\n\n";
//       welcome += "/on para encender GPIO \n";
//       welcome += "/off para apagar GPIO \n";
//       welcome += "/estado para ver estado actualdel GPIO \n";
//       bot.sendMessage(chat_id, welcome, "");
//     }
//   }
// }

void abrir_ventanas() {
  if (estado_vent == false){
    Serial.println("Abriendo ventana...");
    digitalWrite(pin_dos, HIGH);
    delay(10000);
    digitalWrite(pin_dos, LOW);
    estado_vent = true;
  }
}

void cerrar_ventanas() {
  if (estado_vent == true){
    Serial.println("Cerrando ventana...");
    digitalWrite(pin_dos, HIGH);
    delay(10000);
    digitalWrite(pin_dos, LOW);
    estado_vent = false;
  }
}
//Iniciamos los pines del ESP32
void setup() {
  //Declaramos que el pin del led es de tipo salida, osea que la señal va salir
  Serial.begin(115200);

  pinMode(pin_dos, OUTPUT);

  connect_wifi();
}
//Iniciamos la funcion bucle que se repetira indefinidamente
void loop() {
  //Encendemos el led
  int temp = temp_ini + random() % 20;
  Serial.println(temp);
  if (temp >= 30){
    abrir_ventanas();
  }
  else if (temp <= 20){
    cerrar_ventanas();
  }
  if (estado_vent == true){
    Serial.println("Ventana abierta");
  }
  else{
    Serial.println("Ventana cerrada");
  }
  delay(3000);
}