TTGO-DOOM
=========

This is a port of DOOM to the [LilyGO T-Watch](https://www.tindie.com/products/ttgo/lilygor-ttgo-t-watch-2020/), a cheap ESP32 based "smart" watch. I have only tried it on the T-Watch-2020, but it may work on other models.

![doomwatch](https://user-images.githubusercontent.com/3131268/92416444-5cfcd400-f112-11ea-84e7-c824d945da85.png)

Building / Flashing
-------------------

This is a [PlatformIO](https://platformio.org/) project. You can build it through the PlatformIO IDE (vscode) or from the command line using

```
pio run -t upload
```

After building and uploading the firmware, you will also need to upload the game data (WAD files). Run the `flashwad.sh` script. This requires [esptool](https://github.com/espressif/esptool) to be installed. You will need to specify the serial port the device is connected to, for instance `/dev/ttyUSB0`.

```
./flashwad.sh /dev/ttyUSB0
```

I have included the WAD file for the shareware version of DOOM (`data/DOOM1.WAD`), but you can replace it with the WAD for a retail version if you have it. There is 14,720 Kb available for the WAD file.

TODO
----
- [ ] Touchscreen gestures
- [ ] Accelerometer for movement control
- [ ] Sound
- [ ] Load WAD files from SPIFFS filesystem instead of reading data directly from flash partitions
- [ ] Save / Load games
- [ ] Run vibration motor when you get hit
- [ ] Multiplayer using WiFi
- [ ] Gamepad using bluetooth
- [ ] Maybe show the time?

Credits
-------
* DOOM was released by iD software in 1999 under the Gnu GPL.
* [PrBoom](http://prboom.sourceforge.net/) is a modification of this code; its authors are credited in the src/prboom/AUTHORS file.
* [The ESP32 port](https://github.com/espressif/esp32-doom) was done by Espressif and is licensed under the Apache license, version 2.0.
* Also uses code from [App-Z](https://github.com/app-z/esp32-doom) and [jkirsons](https://github.com/jkirsons/doom-espidf).
