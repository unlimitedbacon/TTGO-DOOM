/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2006 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  DOOM graphics stuff for SDL
 *
 *-----------------------------------------------------------------------------
 */

#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include "prboom/m_argv.h"
#include "prboom/doomstat.h"
#include "prboom/doomdef.h"
#include "prboom/doomtype.h"
#include "prboom/v_video.h"
#include "prboom/r_draw.h"
#include "prboom/d_main.h"
#include "prboom/d_event.h"
#include "prboom/i_video.h"
#include "prboom/z_zone.h"
#include "prboom/s_sound.h"
#include "prboom/sounds.h"
#include "prboom/w_wad.h"
#include "prboom/st_stuff.h"
#include "prboom/lprintf.h"

#include "rom/ets_sys.h"
#include "prboom-esp32-compat/spi_lcd.h"
#include "prboom-esp32-compat/gamepad.h"
#include "prboom-esp32-compat/ttgo_c.h"

// #include "esp_heap_alloc_caps.h"

#define ACCEL_DEADZONE 150

int use_fullscreen=0;
int use_doublebuffer=0;

bool last_touched_state = false;
bool last_button_state = false;

uint16_t swipe_start_x;
uint16_t swipe_start_y;
uint16_t swipe_end_x;
uint16_t swipe_end_y;

int last_touch_event = 0;

