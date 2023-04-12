#include <M5StickCPlus.h>
#include <AXP192.h>

const int pinBotonB = 39;
bool botonB_pulsado;
bool botonB_siguePulsado;

void iniciaEntradasSalidas() {
  const int pinLed = 10;
  pinMode(pinLed, OUTPUT);     // LED
  digitalWrite(pinLed, HIGH);  // Apaga el led. El led luce con nivel bajo del pin 10
  pinMode(pinBotonB, INPUT);
}

void iniciarM5StickCPlus() {
  M5.begin();
  Serial.begin(115200);
  iniciaEntradasSalidas();
}

void ledOn() {
  const int pinLed = 10;
  pinMode(10, pinLed);
  digitalWrite(10, LOW);  // El led luce con nivel bajo
}

void ledOff() {
  const int pinLed = 10;
  pinMode(10, pinLed);
  digitalWrite(10, HIGH);  // El led se apaga con nivel alto
}

int porcentajeDeBateria() {
  int porcentajeBateria;
  porcentajeBateria = (M5.Axp.GetBatVoltage()) * 1000;
  porcentajeBateria = constrain(porcentajeBateria, 3400, 4200);
  porcentajeBateria = map(porcentajeBateria, 3400, 4200, 0, 100);
  return porcentajeBateria;
}

void actualizarAccesorios() {
  if (digitalRead(pinBotonB)) { // Sin pulsar el botón el pin está a 1.
    botonB_pulsado = 0;
  } else {
    botonB_pulsado = 1;
  }
}

void timerAccesorios() {
  const int intervaloDeActualizacion = 20;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    actualizarAccesorios();
  }
}
