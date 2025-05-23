#include <SoftwareSerial.h>

const int rxPin = 10;   // Collegato al TX del MAX232
const int txPin = 11;   // Collegato all'RX del MAX232

SoftwareSerial gmxSerial(rxPin, txPin);  // RX, TX

const unsigned long interval = 10000;  // Intervallo tra richieste
unsigned long previousMillis = 0;

const int bufferSize = 150;
char gmxBuffer[bufferSize];

void setup() {
  Serial.begin(9600);        // Monitor seriale
  gmxSerial.begin(19200);    // GMX240 baud rate
  delay(2000);

  Serial.println("Sistema pronto.");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Svuota eventuali caratteri residui
    while (gmxSerial.available()) gmxSerial.read();

    // Invia comando 'Q' con CR+LF (ordine corretto!)
    gmxSerial.print("Q\r\n");
    Serial.println("Comando 'Q' inviato...");

    // Attende la risposta (fino a \n)
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
        timeout = millis(); // resetta timeout ad ogni nuovo carattere
      }
    }

    if (rispostaRicevuta) {
      Serial.println("Dati GMX240:");
      parseGMX(gmxBuffer);
    } else {
      Serial.println("Nessuna risposta dal GMX240.");
    }
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
    
    if (fieldIndex == 8) {  // campo TIME
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
             rawTime[0], rawTime[1], rawTime[2], rawTime[3],   // YYYY
             rawTime[4], rawTime[5],                           // MM
             rawTime[6], rawTime[7],                           // DD
             rawTime[8], rawTime[9],                           // hh
             rawTime[10], rawTime[11],                         // mm
             rawTime[12], rawTime[13]);                        // ss
    Serial.println(buffer);
  } else {
    Serial.println(rawTime);
  }
}
