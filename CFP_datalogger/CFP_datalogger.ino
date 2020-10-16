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


char inByte = " ";

const int chipSelect = 4;
int sd_flag = 0;
int sd_led = 8;

String dataString = "";

/*************************************************************/
/* SD Class */
class CSVFile
{
  public:
    char filenamechar[100];
    File dataFile;
    //SdFat SD;
    String base_filename;
    int file_number;
    String full_filename;

    CSVFile(String bfilename)
    {
      base_filename = bfilename;
      file_number = 1;
      full_filename = bfilename + String(file_number) + String(F(".csv"));
      full_filename.toCharArray(filenamechar, 100);
    }

    void beginNow()
    {
      SD.begin(chipSelect);
    }

    void openFile()
    {
      dataFile = SD.open(filenamechar, FILE_WRITE);
    }

    void closeFile()
    {
      dataFile.close();
    }

    void writeToCSV(String data)
    {
      dataFile.print(data);
//      dataFile.print(String(F("\r\n")));
    }

    void writeHeaders()
    {
      dataFile.println(String(F("Outlet Temp, Boiler Temp,Warm Plate Temp,Max Temp,Calibrated Offset Temp,Pump PWN, Boiler On/Off, PTC On/Off, Flow Rate, Current Block Volume, Total Block Volume, Clean Count, Recipe Size, Recipe Brew, Recipe Block, Recipe Total Volume, Recipe Time "))); //Headers
    }

    void _update_file_number()
    {
      // See if the file already exists, if so, see if it has _###
      // If file doesn't exist, add _###
      // If file does exists, make _### 1 more

      while (SD.exists(filenamechar)) {
        file_number += 1;
        full_filename = base_filename + String(file_number) + String(F(".csv"));
        full_filename.toCharArray(filenamechar, 100);
      }
    }
};

//setup csv file object
String filename = "CFP";
CSVFile csv(filename);

void setup() {

  // initialize both serial ports:
  Serial1.begin(57600);
  Serial.begin(57600);
  pinMode (sd_led, OUTPUT);
  digitalWrite(sd_led, LOW);

  // see if the card is present and can be initialized:
  if (SD.begin(chipSelect)) {
    csv.beginNow();
    csv._update_file_number();
    csv.openFile();
    csv.writeHeaders();
    csv.closeFile();
    Serial.println("Card Present");
  }
  //  Serial.println(FreeRam());

}

void loop() {

  // read from port 1, send to port 0:
  if (Serial1.available()) {

    inByte = Serial1.read();
    dataString += inByte;

//    Serial.print(dataString);
    if (inByte == '\n') {
      Serial.print(dataString);
      if (SD.begin(chipSelect)) {
        csv.openFile();

        digitalWrite(sd_led, HIGH);
        csv.writeToCSV(dataString);
        
        csv.closeFile();
        dataString = "";
      }
    }
    digitalWrite(sd_led, LOW);
  }
}
