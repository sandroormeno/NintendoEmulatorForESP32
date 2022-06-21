/* Arduino Nofrendo
   Please check hw_config.h and display.cpp for configuration details
*/
#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <FFat.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>

#include "hw_config.h"

extern "C"
{
#include <nofrendo.h>
}

int16_t bg_color;
extern Arduino_TFT *gfx;
extern void display_begin();

void setup()
{
  Serial.begin(115200);


  //#if defined(ARDUINO_M5Stack_Core_ESP32) || defined(ARDUINO_M5STACK_FIRE)
  //    Serial.println("ARDUINO_M5Stack_Core_ESP32");
  //
  //#elif defined(ARDUINO_ODROID_ESP32)
  //    Serial.println("ARDUINO_ODROID_ESP32");
  //
  //#elif defined(ARDUINO_T) || defined(ARDUINO_TWATCH_BASE) || defined(ARDUINO_TWATCH_2020_V1) || defined(ARDUINO_TWATCH_2020_V2) // TTGO T-Watch
  //    Serial.println("ARDUINO_TWATCH_BASE");
  //#else
  //    Serial.println("ESP32 normal");
  //#endif /* custom hardware */


  // turn off WiFi
  esp_wifi_deinit();

  // disable Core 0 WDT
  TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
  esp_task_wdt_delete(idle_0);

  // start display
  display_begin();

  // filesystem defined in hw_config.h
  FILESYSTEM_BEGIN

  // find first rom file (*.nes)
  File root = filesystem.open("/");
  char *argv[1];
  if (!root)
  {
    Serial.println("Filesystem mount failed! Please check hw_config.h settings.");
    gfx->println("Filesystem mount failed! Please check hw_config.h settings.");
  }
  else
  {
    bool foundRom = false;
    //gfx->println("It is a success!!!!.");
    File file = root.openNextFile();
    while (file)
    {
      if (file.isDirectory())
      {
        // skip
      }
      else
      {
        char *filename = (char *)file.name();
        int8_t len = strlen(filename);
        if (strstr(strlwr(filename + (len - 4)), ".nes"))
        {
          foundRom = true;
          char fullFilename[256];
          sprintf(fullFilename, "%s%s", FSROOT, filename);
          Serial.println(fullFilename);
          //gfx->println(fullFilename);
          argv[0] = fullFilename;
          break;
        }
      }

      file = root.openNextFile();
    }

    if (!foundRom)
    {
      Serial.println("Failed to find rom file, please copy rom file to data folder and upload with \"ESP32 Sketch Data Upload\"");
      argv[0] = "/";
    }

    Serial.println("NoFrendo start!\n");
    //gfx->println("we are starting...!!!");
    
    nofrendo_main(1, argv);
    Serial.println("NoFrendo end!\n");
  }
}

void loop()
{
}
