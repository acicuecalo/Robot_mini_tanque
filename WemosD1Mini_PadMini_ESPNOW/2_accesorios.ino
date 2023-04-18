// Pin definitions
#define PIN_BOTON_ARRIBA D1
#define PIN_BOTON_ABAJO D7
#define PIN_BOTON_IZQUIERDA D6
#define PIN_BOTON_DERECHA D3
#define PIN_LED_ESP D4

bool botonArribaPulsado;
bool botonAbajoPulsado;
bool botonIzquierdaPulsado;
bool botonDerechaPulsado;

int pesoSalidaEjeX = 100;
int pesoSalidaEjeY = 100;

void iniciarEntradasSalidas() {
  pinMode(PIN_LED_ESP, OUTPUT);
  digitalWrite(PIN_LED_ESP, LOW);  // led encendido con nivel bajo
  pinMode(PIN_BOTON_ARRIBA, INPUT_PULLUP);
  pinMode(PIN_BOTON_ABAJO, INPUT_PULLUP);
  pinMode(PIN_BOTON_IZQUIERDA, INPUT_PULLUP);
  pinMode(PIN_BOTON_DERECHA, INPUT_PULLUP);
}

void iniciarAccesorios() {
  Serial.begin(115200);
  iniciarEntradasSalidas();
}

void leerBotones() {
  botonArribaPulsado = !digitalRead(PIN_BOTON_ARRIBA);
  botonAbajoPulsado = !digitalRead(PIN_BOTON_ABAJO);
  botonIzquierdaPulsado = !digitalRead(PIN_BOTON_IZQUIERDA);
  botonDerechaPulsado = !digitalRead(PIN_BOTON_DERECHA);

  if (botonIzquierdaPulsado) {
    ejeX = -pesoSalidaEjeX;
  }
  else if (botonDerechaPulsado) {
    ejeX = pesoSalidaEjeX;
  } else {
    ejeX = 0;
  }

  if (botonArribaPulsado) {
    ejeY = pesoSalidaEjeY;
  }
  else if (botonAbajoPulsado) {
    ejeY = -pesoSalidaEjeY;
  } else {
    ejeY = 0;
  }

}

void timerAccesorios() {
  const int intervaloDeActualizacion = 10;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    leerBotones();
  }
}
