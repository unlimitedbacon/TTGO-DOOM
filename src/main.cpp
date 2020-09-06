#include <Arduino.h>

#include "sdkconfig.h"

#include "esp_attr.h"

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/crc.h"

#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"
// #include "esp_partition.h"

extern "C" {
#include "prboom/i_system.h"
#include "prboom-esp32-compat/spi_lcd.h"
#include "prboom-esp32-compat/gamepad.h"
}

extern "C" void jsInit();

void doomEngineTask(void *pvParameters)
{
  char const *argv[]={"doom","-cout","ICWEFDA", NULL};
  doom_main(3, argv);
}

#include "ttgo-config.h"
#include <TTGO.h>

TTGOClass *ttgo;

void setup() {
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  // put your setup code here, to run once:
  //int i;
  //const esp_partition_t* part;
  //spi_flash_mmap_handle_t hdoomwad;
  //esp_err_t err;

	//part=esp_partition_find_first(66, 6, NULL);
	//if (part==0) printf("Couldn't find wad part!\n");

	spi_lcd_init();
	jsInit();
  TaskHandle_t task;
	xTaskCreatePinnedToCore(&doomEngineTask, "doomEngine", 22480, NULL, 5, &task, 0);
  //esp_task_wdt_add(task);
}

void loop() {
  // put your main code here, to run repeatedly:
}