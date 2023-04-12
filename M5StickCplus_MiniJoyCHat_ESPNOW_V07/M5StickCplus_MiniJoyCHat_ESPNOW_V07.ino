//v01 ok
//v02 ok, al miniJoyCHat se le añade mando auxiliar 8 Angle
//v03 ok, se añare recepción de telemetría con id2
//v04 ok
//v05 ok Se añade modo0 (normal) y modo1 del eje x del joystick cambiando con el botón B del M5Stick
//v07 ok Se añaden sprites en pantalla

int8_t joy_x;
int8_t joy_y;
bool joy_button;
bool modoJoy;

void setup() {
  iniciarESPNOW();
  iniciarM5StickCPlus();
  iniciarDisplay();
  iniciarMiniJoyC();
}

void loop() {
  timerESPNOW();
  timerAccesorios();
  timerPantallaLCD();
  timerMiniJoyC();
}
