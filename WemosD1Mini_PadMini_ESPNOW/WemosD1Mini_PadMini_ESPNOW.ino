int8_t ejeX;
int8_t ejeY;

void setup() {
  iniciarESPNOW();
  iniciarAccesorios();
}

void loop() {
  timerESPNOW();
  timerAccesorios();
}
