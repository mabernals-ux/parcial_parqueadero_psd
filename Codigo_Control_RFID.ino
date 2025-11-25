
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ======================================
// CONFIG WIFI
// ======================================
const char* ssid     = "HONOR Magic7 Lite";
const char* password = "85591078";
String serverURL     = "http://10.212.226.52:5000/rfid";
String serverEstado  = "http://10.212.226.52:5000/puesto";

// ======================================
// UART2 – Arduino Maestro
// ======================================
#define RXp2 16
#define TXp2 17
String buffer = "";

// Estado de pines 2–11
String estadoPuesto[12];

// Variables nuevas
int puestoEsperado = -1;   // Lo que dice el backend
int puestoDetectado = -1;  // Lo que envía Arduino

// ======================================
// RFID
// ======================================
#define RST_PIN 2
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define SS_IN   5
#define SS_OUT  15

MFRC522 rfidEntrada(SS_IN, RST_PIN);
MFRC522 rfidSalida(SS_OUT, RST_PIN);

// ======================================
// LCD
// ======================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ======================================
// SERVO / BUZZER
// ======================================
#define SERVO_PIN 13
#define BUZZER_PIN 4
Servo barrera;

// ======================================
// FUNCIONES GENERALES
// ======================================
void lcdMsg(String l1, String l2) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(l1.substring(0,16));
  lcd.setCursor(0,1);
  lcd.print(l2.substring(0,16));
}

void beep(int ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(ms);
  digitalWrite(BUZZER_PIN, LOW);
}

void levantarBarrera() {
  barrera.write(90);
  delay(3000);
  barrera.write(0);
}

void mostrarEspera() {
  lcdMsg("Esperando...", "RFID");
}

// ======================================
// ENVÍO A SERVIDOR – OCUPAR PUESTO
// ======================================
void marcarPuestoOcupado(int puesto) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(serverEstado);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"puesto\":" + String(puesto) + ",\"estado\":\"OCUPADO\"}";

  int code = http.POST(json);
  http.end();

  Serial.print("Marcado puesto ");
  Serial.print(puesto);
  Serial.print(" como ocupado. Respuesta: ");
  Serial.println(code);
}

// ======================================
// PROCESAR MENSAJE DEL ARDUINO
// ======================================
void procesarLinea(String linea) {
  linea.trim();
  if (!linea.startsWith("PIN")) return;

  int sep = linea.indexOf(':');
  if (sep < 0) return;

  int pin = linea.substring(4, sep).toInt();
  String mensaje = linea.substring(sep + 2);

  estadoPuesto[pin] = mensaje;

  Serial.print("[ESP32] Sensor ");
  Serial.print(pin);
  Serial.print(" → ");
  Serial.println(mensaje);

  // ---------------------------------------------------
  // Si sensor indica VEHÍCULO o MOTO detectada
  // ---------------------------------------------------
  if (mensaje.indexOf("Vehiculo detectado") >= 0 || mensaje.indexOf("Moto detectada") >= 0) {

    puestoDetectado = pin;

    // Caso: no hay reserva (ningún puesto asignado)
    if (puestoEsperado == -1) {
      lcdMsg("Vehiculo", "sin reserva");
      return;
    }

    // CASO 1: Correcto
    if (puestoDetectado == puestoEsperado) {
      lcdMsg("Puesto correcto", "Ocupando...");
      marcarPuestoOcupado(puestoDetectado);
      puestoEsperado = -1;  // se libera la reserva
    }

    // CASO 2: Incorrecto
    else {
      lcdMsg("Puesto INCORRECTO", "Debe ir al " + String(puestoEsperado));
      beep(300);
      // Nota: No limpiar puestoEsperado para seguir mostrando mensaje
    }
  }

  // ---------------------------------------------------
  // Si el vehículo salió
  // ---------------------------------------------------
  else if (mensaje.indexOf("Salio vehiculo") >= 0 || mensaje.indexOf("Salida Moto") >= 0) {
    if (puestoDetectado == pin) {
      Serial.print("Vehiculo/moto salió del puesto ");
      Serial.println(pin);
      estadoPuesto[pin] = "Libre";

      // Solo liberar puestoEsperado si el vehículo correcto salió
      if (pin == puestoEsperado) {
        puestoEsperado = -1;

      }
    }
  }
}


// ======================================
// LEER RFID
// ======================================
bool leerRFID(MFRC522 &lector, uint64_t &uidNum, int ssPin) {
  digitalWrite(SS_IN, HIGH);
  digitalWrite(SS_OUT, HIGH);
  digitalWrite(ssPin, LOW);

  if (!lector.PICC_IsNewCardPresent()) { digitalWrite(ssPin,HIGH); return false; }
  if (!lector.PICC_ReadCardSerial())   { digitalWrite(ssPin,HIGH); return false; }

  uidNum = 0;
  for (byte i = 0; i < lector.uid.size; i++)
    uidNum = (uidNum << 8) | lector.uid.uidByte[i];

  lector.PICC_HaltA();
  lector.PCD_StopCrypto1();
  digitalWrite(ssPin, HIGH);
  return true;
}

// ======================================
// PROCESAR UID CON SERVIDOR
// ======================================
void procesarUID(String tipo, uint64_t uid) {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type","application/json");

    String json = "{\"tipo\":\"" + tipo + "\",\"uid\":\"" + String(uid) + "\"}";
    int httpCode = http.POST(json);
    if (httpCode <= 0) { http.end(); return; }

    String resp = http.getString();
    http.end();

    StaticJsonDocument<256> doc;
    deserializeJson(doc, resp);

    String line1 = doc["line1"];
    String line2 = doc["line2"];

    lcdMsg(line1, line2);

    // Detectar puesto asignado si existe
    bool autorizado = false;
    if (line2.indexOf("Puesto") >= 0) {
        int pos = line2.indexOf("Puesto ") + 7;
        puestoEsperado = line2.substring(pos).toInt();
        Serial.print("Puesto esperado: "); Serial.println(puestoEsperado);
        autorizado = true;
    }

    if (line1.indexOf("Bienvenido") >= 0 || tipo == "OUT") {
        autorizado = true;
    }

    beep(200);

    // Levantar barrera si autorizado
    if (autorizado) {
        levantarBarrera();
    }

    delay(1500);
    mostrarEspera();
}

// ======================================
// SETUP
// ======================================
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  WiFi.begin(ssid, password);
  lcd.init();
  lcd.backlight();
  lcdMsg("Conectando...", "");

  while (WiFi.status() != WL_CONNECTED) delay(500);

  lcdMsg("WIFI OK", WiFi.localIP().toString());
  delay(1000);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
  pinMode(SS_IN, OUTPUT);
  pinMode(SS_OUT, OUTPUT);
  digitalWrite(SS_IN, HIGH);
  digitalWrite(SS_OUT, HIGH);

  rfidEntrada.PCD_Init();
  rfidSalida.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  barrera.attach(SERVO_PIN);
  barrera.write(0);

  mostrarEspera();
}

// ======================================
// LOOP
// ======================================
void loop() {
  // ----------------------------
  // RFID Entrada
  // ----------------------------
  uint64_t uid = 0;
  if (leerRFID(rfidEntrada, uid, SS_IN)) {
    procesarUID("IN", uid);
  }

  // ----------------------------
  // RFID Salida
  // ----------------------------
  if (leerRFID(rfidSalida, uid, SS_OUT)) {
    procesarUID("OUT", uid);
  }

  // ----------------------------
  // Lectura Arduino por UART2
  // ----------------------------
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {
      procesarLinea(buffer);
      buffer = "";
    } else buffer += c;
  }
}




