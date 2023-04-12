TFT_eSprite cabecera = TFT_eSprite(&M5.Lcd);
TFT_eSprite cuadranteJoystick = TFT_eSprite(&M5.Lcd);
TFT_eSprite pie = TFT_eSprite(&M5.Lcd);

const int anchoCuadranteJoystick = 100;
const int altoCuadranteJoystick = 100;

void iniciarDisplay() {
  iniciarLcd();
  iniciarSpriteCabecera();
  iniciarSpriteCuadranteJoystick();
  iniciarSpritePie();
}

void iniciarLcd() {
  M5.Lcd.setRotation(0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println(" Mini JoyC");
}

void iniciarSpriteCabecera() {
  cabecera.createSprite(135, 35);
  cabecera.setRotation(0);
  cabecera.setTextSize(2);
}

void iniciarSpritePie() {
  pie.createSprite(135, 35);
  pie.setRotation(0);
  pie.setTextSize(2);
}
void iniciarSpriteCuadranteJoystick() {
  cuadranteJoystick.createSprite(anchoCuadranteJoystick, altoCuadranteJoystick);
  cuadranteJoystick.setRotation(0);
  cuadranteJoystick.setTextSize(2);
}

void actualizarSpriteCabecera() {
  cabecera.fillScreen(BLACK);
  cabecera.setCursor(0, 0);
  cabecera.print("   ");
  cabecera.print(M5.Axp.GetBatVoltage(), 2);  //3 decimales
  cabecera.println(" v");
  cabecera.print("  ");
  cabecera.print("Modo: ");
  cabecera.println(modoJoy);
  cabecera.pushSprite(0, 25);
}

void actualizarSpriteCuadranteJoystick() {
  const int origenCuadranteJoystickX = 20;
  const int origenCuadranteJoystickY = 95;
  const int centroCuadranteX = anchoCuadranteJoystick / 2;
  const int centroCuadranteY = altoCuadranteJoystick / 2;
  const int colorCuadranteJoystick = RED;
  const int sizeCenterCross = 25;
  const int radioCuadrante = 10;
  int coordenadaCirculoX = map(joy_x, -100, 100, 0, 100);
  int coordenadaCirculoY = map(joy_y, -100, 100, 100, 0);

  cuadranteJoystick.fillScreen(BLACK);
  cuadranteJoystick.drawRoundRect(0, 0, anchoCuadranteJoystick, altoCuadranteJoystick, radioCuadrante, colorCuadranteJoystick);
  cuadranteJoystick.drawCircle(coordenadaCirculoX, coordenadaCirculoY, radioCuadrante, colorCuadranteJoystick);
  cuadranteJoystick.drawFastHLine(centroCuadranteX - sizeCenterCross, centroCuadranteY, sizeCenterCross * 2, colorCuadranteJoystick);  // linea horizontal de la cruz central
  cuadranteJoystick.drawFastVLine(centroCuadranteX, centroCuadranteY - sizeCenterCross, sizeCenterCross * 2, colorCuadranteJoystick);  // linea vertical de la cruz central
  cuadranteJoystick.pushSprite(origenCuadranteJoystickX, origenCuadranteJoystickY);
}

void actualizarSpritePie() {
  pie.fillScreen(BLACK);
  pie.setCursor(35, 2);
  pie.print(millis());
  pie.pushSprite(0, 220);
}

void actualizarDisplay() {
  actualizarSpriteCabecera();
  actualizarSpriteCuadranteJoystick();
  actualizarSpritePie();
}

void timerPantallaLCD() {
  const int intervaloDeActualizacion = 20;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    actualizarDisplay();
  }
}