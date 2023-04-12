#include <Servo.h>
Servo servoIzquierdo;
Servo servoDerecho;

void moverBaseRobot(int ejeX, int ejeY)  // Los valores de control de -100 a 100
{
  const int pinServoIzquierdo = D2;
  const int pinServoDerecho = D8;
  const int deathBand = 10;
  int velocidadMotorIzquierdo;
  int velocidadMotorDerecho;
  

  // Entradas
  ejeX = constrain(ejeX, -100, 100);
  ejeY = constrain(ejeY, -100, 100);
  if (abs(ejeX) < deathBand && abs(ejeY) < deathBand) {
    ejeX = 0;
    ejeY = 0;
  }

  // Mezclador
  velocidadMotorIzquierdo = ejeY + ejeX;  //motorDelanteroIzquierdo
  velocidadMotorDerecho = -ejeY + ejeX;   //motorDelanteroDerecho

  // Salidas Motores
  const int spanGrados = 35;
  const int offsetGrados = 0;
  const int gradosCentro = 90;
  const int gradosMin = gradosCentro - spanGrados + offsetGrados;
  const int gradosMax = gradosCentro + spanGrados + offsetGrados;
  velocidadMotorIzquierdo = map(velocidadMotorIzquierdo, -100, 100, gradosMin, gradosMax);
  velocidadMotorDerecho = map(velocidadMotorDerecho, -100, 100, gradosMin, gradosMax);
  
  if (velocidadMotorIzquierdo != 90) {
    if (!servoIzquierdo.attached()) {
      servoIzquierdo.attach(pinServoIzquierdo);
    }
    servoIzquierdo.write(velocidadMotorIzquierdo);
  } else {
    servoIzquierdo.detach();
    ;
  }

  if (velocidadMotorDerecho != 90) {
    if (!servoDerecho.attached()) {
      servoDerecho.attach(pinServoDerecho);
    }
    servoDerecho.write(velocidadMotorDerecho);
  } else {
    servoDerecho.detach();
    ;
  }
}

void pararMotores() {
  servoIzquierdo.detach();
  servoDerecho.detach();
}

void timerBaseRobot() {
  const int intervaloDeActualizacion = 20;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    moverBaseRobot(RX_Channel[0], RX_Channel[1]);
  }
}
