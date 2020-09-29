#ifndef TTGO_C_H
#define TTGO_C_H

/*
typedef enum {
    FOCALTECH_NO_GESTRUE,
    FOCALTECH_MOVE_UP,
    FOCALTECH_MOVE_LEFT,
    FOCALTECH_MOVE_DOWN,
    FOCALTECH_MOVE_RIGHT,
    FOCALTECH_ZOOM_IN,
    FOCALTECH_ZOOM_OUT,
} GesTrue_t;
*/

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC bool ttgo_touched();
EXTERNC bool ttgo_touch_get_point(uint16_t* x, uint16_t* y);
//EXTERNC GesTrue_t ttgo_get_gesture();
//EXTERNC int ttgo_touch_get_vendor_id();
EXTERNC bool ttgo_power_readirq();
EXTERNC bool ttgo_power_clearirq();
EXTERNC bool ttgo_pek_short_press();
EXTERNC bool ttgo_accel_init();
EXTERNC bool ttgo_get_accel(int16_t* x, int16_t* y, int16_t* z);

#undef EXTERNC

#endif