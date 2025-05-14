#include <cstdlib>

int pin_dos = 2;
int temp_ini = 15;
bool estado_vent = false;

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