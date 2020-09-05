# set(COMPONENT_REQUIRES spi_flash)


idf_component_register(SRCS 
 gamepad.c
 i_main.c
 i_network.c
 i_sound.c
 i_system.c
 i_video.c
 include
 mcp23017.c
 psxcontroller.c
 spi_lcd.c
 INCLUDE_DIRS "." "include" "../prboom/include" "../mcp23x17" 
 PRIV_REQUIRES spiffs vfs fatfs prboom mcp23x17 prboom-wad-tables)


