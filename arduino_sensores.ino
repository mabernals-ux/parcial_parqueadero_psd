// --- Pines usados del 2 al 11 ---
const int pines[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int numPines = 10;

// --- Aquí se guarda el estado anterior de cada pin ---
int estadosAnteriores[numPines];

// --- Identificación de sensores Hall ---
bool esHall(int pin) {
  return (pin == 8 || pin == 9 || pin == 10);
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numPines; i++) {
    pinMode(pines[i], INPUT_PULLUP);
    estadosAnteriores[i] = HIGH;
  }

  Serial.println("ARDUINO_LISTO");
}

void loop() {

  for (int i = 0; i < numPines; i++) {

    int estadoActual = digitalRead(pines[i]);

    if (estadoActual != estadosAnteriores[i]) {

      Serial.print("PIN ");
      Serial.print(pines[i]);
      Serial.print(": ");

      if (esHall(pines[i])) {
        if (estadoActual == HIGH) Serial.println("Moto detectada");
        else Serial.println("Salida Moto");
      } else {
        if (estadoActual == LOW) Serial.println("Vehiculo detectado");
        else Serial.println("Salio vehiculo");
      }

      estadosAnteriores[i] = estadoActual;
    }
  }

  delay(50);
}
