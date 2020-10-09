/*
  CFP Datalogger

  Logs serial output data from CFP coffee machines

  The circuit:
  CFP         Datalogger
  ----------------------
  GND         GND
  Tx          Rx
  Rx          Tx

  created 04 Oct 2020
  by Vikram Seshadri

*/

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
int sd_flag = 0;
int sd_led = 8;

String base_filename = "CFP";
int file_number = 1;
String full_filename = " ";

void update_file_number()
{
  // See if the file already exists, if so, see if it has _###
  // If file doesn't exist, add _###
  // If file does exists, make _### 1 more
  full_filename = base_filename + String(file_number) + String(F(".txt"));
  while (SD.exists(full_filename)) {
    file_number += 1;
    full_filename = base_filename + String(file_number) + String(F(".txt"));
  }
}
void setup() {
  // initialize both serial ports:
  Serial.begin(57600);
  Serial1.begin(57600);
  pinMode (sd_led, OUTPUT);

  // see if the card is present and can be initialized:
  if (SD.begin(chipSelect)) {
    Serial.println("Card success");
    sd_flag = 1;
    update_file_number();
  }

}

void loop() {

  // read from port 1, send to port 0:
  if (Serial1.available()) {
    digitalWrite(sd_led, LOW);
    char inByte = Serial1.read();
    if (sd_flag == 1) {
      File dataFile = SD.open(full_filename, FILE_WRITE);
      if (dataFile) {
        digitalWrite(sd_led, HIGH);
        dataFile.print(inByte);
        dataFile.close();
      }

    }
    Serial.write(inByte);
    digitalWrite(sd_led, LOW);

  }
}