void I_StartTic (void)
{
  // Read from game pad
	gamepadPoll();

  // Read button press
  ttgo_power_readirq();
  bool button = ttgo_pek_short_press();
  if (button != last_button_state)
  {
    event_t event;
    event.data1 = KEYD_ESCAPE;
    if (button) {
      event.type = ev_keydown;
    } else {
      event.type = ev_keyup;
    }
    last_button_state = button;
    D_PostEvent(&event);
  }
  ttgo_power_clearirq();

  // Send keyup for touch events on next tick
  if (last_touch_event)
  {
    event_t event;
    event.type = ev_keyup;
    event.data1 = last_touch_event;
    //lprintf(LO_INFO,"I_StartTic: Keyup %d\n", last_touch_event);
    D_PostEvent(&event);
    last_touch_event = 0;
  }

  // Read touchscreen input
  uint16_t touch_x;
  uint16_t touch_y;
  bool touched = ttgo_touch_get_point(&touch_x, &touch_y);

  // Detect touch events
  if (touched) {
    if (last_touched_state) {
      // Continuing previous gesture
      swipe_end_x = touch_x;
      swipe_end_y = touch_y;
    } else {
      // Starting new gesture
      swipe_start_x = swipe_end_x = touch_x;
      swipe_start_y = swipe_end_y = touch_y;
    }
  } else {
    if (last_touched_state) {
      // Ending gesture
      lprintf(LO_INFO,"I_StartTic: Swipe from %d,%d to %d,%d\n", swipe_start_x, swipe_start_y, swipe_end_x, swipe_end_y);
      int16_t dx = swipe_end_x - swipe_start_x;
      int16_t dy = swipe_end_y - swipe_start_y;
      event_t event;
      event.type = ev_keydown;
      // Check if this is a touch or a swipe
      if (abs(dx) > 4 || abs(dy) > 4)
      {
        // Swipe
        if ((dx > 0) && (abs(dx) > abs(dy))) {
          //Swipe right
          //lprintf(LO_INFO,"I_StartTic: Swipe right\n");
          event.data1 = KEYD_RIGHTARROW;
        } else if ((dx < 0) && (abs(dx) > abs(dy))) {
          //Swipe left
          //lprintf(LO_INFO,"I_StartTic: Swipe left\n");
          event.data1 = KEYD_LEFTARROW;
        } else if ((dy < 0) && (abs(dy) > abs(dx))) {
          //Swipe up
          //lprintf(LO_INFO,"I_StartTic: Swipe up\n");
          event.data1 = KEYD_UPARROW;
        } else if ((dy > 0) && (abs(dy) > abs(dx))) {
          //Swipe down
          //lprintf(LO_INFO,"I_StartTic: Swipe down\n");
          event.data1 = KEYD_DOWNARROW;
        }
      } else {
        // Touch
        // Double event. Send menu select and also shoot
        event.data1 = KEYD_ENTER;
        D_PostEvent(&event);
        event.type = ev_keydown;
        event.data1 = KEYD_RCTRL;
        lprintf(LO_INFO,"I_StartTic: FIRE\n");
      }
      last_touch_event = event.data1;
      D_PostEvent(&event);
    }

  }

  last_touched_state = touched;

  // Read touchscreen gestures
  // This does not appeare to be supported
  /*
  GesTrue_t gesture = ttgo_get_gesture();
  if (gesture != FOCALTECH_NO_GESTRUE)
  {
    event_t event;
    event.type = ev_keydown;
    switch (gesture)
    {
      case FOCALTECH_MOVE_UP:
        event.data1 = KEYD_UPARROW;
        lprintf(LO_INFO,"I_StartTic: gest up");
        D_PostEvent(&event);
        break;
      case FOCALTECH_MOVE_LEFT:
        event.data1 = KEYD_LEFTARROW;
        lprintf(LO_INFO,"I_StartTic: gest left");
        D_PostEvent(&event);
        break;
      case FOCALTECH_MOVE_DOWN:
        event.data1 = KEYD_DOWNARROW;
        lprintf(LO_INFO,"I_StartTic: gest down");
        D_PostEvent(&event);
        break;
      case FOCALTECH_MOVE_RIGHT:
        event.data1 = KEYD_RIGHTARROW;
        lprintf(LO_INFO,"I_StartTic: gest right");
        D_PostEvent(&event);
        break;
      case FOCALTECH_ZOOM_IN:
        lprintf(LO_INFO,"I_StartTic: gest zoomin");
        break;
      case FOCALTECH_ZOOM_OUT:
        lprintf(LO_INFO,"I_StartTic: gest zoomout");
        break;
      default:
        break;
    }
  }
  */

  // May be helpful for reading touch
  // https://android.googlesource.com/kernel/msm/+/android-7.0.0_r0.1/drivers/input/touchscreen/ft5x06_ts.c

  // Looks like the touchscreen and acceleromenter both support
  // interrupts. Might be better to use those?

  // Accelerometer joystick emulation
  int16_t accel_x;
  int16_t accel_y;
  int16_t accel_z;
  if (ttgo_get_accel(&accel_x, &accel_y, &accel_z))
  {
    //lprintf(LO_INFO,"I_StartTic: Accel X:%i Y:%i Z:%i\n", accel_x, accel_y, accel_z);
    // Deadzone
    if (abs(accel_x) < ACCEL_DEADZONE)
    {
      accel_x = 0;
    }
    if (abs(accel_y) < ACCEL_DEADZONE)
    {
      accel_y = 0;
    }
    //Smooth ramp up from edge of deadzone
    if (accel_x > 0) accel_x -= ACCEL_DEADZONE;
    if (accel_x < 0) accel_x += ACCEL_DEADZONE;
    if (accel_y > 0) accel_y -= ACCEL_DEADZONE;
    if (accel_y < 0) accel_y += ACCEL_DEADZONE;
    // Cap accel at 1/2 G
    if (accel_x > 512) accel_x = 512;
    if (accel_y > 512) accel_y = 512;
    event_t event;
    event.type = ev_joystick;
    event.data1 = 0;
    event.data2 = accel_x;
    event.data3 = accel_y;
    D_PostEvent(&event);
  }
}


static void I_InitInputs(void)
{
  //lprintf(LO_INFO,"I_InitInputs: Touch vendor id: %X\n", ttgo_touch_get_vendor_id());
  if (!ttgo_accel_init())
  {
    lprintf(LO_ERROR,"I_InitInputs: Couldn't initialize accelerometer");
  }
}



static void I_UploadNewPalette(int pal)
{
}

//////////////////////////////////////////////////////////////////////////////
// Graphics API

void I_ShutdownGraphics(void)
{
}

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
}


void I_StartFrame (void)
{
}


int I_StartDisplay(void)
{
	spi_lcd_wait_finish();
  return true;
}

void I_EndDisplay(void)
{
}



static uint16_t *screena, *screenb;


//
// I_FinishUpdate
//

