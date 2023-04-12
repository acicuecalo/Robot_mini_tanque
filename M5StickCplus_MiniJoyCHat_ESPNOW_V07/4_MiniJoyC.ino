#include <UNIT_MiniJoyC.h>

#define POS_X 0
#define POS_Y 1

UNIT_JOYC joystick;

void leerJoystick() {
  joy_x = joystick.getPOSValue(POS_X, _8bit);
  joy_x = constrain(joy_x, -100, 100);
  joy_y = joystick.getPOSValue(POS_Y, _8bit);
  joy_y = constrain(joy_y, -100, 100);
  joy_button = joystick.getButtonStatus();
  seleccionarModoJoystick();
  //debugPrintMiniJoyC();
}

void debugPrintMiniJoyC() {
  Serial.print(joy_x);
  Serial.print("\t");
  Serial.print(joy_y);
  Serial.print("\t");
  Serial.println(joy_button);
}

int8_t EjeX_JoyC() {
  return joystick.getPOSValue(POS_X, _8bit);
}

int8_t EjeY_JoyC() {
  return joystick.getPOSValue(POS_Y, _8bit);
}

bool btn_JoyC() {
  return joystick.getButtonStatus();
}

void iniciarMiniJoyC() {
  M5.begin();
  joystick.begin(&Wire, JoyC_ADDR, 0, 26, 100000UL);
  
  /*
  while (!(joystick.begin(&Wire, JoyC_ADDR, 0, 26, 100000UL))) {
    delay(100);
    Serial.println("I2C Error!\r\n");
  }
  */

  const uint8_t ledIndex = 0;
  uint8_t ledRed = 0;
  uint8_t ledGreen = 0;
  uint8_t ledBlue = 10;
  uint32_t ledColor = (256 * 256 * ledRed) + (256 * ledGreen) + ledBlue;
  joystick.setLEDColor(ledIndex, ledColor);
}

void seleccionarModoJoystick() {
  if (botonB_pulsado && !botonB_siguePulsado) {
    modoJoy = !modoJoy;
    botonB_siguePulsado = 1;
  } else if (!botonB_pulsado) {
    botonB_siguePulsado = 0;
  }
}

void timerMiniJoyC() {
  const int intervaloDeActualizacion = 20;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    leerJoystick();
  }
}
