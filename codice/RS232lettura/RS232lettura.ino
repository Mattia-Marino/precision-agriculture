#include <AltSoftSerial.h>
#include <SDI12.h>

// GMX240: AltSoftSerial (8 = RX, 9 = TX)
AltSoftSerial gmxSerial;

// ATMOS14: SDI12 (data pin 7)
const int sdiPin = 7;
SDI12 sdi(sdiPin);

const unsigned long interval = 10000; // ogni 10 secondi
unsigned long previousMillis = 0;

const int bufferSize = 150;
char gmxBuffer[bufferSize];

void setup() {
  // Inizializza la Serial (hardware) per LoRa
  Serial.begin(9600);
  delay(1000); // attesa avvio LoRa
  
  // Inizializza GMX240
  gmxSerial.begin(19200);

  // Inizializza SDI-12
  sdi.begin();

  delay(2000); // attesa dispositivi
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    String gmxData = leggiGMX240();
    String atmosData = leggiATMOS14();

    String messaggio = atmosData + "," + gmxData;

    Serial.println(messaggio);  // Invio al modulo LoRa
  }
}

// ======================= GMX240 =========================

String leggiGMX240() {
  // Svuota eventuali dati precedenti
  while (gmxSerial.available()) gmxSerial.read();

  gmxSerial.print("Q\r\n");

  int i = 0;
  unsigned long timeout = millis();
  bool rispostaRicevuta = false;

  while (millis() - timeout < 5000 && i < bufferSize - 1) {
    if (gmxSerial.available()) {
      char c = gmxSerial.read();
      if (c == '\n') {
        rispostaRicevuta = true;
        break;
      }
      gmxBuffer[i++] = c;
      timeout = millis(); // reset timeout
    }
  }

  if (rispostaRicevuta) {
    return String(gmxBuffer);
  } else {
    return "GMX:NODATA";
  }
}

// ======================= ATMOS14 ========================

String leggiATMOS14() {

  sdi.sendCommand("0D0!");
  delay(500);

  String risposta = "";
  while (sdi.available()) {
    char c = sdi.read();
    risposta += c;
  }

  risposta.trim();

  if (risposta.length() > 0) {
    return risposta;
  } else {
    return "ATMOS:NODATA";
  }
}
