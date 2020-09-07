#!/bin/bash
esptool.py --chip esp32 --port $1 --baud 230400 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x120000 data/prboom.wad
esptool.py --chip esp32 --port $1 --baud 230400 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x1A0000 data/DOOM1.WAD
