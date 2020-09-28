#include "main.h"

/*
 * This is a simple wrapper for the C++ library functions
 * so they can be used in the DOOM engine, written in C.
 */

extern "C" bool ttgo_touched() {
    return ttgo->touched();
}

extern "C" bool ttgo_touch_get_point(uint16_t* x, uint16_t* y) {
    //return ttgo->touch->getPoint(*x, *y);
    if (ttgo->touched())
    {
        TP_Point tp = ttgo->touch->getPoint(0);
        *x = tp.x;
        *y = tp.y;
        return true;
    } else {
        return false;
    }
    
}

/*
extern "C" GesTrue_t ttgo_get_gesture() {
    return ttgo->touch->getGesture();
}
*/

/*
extern "C" int ttgo_touch_get_vendor_id() {
    return ttgo->touch->getVendorID();
}
*/

extern "C" void ttgo_power_readirq()
{
    ttgo->power->readIRQ();
}

extern "C" void ttgo_power_clearirq()
{
    ttgo->power->clearIRQ();
}

/*
bool ttgo_pek_rising()
{
    ttgo->power->rising
}
*/

extern "C" bool ttgo_pek_short_press() {
    return ttgo->power->isPEKShortPressIRQ();
}