#include <AltSoftSerial.h>
#include <SDI12.h>

AltSoftSerial gmxSerial; // Pin 8 = RX, Pin 9 = TX
const int sdiPin = 7;    // ATMOS14 SDI-12 data line
SDI12 sdi(sdiPin);

const unsigned long interval = 10000;
unsigned long previousMillis = 0;

const int bufferSize = 150;
char gmxBuffer[bufferSize];

void setup() {
  Serial.begin(9600);        // Monitor seriale
  gmxSerial.begin(19200);    // GMX240 baud rate
  sdi.begin();               // SDI-12 setup

  delay(2000);
  Serial.println("Sistema pronto.");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    leggiGMX240();
    Serial.println("\n\r");
    leggiATMOS14();
    Serial.println("\n\r");

  }
}

void leggiGMX240() {
  // Svuota buffer
  while (gmxSerial.available()) gmxSerial.read();

  gmxSerial.print("Q\r\n");
  Serial.println("Comando 'Q' inviato al GMX240...");
  Serial.println("\n\r");


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
      timeout = millis();  // resetta il timeout
    }
  }

  gmxBuffer[i] = '\0';  // termina stringa

  if (rispostaRicevuta) {
    Serial.println("Dati GMX240:");
    parseGMX(gmxBuffer);
  } else {
    Serial.println("Nessuna risposta dal GMX240.");
  }
}

void leggiATMOS14() {
  Serial.println("Richiesta dati da ATMOS 14 (SDI-12)...");

  sdi.sendCommand("0M!");
  delay(1500); // attesa misura completa

  sdi.sendCommand("0D0!");
  delay(500);

  String risposta = "";
  while (sdi.available()) {
    char c = sdi.read();
    risposta += c;
  }

  risposta.trim(); // rimuove spazi o newline finali

  if (risposta.length() > 0) {

    // Parsing basato su '+'
    char raw[100];
    risposta.toCharArray(raw, sizeof(raw));

    char* token = strtok(raw, "+");
    int campo = 0;

    while (token != NULL) {
      switch (campo) {
        case 1:
          Serial.print("Pressione vapore (hPa): ");
          Serial.println(token);
          break;
        case 2:
          Serial.print("Temperatura (°C): ");
          Serial.println(token);
          break;
        case 3:
          Serial.print("Umidità relativa (%): ");
          Serial.println(token);
          break;
        case 4:
          Serial.print("Pressione barometrica (hPa): ");
          Serial.println(token);
          break;
      }

      token = strtok(NULL, "+");
      campo++;
    }
  } else {
    Serial.println("Nessuna risposta da ATMOS 14.");
  }
}

void parseGMX(char* line) {
  const int numFields = 12;
  const char* labels[numFields] = {
    "NODE", "DIR", "SPEED", "CDIR", "CSPEED",
    "TOTAL PRECIP", "PRECIP INTENSITY", "GPS LOCATION",
    "TIME", "VOLT", "STATUS", "CHECK"
  };

  int fieldIndex = 0;
  char* token = strtok(line, ",");

  Serial.println("Parsing dati GMX240:");
  while (token != NULL && fieldIndex < numFields) {
    Serial.print(labels[fieldIndex]);
    Serial.print(": ");

    if (fieldIndex == 8) {
      formatTime(token);
    } else {
      Serial.println(token);
    }

    token = strtok(NULL, ",");
    fieldIndex++;
  }
}

void formatTime(const char* rawTime) {
  if (strlen(rawTime) >= 14) {
    char buffer[25];
    snprintf(buffer, sizeof(buffer), "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
             rawTime[0], rawTime[1], rawTime[2], rawTime[3],
             rawTime[4], rawTime[5],
             rawTime[6], rawTime[7],
             rawTime[8], rawTime[9],
             rawTime[10], rawTime[11],
             rawTime[12], rawTime[13]);
    Serial.println(buffer);
  } else {
    Serial.println(rawTime);
  }
}
