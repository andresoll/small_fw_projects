#include <Wire.h>

#define MCP3421_ADDRESS 0x69 // Indirizzo I2C del MCP3421

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial); // Aspetta che il monitor seriale sia pronto
  //Serial.println("\nI2C Scanner e Lettura MCP3421");
}

void loop() {
  // Scanner I2C
  int nDevices = 0;
  //Serial.println("Scanning...");
  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      //Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      //Serial.print(address, HEX);
      //Serial.println("  !");
      ++nDevices;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    //Serial.println("done\n");
  }
  
  // Lettura MCP3421
  readMCP3421();
  
  delay(50); // Aspetta 5 secondi per il prossimo scan e lettura
}

/*-------------------------------------------------------------------------------------------------------------------------

  // Configurazione MCP3421 per iniziare la conversione (1-shot, 18-bit, PGA = 1)
  
  0 0 0 1   1 1 0 0
  | | | |   | | | |
  | | | |   | | | +-- Bit 0: RDY (Ready, sempre 0 in modalità One-Shot)
  | | | |   | | +---- Bit 1: C1 (Configurazione per modalità e guadagno)
  | | | |   | +------ Bit 2: C0 (Configurazione per modalità e guadagno)
  | | | |   +-------- Bit 3: S1 (Configurazione per risoluzione)
  | | +-------------- Bit 4: S0 (Configurazione per risoluzione)
  | +---------------- Bit 5: O/C (Mode of Operation: 1 = One-Shot Conversion Mode)
  +------------------ Bit 6: Gain Setting
  

  ------------------------S1 e S2 determinano la risoluzione dell'ADC ---------

  00 = 12-bit (240 SPS)
  01 = 14-bit (60 SPS)
  10 = 16-bit (15 SPS)
  11 = 18-bit (3.75 SPS)

  ------------------------Il bit O/C determina la modalità operativa:----------

  0 = Continuous Conversion Mode
  1 = One-Shot Conversion Mode

  ------------------------I bit G1 e G0 determinano il guadagno dell'ADC:------

  00 = Gain 1
  01 = Gain 2
  10 = Gain 4
  11 = Gain 8

-------------------------------------------------------------------------------------------------------------------------*/
  
  void readMCP3421() {
    
  Wire.beginTransmission(MCP3421_ADDRESS);
  Wire.write(0x4B); // Config register: 0x1C -> 00011100 (1-shot, 18-bit, PGA=1)
  Wire.endTransmission();

  delay(100); // Attendi la conversione (tempo massimo di conversione per 18-bit è 267ms)

  Wire.requestFrom(MCP3421_ADDRESS, 3);
  if (Wire.available() == 3) {
    byte msb = Wire.read();
    byte csb = Wire.read();
    byte lsb = Wire.read();

    long result = ((long)msb << 16) | ((long)csb << 8) | lsb;
    result = result >> 4; // Shift per avere i 18 bit significativi

    float voltage = (float)result * 0.000015625; // 15.625uV per LSB per un guadagno di 1

    //Serial.print("MCP3421 Reading: ");
    //Serial.print(result);
    //Serial.print(" -> Voltage: ");
    Serial.println(voltage, 6);
    //Serial.println(" V");
  } else {
    Serial.println("Error reading MCP3421");
  }
}
