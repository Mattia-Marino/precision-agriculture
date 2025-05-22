#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

// GMX240 su pin 10 RX, 11 TX (SoftwareSerial)
SoftwareSerial gmxSerial(10, 11);

// ATMOS14 su pin 8 RX (AltSoftSerial)
AltSoftSerial atmosSerial;

const unsigned long interval = 10000; // 10 secondi
unsigned long previousMillis = 0;

const int bufferSize = 150;
char gmxBuffer[bufferSize];
char atmosBuffer[bufferSize];

void setup() {
  Serial.begin(9600);      // Monitor seriale
  gmxSerial.begin(19200);   // GMX240 baud
  atmosSerial.begin(115000); // ATMOS14 baud
  delay(2000);

  Serial.println("Sistema pronto.");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // --- Lettura GMX240 ---
    while (gmxSerial.available()) gmxSerial.read(); // svuota buffer
    gmxSerial.write('Q'); // invia comando
    delay(2000); // attesa risposta

    int i = 0;
    while (gmxSerial.available() && i < bufferSize - 1) {
      char c = gmxSerial.read();
      if (c == '\n') break;
      gmxBuffer[i++] = c;
    }
    gmxBuffer[i] = '\0';

    Serial.println("Dati GMX240:");
    Serial.println(gmxBuffer);
    parseGMX(gmxBuffer);

    // --- Lettura ATMOS 14 ---
    i = 0;
    delay(100); // accumula dati
    while (atmosSerial.available() && i < bufferSize - 1) {
      char c = atmosSerial.read();
      if (c == '\n') break;
      atmosBuffer[i++] = c;
    }
    atmosBuffer[i] = '\0';

    Serial.println("Dati ATMOS 14:");
    Serial.println(atmosBuffer);
    parseAtmos(atmosBuffer);

    Serial.println("----------");
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

  while (token != NULL && fieldIndex < numFields) {
    if (fieldIndex == 8) {
      Serial.print(labels[fieldIndex]);
      Serial.print(": ");
      formatTime(token);
    } else {
      Serial.print(labels[fieldIndex]);
      Serial.print(": ");
      Serial.println(token);
    }
    token = strtok(NULL, ",");
    fieldIndex++;
  }
}

void formatTime(const char* rawTime) {
  if (strlen(rawTime) >= 14) {
    char buffer[20];
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

void parseAtmos(char* line) {
  char* token = strtok(line, ",");

  if (token) {
    Serial.print("Temperatura aria (°C): ");
    Serial.println(token);
  }

  token = strtok(NULL, ",");
  if (token) {
    Serial.print("Umidità relativa (%): ");
    Serial.println(token);
  }

  token = strtok(NULL, ",");
  if (token) {
    Serial.print("Pressione di vapore (kPa): ");
    Serial.println(token);
  }

  token = strtok(NULL, ",");
  if (token) {
    Serial.print("Pressione barometrica (kPa): ");
    Serial.println(token);
  }
}
