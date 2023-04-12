const int analogInPin = A0;  // ESP8266 Analog Pin ADC0=A0
float adcValue = 0;
float batteryVoltage = 0;

int incoming_ejeX_Joystick;
int incoming_ejeY_Joystick;
bool incoming_modo_Joystick;

void iniciarEntradasSalidas() {
  pinMode(D4, OUTPUT);    //pinLED
  digitalWrite(D4, LOW);
}

void iniciarAccesorios() {
  Serial.begin(115200);
  iniciarEntradasSalidas();
}

float robotBatteryVoltage() {
  adcValue = analogRead(analogInPin);
  return (adcValue / 100);
}