void I_FinishUpdate (void)
{
	uint16_t *scr=(uint16_t*)screens[0].data;
#if 0
	int x, y;
	char *chrs=" '.~+mM@";
	ets_printf("\033[1;1H");
	for (y=0; y<240; y+=4) {
		for (x=0; x<320; x+=2) {
			ets_printf("%c", chrs[(scr[x+y*320])>>13]);
		}
		ets_printf("\n");
	}
#endif
#if 1 
	spi_lcd_send(scr);
#endif
	//Flip framebuffers
//	if (scr==screena) screens[0].data=screenb; else screens[0].data=screena;
}

int16_t lcdpal[256];

void I_SetPalette (int pal)
{
	int i, r, g, b, v;
	int pplump = W_GetNumForName("PLAYPAL");
	const byte * palette = W_CacheLumpNum(pplump);
	palette+=pal*(3*256);
	for (i=0; i<255 ; i++) {
		v=((palette[0]>>3)<<11)+((palette[1]>>2)<<5)+(palette[2]>>3);
		lcdpal[i]=(v>>8)+(v<<8);
//		lcdpal[i]=v;
		palette += 3;
	}
	W_UnlockLumpNum(pplump);
}


unsigned char *screenbuf;

#define INTERNAL_MEM_FB


void I_PreInitGraphics(void)
{
	lprintf(LO_INFO, "preinitgfx");
#ifdef INTERNAL_MEM_FB
	//screenbuf=pvPortMallocCaps(320*240, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
	screenbuf=malloc(320*240);
	assert(screenbuf);
#endif
}


// CPhipps -
// I_SetRes
// Sets the screen resolution
void I_SetRes(void)
{
  int i;

//  I_CalculateRes(SCREENWIDTH, SCREENHEIGHT);

  // set first three to standard values
  for (i=0; i<3; i++) {
    screens[i].width = SCREENWIDTH;
    screens[i].height = SCREENHEIGHT;
    screens[i].byte_pitch = SCREENPITCH;
    screens[i].short_pitch = SCREENPITCH / V_GetModePixelDepth(VID_MODE16);
    screens[i].int_pitch = SCREENPITCH / V_GetModePixelDepth(VID_MODE32);
  }

  // statusbar
  screens[4].width = SCREENWIDTH;
  screens[4].height = (ST_SCALED_HEIGHT+1);
  screens[4].byte_pitch = SCREENPITCH;
  screens[4].short_pitch = SCREENPITCH / V_GetModePixelDepth(VID_MODE16);
  screens[4].int_pitch = SCREENPITCH / V_GetModePixelDepth(VID_MODE32);

//Attempt at double-buffering. Does not work.
//  free(screena);
//  free(screenb);
//  screena=malloc(SCREENPITCH*SCREENHEIGHT);
//  screenb=malloc(SCREENPITCH*SCREENHEIGHT);


#ifdef INTERNAL_MEM_FB
  screens[0].not_on_heap=true;
  screens[0].data=screenbuf;
  assert(screens[0].data);
#endif

//  spi_lcd_init();

  lprintf(LO_INFO,"I_SetRes: Using resolution %dx%d\n", SCREENWIDTH, SCREENHEIGHT);
}

void I_InitGraphics(void)
{
  char titlebuffer[2048];
  static int    firsttime=1;

  if (firsttime)
  {
    firsttime = 0;

    atexit(I_ShutdownGraphics);
    lprintf(LO_INFO, "I_InitGraphics: %dx%d\n", SCREENWIDTH, SCREENHEIGHT);

    /* Set the video mode */
    I_UpdateVideoMode();

    /* Initialize the input system */
    I_InitInputs();
	gamepadInit();

  }
}


void I_UpdateVideoMode(void)
{
  int init_flags;
  int i;
  video_mode_t mode;

  lprintf(LO_INFO, "I_UpdateVideoMode: %dx%d\n", SCREENWIDTH, SCREENHEIGHT);

//    mode = VID_MODE16;
    mode = VID_MODE8;

  V_InitMode(mode);
  V_DestroyUnusedTrueColorPalettes();
  V_FreeScreens();

  I_SetRes();

  V_AllocScreens();

  R_InitBuffer(SCREENWIDTH, SCREENHEIGHT);

}
