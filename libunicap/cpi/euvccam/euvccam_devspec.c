/*
  unicap euvccam plugin

  Copyright (C) 2009  Arne Caspari

  Compilation, installation or use of this program requires a written license. 

  By compilling, installing or using this software you agree to accept the terms 
  of the license as specified in the COPYING file in the root folder of this 
  software package.
 */

#include <unicap.h>

#include "euvccam_cpi.h"
#include "euvccam_device.h"
#include "euvccam_devspec.h"
#include "euvccam_colorproc.h"

#define N_ELEMENTS(a) (sizeof( a )/ sizeof(( a )[0] ))
#define FOURCC(a,b,c,d) (unsigned int)((((unsigned int)d)<<24)+(((unsigned int)c)<<16)+(((unsigned int)b)<<8)+a)
#define N_(x) x

#define TRIGGER_FREE_RUNNING "free running"
#define TRIGGER_FALLING_EDGE "trigger on falling edge"
#define TRIGGER_RISING_EDGE  "trigger on rising edge"

static char* trigger_menu[] = 
{
   N_(TRIGGER_FREE_RUNNING), 
   N_(TRIGGER_RISING_EDGE)
};

static unicap_rect_t euvccam_WVGA_sizes[] =
{
   { 0,0,640,480 },
   { 0,0,744,480 },
};   



static double format_8201_framerates[] = {    30, 15, 7.5, 3.75 };
static int    format_8201_framerate_map[] = {  0,  1,   2,    3 };

static euvccam_property_t properties_8201[] =
{
   {
      {
	identifier: "Shutter", 
	category: "Exposure",
	unit: "s",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0.0001, max: 4 } },
	stepping: 0.0001,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_exposure,
      set_func: euvccam_device_set_exposure,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "Gain", 
	category: "Exposure",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 16, max: 63 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_gain,
      set_func: euvccam_device_set_gain,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "White Balance Mode", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0, max: 0 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_white_balance_mode,
      set_func: euvccam_device_set_white_balance_mode,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "White Balance Red", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 63}, 
	{range: { min: 0, max: 255 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_white_balance,
      set_func: euvccam_device_set_white_balance,
      enumerate_func: NULL,
   },   
   {
      {
	identifier: "White Balance Blue", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 63}, 
	{range: { min: 0, max: 255 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_white_balance,
      set_func: euvccam_device_set_white_balance,
      enumerate_func: NULL,
   },   
/*    { */
/*       { */
/* 	identifier: "Frame Rate",  */
/* 	category: "Video", */
/* 	unit: "", */
/* 	relations: NULL, */
/* 	relations_count: 0,  */
/* 	{value: 0},  */
/* 	{range: { min: 10, max: 63 } }, */
/* 	stepping: 1.0, */
/* 	type: UNICAP_PROPERTY_TYPE_VALUE_LIST, */
/* 	flags: UNICAP_FLAGS_MANUAL, */
/* 	flags_mask: UNICAP_FLAGS_MANUAL, */
/* 	property_data: NULL,  */
/* 	property_data_size: 0, */
/*       }, */
      
/*       get_func: euvccam_device_get_frame_rate, */
/*       set_func: euvccam_device_set_frame_rate, */
/*       enumerate_func: euvccam_device_enumerate_frame_rate, */
/*    }, */

#if 1
   {
      {
	identifier: "Pixel Clock", 
	category: "Video",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 5, max: 120 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_pixel_clock,
      set_func: euvccam_device_set_pixel_clock,
      enumerate_func: NULL,
   },
#endif
   {
      {
	identifier: N_("trigger"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	{menu_item: TRIGGER_FREE_RUNNING},
	{menu: { menu_items: trigger_menu, 
		 menu_item_count: sizeof( trigger_menu ) / sizeof( char * ) } },
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_MENU,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_trigger,
      set_func: euvccam_device_set_trigger,
      enumerate_func: euvccam_device_enumerate_trigger,
   },
   {
      {
	identifier: N_("reset sensor"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_reset_mt9v024,
      set_func: euvccam_device_set_reset_mt9v024,
   },
   {
      {
	identifier: "IIC Command", 
	category: "Device",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 5, max: 120 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_DATA,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_read_iic,
      set_func: euvccam_device_write_iic,
      enumerate_func: NULL,
   },
};

static euvccam_property_t properties_8206[] =
{
   {
      {
	identifier: "Shutter", 
	category: "Exposure",
	unit: "s",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0.0001, max: 4 } },
	stepping: 0.0001,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_exposure,
      set_func: euvccam_device_set_exposure,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "Gain", 
	category: "Exposure",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0, max: 255 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_gain,
      set_func: euvccam_device_set_gain,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "White Balance Mode", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0, max: 0 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_white_balance_mode,
      set_func: euvccam_device_set_white_balance_mode,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "White Balance Red", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 63}, 
	{range: { min: 0, max: 255 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_white_balance,
      set_func: euvccam_device_set_white_balance,
      enumerate_func: NULL,
   },   
   {
      {
	identifier: "White Balance Blue", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 63}, 
	{range: { min: 0, max: 255 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_white_balance,
      set_func: euvccam_device_set_white_balance,
      enumerate_func: NULL,
   },   
   {
      {
	identifier: "Frame Rate", 
	category: "Video",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0, max: 0 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_VALUE_LIST,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_frame_rate,
      set_func: euvccam_device_set_frame_rate,
      enumerate_func: euvccam_device_enumerate_frame_rate,
   },
   {
      {
	identifier: N_("trigger"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	{menu_item: TRIGGER_FREE_RUNNING},
	{menu: { menu_items: trigger_menu, 
		 menu_item_count: sizeof( trigger_menu ) / sizeof( char * ) } },
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_MENU,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_trigger,
      set_func: euvccam_device_set_trigger,
      enumerate_func: euvccam_device_enumerate_trigger,
   },

#if 1
   {
      {
	identifier: "Pixel Clock", 
	category: "Video",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 5, max: 120 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_pixel_clock,
      set_func: euvccam_device_set_pixel_clock,
      enumerate_func: NULL,
   },
#endif
   {
      {
	identifier: "IIC Command", 
	category: "Device",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 5, max: 120 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_DATA,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_read_iic,
      set_func: euvccam_device_write_iic,
      enumerate_func: NULL,
   },
};

static euvccam_property_t properties_8202_color[] =
{
   {
      {
	identifier: "Shutter", 
	category: "Exposure",
	unit: "s",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0.0001, max: 4 } },
	stepping: 0.0001,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_exposure,
      set_func: euvccam_device_set_exposure,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "Gain", 
	category: "Exposure",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 16, max: 63 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_gain,
      set_func: euvccam_device_set_gain,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "Frame Rate", 
	category: "Video",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0, max: 0 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_VALUE_LIST,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_frame_rate,
      set_func: euvccam_device_set_frame_rate,
      enumerate_func: euvccam_device_enumerate_frame_rate,
   },
   {
      {
	identifier: "White Balance Mode", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0.0}, 
	{range: { min: 0.0, max: 0.0 } },
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO | UNICAP_FLAGS_ONE_PUSH,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_colorproc_get_wbgain_mode,
      set_func: euvccam_colorproc_set_wbgain_mode,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "White Balance Blue", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 1.0}, 
	{range: { min: 0.0, max: 8.0 } },
	stepping: 0.01,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_colorproc_get_wbgain,
      set_func: euvccam_colorproc_set_wbgain,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "White Balance Red", 
	category: "Color",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 1.0}, 
	{range: { min: 0.0, max: 8.0 } },
	stepping: 0.01,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_colorproc_get_wbgain,
      set_func: euvccam_colorproc_set_wbgain,
      enumerate_func: NULL,
   },
   {
      {
	identifier: N_("trigger"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	{menu_item: TRIGGER_FREE_RUNNING},
	{menu: { menu_items: trigger_menu, 
		 menu_item_count: sizeof( trigger_menu ) / sizeof( char * ) } },
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_MENU,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_trigger,
      set_func: euvccam_device_set_trigger,
      enumerate_func: euvccam_device_enumerate_trigger,
   },
   {
      {
	identifier: N_("software trigger"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_software_trigger,
      set_func: euvccam_device_set_software_trigger,
      enumerate_func: euvccam_device_enumerate_software_trigger,
   },
   {
      {
	identifier: N_("reset sensor"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_reset_mt9v024,
      set_func: euvccam_device_set_reset_mt9v024,
   },
   {
      {
	identifier: "IIC Command", 
	category: "Device",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 5, max: 120 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_DATA,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_read_iic,
      set_func: euvccam_device_write_iic,
      enumerate_func: NULL,
   },

};

static euvccam_property_t properties_8202_mono[] =
{
   {
      {
	identifier: "Shutter", 
	category: "Exposure",
	unit: "s",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0.0001, max: 4 } },
	stepping: 0.0001,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_exposure,
      set_func: euvccam_device_set_exposure,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "Gain", 
	category: "Exposure",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 16, max: 63 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_RANGE,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL | UNICAP_FLAGS_AUTO,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_gain,
      set_func: euvccam_device_set_gain,
      enumerate_func: NULL,
   },
   {
      {
	identifier: "Frame Rate", 
	category: "Video",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 0, max: 0 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_VALUE_LIST,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_get_frame_rate,
      set_func: euvccam_device_set_frame_rate,
      enumerate_func: euvccam_device_enumerate_frame_rate,
   },
   {
      {
	identifier: N_("trigger"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	{menu_item: TRIGGER_FREE_RUNNING},
	{menu: { menu_items: trigger_menu, 
		 menu_item_count: sizeof( trigger_menu ) / sizeof( char * ) } },
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_MENU,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_trigger,
      set_func: euvccam_device_set_trigger,
      enumerate_func: euvccam_device_enumerate_trigger,
   },
   {
      {
	identifier: N_("software trigger"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_software_trigger,
      set_func: euvccam_device_set_software_trigger,
      enumerate_func: euvccam_device_enumerate_software_trigger,
   },
   {
      {
	identifier: N_("reset sensor"),
	category: N_("device"),
	unit: "",
	relations: NULL,
	relations_count: 0,
	stepping: 0.0,
	type: UNICAP_PROPERTY_TYPE_FLAGS,
	flags: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_ONE_PUSH | UNICAP_FLAGS_MANUAL,
	property_data: NULL,
	property_data_size: 0
      },
      
      get_func: euvccam_device_get_reset_mt9v024,
      set_func: euvccam_device_set_reset_mt9v024,
   },
   {
      {
	identifier: "IIC Command", 
	category: "Device",
	unit: "",
	relations: NULL,
	relations_count: 0, 
	{value: 0}, 
	{range: { min: 5, max: 120 } },
	stepping: 1.0,
	type: UNICAP_PROPERTY_TYPE_DATA,
	flags: UNICAP_FLAGS_MANUAL,
	flags_mask: UNICAP_FLAGS_MANUAL,
	property_data: NULL, 
	property_data_size: 0,
      },
      
      get_func: euvccam_device_read_iic,
      set_func: euvccam_device_write_iic,
      enumerate_func: NULL,
   },

};

static struct euvccam_video_format_description formats_8201[] =
{
   {
      2,
      2,
      { identifier: "UYVY 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 640 * 480 * 2,
	fourcc: FOURCC( 'U', 'Y', 'V', 'Y' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8201_framerates ), 
      format_8201_framerates,
      format_8201_framerate_map,
      640 * 480 * 2,
      NULL,
   },
   {
      2,
      1,
      { identifier: "UYVY 744x480",
	size: {0,0,744,480},
	min_size: { 0,0,744,480 },
	max_size: { 0,0,744,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 744 * 480 * 2,
	fourcc: FOURCC( 'U', 'Y', 'V', 'Y' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8201_framerates ), 
      format_8201_framerates,
      format_8201_framerate_map,
      744 * 480 * 2, 
      NULL, 
   },
   {
      1,
      2,
      { identifier: "YUYV 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 640 * 480 * 2,
	fourcc: FOURCC( 'Y', 'U', 'Y', 'V' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8201_framerates ), 
      format_8201_framerates,
      format_8201_framerate_map,
      640 * 480 * 2, 
      NULL, 
   },
   {
      1,
      1,
      { identifier: "YUYV 744x480",
	size: {0,0,744,480},
	min_size: { 0,0,744,480 },
	max_size: { 0,0,744,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 744 * 480 * 2,
	fourcc: FOURCC( 'Y', 'U', 'Y', 'V' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8201_framerates ), 
      format_8201_framerates,
      format_8201_framerate_map,
      744 * 480 * 2, 
      NULL, 
   },
      
};


static double format_8202_framerates[] = {   60, 30, 25, 15, 7.5, 5};
static int    format_8202_framerate_map[] = { 6,  0,  1,  2,   3, 4};


static struct euvccam_video_format_description formats_8202_color[] =
{
/*    { */
/*       1, */
/*       2, */
/*       { identifier: "Y800 104x104", */
/* 	size: {0,0,104,104}, */
/* 	min_size: { 0,0,104,104 }, */
/* 	max_size: { 0,0,104,104 }, */
/* 	h_stepping: 0, */
/* 	v_stepping: 0, */
/* 	sizes: NULL, */
/* 	size_count: 0, */
/* 	bpp: 8, */
/* 	buffer_size: 104 * 104, */
/* 	fourcc: FOURCC( 'Y', '8', '0', '0' ), */
/* 	flags: 0, */
/* 	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, */
/* 	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT, */
/* 	buffer_type: 0 */
/*       }, */
/*       N_ELEMENTS( format_8202_framerates ), */
/*       format_8202_framerates, */
/*       format_8202_framerate_map, */
/*       104 * 104, */
/*       NULL, */
/*       EUVCCAM_FORMAT_IS_PARTIAL_SCAN, */
/*    }, */
   {
      1,
      2,
      { identifier: "RGB24 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 640 * 480 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      640 * 480, 
      euvccam_colorproc_by8_rgb24_nn,
   },
   {
      1,
      1,
      { identifier: "RGB24 744x480",
	size: {0,0,744,480},
	min_size: { 0,0,744,480 },
	max_size: { 0,0,744,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 744 * 480 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      744 * 480, 
      euvccam_colorproc_by8_rgb24_nn,
   },
   {
      1,
      2,
      { identifier: "Y800 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 640 * 480,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      640 * 480, 
      NULL, 
   },
   {
      1,
      1,
      { identifier: "Y800 744x480",
	size: {0,0,744,480},
	min_size: { 0,0,744,480 },
	max_size: { 0,0,744,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 744 * 480,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      744 * 480, 
      NULL, 
   },
};


static struct euvccam_video_format_description formats_8202_mono[] =
{
   {
      1,
      2,
      { identifier: "Y800 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 640 * 480,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      640 * 480, 
      NULL,
   },
   {
      1,
      1,
      { identifier: "Y800 744x480",
	size: {0,0,744,480},
	min_size: { 0,0,744,480 },
	max_size: { 0,0,744,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 744 * 480,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      744 * 480, 
      NULL, 
   },
};

static double format_8203_1024_framerates[] = {   60, 30, 15, 7.5};
static int    format_8203_1024_framerate_map[] = { 0,  1,  2,   3};
static double format_8203_2048_framerates[] = {   60, 30, 15, 7.5};
static int    format_8203_2048_framerate_map[] = { 0,  1,  2,   3};

static struct euvccam_video_format_description formats_8203_color[] =
{
   {
      1,
      2,
      { identifier: "RGB24 1024x768",
	size: {0,0,1024,768},
	min_size: { 0,0,1024,768 },
	max_size: { 0,0,1024,768 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 1024 * 768 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8203_1024_framerates ), 
      format_8203_1024_framerates,
      format_8203_1024_framerate_map,
      1024 * 768, 
      euvccam_colorproc_by8_gr_rgb24_nn,
   },
   {
      1,
      1,
      { identifier: "RGB24 2048x1536",
	size: {0,0,2048,1536},
	min_size: { 0,0,2048,1536 },
	max_size: { 0,0,2048,1536 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 2048 * 1536 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8203_2048_framerates ), 
      format_8203_2048_framerates,
      format_8203_2048_framerate_map,
      2048 * 1536, 
      euvccam_colorproc_by8_gr_rgb24_nn,
   },
   {
      1,
      2,
      { identifier: "Y800 1024x768",
	size: {0,0,1024,768},
	min_size: { 0,0,1024,768 },
	max_size: { 0,0,1024,768 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 1024 * 768,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8203_1024_framerates ), 
      format_8203_1024_framerates,
      format_8203_1024_framerate_map,
      1024 * 768, 
      NULL, 
   },
   {
      1,
      1,
      { identifier: "Y800 2048x1536",
	size: {0,0,2048,1536},
	min_size: { 0,0,2048,1536 },
	max_size: { 0,0,2048,1536 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 2048 * 1536,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8203_2048_framerates ), 
      format_8203_2048_framerates,
      format_8203_2048_framerate_map,
      2048 * 1536, 
      NULL, 
   },
};
static double format_8204_framerates[] = {   27.02, 20, 15, 7.5};
static int    format_8204_framerate_map[] = { 0,  1,  2,   3};

static struct euvccam_video_format_description formats_8204_mono[] =
{
   {
      1,
      2,
      { identifier: "Y800 1280x1024",
	size: {0,0,1280,1024},
	min_size: { 0,0,1280,1024 },
	max_size: { 0,0,1280,1024 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 1280 * 1024,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8204_framerates ), 
      format_8204_framerates,
      format_8204_framerate_map,
      1280 * 1024, 
      NULL,
   },
   {
      1,
      2,
      { identifier: "Y800 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 640 * 480,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8204_framerates ), 
      format_8204_framerates,
      format_8204_framerate_map,
      640 * 480, 
      NULL,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
};


static double format_8205_framerates[] = {    30, 15, 7.5, 3.75 };
static int    format_8205_framerate_map[] = {  0,  1,   2,    3 };

static struct euvccam_video_format_description formats_8205[] =
{
   {
      1,
      1,
      { identifier: "Y800 1600x1200",
	size: {0,0,1600,1200},
	min_size: { 0,0,1600,1200 },
	max_size: { 0,0,1600,1200 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 1600 * 1200,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8205_framerates ), 
      format_8205_framerates,
      format_8205_framerate_map,
      1600 * 1200,
      NULL,
   },
   {
      1,
      1,
      { identifier: "RGB24 1600x1200",
	size: {0,0,1600,1200},
	min_size: { 0,0,32,32 },
	max_size: { 0,0,1600,1200 },
	h_stepping: 4,
	v_stepping: 4, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 1600 * 1200 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8205_framerates ), 
      format_8205_framerates,
      format_8205_framerate_map,
      1600 * 1200,
      euvccam_colorproc_by8_rgb24_nn,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,

   },
   {
      2,
      2,
      { identifier: "UYVY 1024x768",
	size: {0,0,1024,768},
	min_size: { 0,0,1024,768 },
	max_size: { 0,0,1024,768 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 16,
	buffer_size: 1024 * 768 * 2,
	fourcc: FOURCC( 'U', 'Y', 'V', 'Y' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8205_framerates ), 
      format_8205_framerates,
      format_8205_framerate_map,
      1024 * 768 * 2, 
      NULL, 
   },
   {
      2,
      4,
      { identifier: "UYVY 640x480",
	size: {0,0,640,480},
	min_size: { 0,0,640,480 },
	max_size: { 0,0,640,480 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 16,
	buffer_size: 640 * 480 * 2,
	fourcc: FOURCC( 'U', 'Y', 'V', 'Y' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8205_framerates ), 
      format_8205_framerates,
      format_8205_framerate_map,
      640 * 480 * 2, 
      NULL, 
   },
   {
      1,
      1,
      { identifier: "YUYV 1600x1200",
	size: {0,0,1600,1200},
	min_size: { 0,0,1600,1200 },
	max_size: { 0,0,1600,1200 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 1600 * 1200 * 2,
	fourcc: FOURCC( 'Y', 'U', 'Y', 'V' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8205_framerates ), 
      format_8205_framerates,
      format_8205_framerate_map,
      1600 * 1200 * 2, 
      NULL, 
   },
   {
      1,
      2,
      { identifier: "YUYV 1024x768",
	size: {0,0,1024,768},
	min_size: { 0,0,1024,768 },
	max_size: { 0,0,1024,768 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 1024 * 768 * 2,
	fourcc: FOURCC( 'Y', 'U', 'Y', 'V' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8205_framerates ), 
      format_8205_framerates,
      format_8205_framerate_map,
      1024 * 768 * 2, 
      NULL, 
   },
      
};

static double format_8206_framerates[] = {    30, 15, 7.5, 3.75 };
static int    format_8206_framerate_map[] = {  0,  1,   2,    3 };

static struct euvccam_video_format_description formats_8206[] =
{
   {
      1,
      1,
      { identifier: "RGB24 1280x1024",
	size: {0,0,1280,1024},
	min_size: { 0,0,1280,1024 },
	max_size: { 0,0,1280,1024 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 1280 * 1024 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      1280 * 1024, 
      euvccam_colorproc_by8_rgb24_nn_be,
   },
   {
      1,
      1,
      { identifier: "Y800 1280x1024",
	size: {0,0,1280,1024},
	min_size: { 0,0,1280,1024 },
	max_size: { 0,0,1280,1024 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 1280 * 1024,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8202_framerates,
      format_8202_framerate_map,
      1280 * 1024, 
      NULL,
   },
   {
      2,
      1,
      { identifier: "UYVY 1280x1024",
	size: {0,0,1280,1024},
	min_size: { 0,0,4,4 },
	max_size: { 0,0,1280,1024 },
	h_stepping: 4,
	v_stepping: 4, 
	sizes: NULL,
	size_count: 0,
	bpp: 16,
	buffer_size: 1280 * 1024 * 2,
	fourcc: FOURCC( 'U', 'Y', 'V', 'Y' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8206_framerates ), 
      format_8206_framerates,
      format_8206_framerate_map,
      1280 * 1024 * 2,
      NULL,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
   {
      2,
      2,
      { identifier: "UYVY 1024x768",
	size: {0,0,1024,768},
	min_size: { 0,0,1024,768 },
	max_size: { 0,0,1024,768 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 16,
	buffer_size: 1024 * 768 * 2,
	fourcc: FOURCC( 'U', 'Y', 'V', 'Y' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8206_framerates ), 
      format_8206_framerates,
      format_8206_framerate_map,
      1024 * 768 * 2, 
      NULL, 
   },
   {
      1,
      1,
      { identifier: "YUYV 1280x960",
	size: {0,0,1280,960},
	min_size: { 0,0,1280,960 },
	max_size: { 0,0,1280,960 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 1280 * 960 * 2,
	fourcc: FOURCC( 'Y', 'U', 'Y', 'V' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8206_framerates ), 
      format_8206_framerates,
      format_8206_framerate_map,
      1280 * 960 * 2, 
      NULL, 
   },
   {
      1,
      2,
      { identifier: "YUYV 1024x768",
	size: {0,0,1024,768},
	min_size: { 0,0,1024,768 },
	max_size: { 0,0,1024,768 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: euvccam_WVGA_sizes,
	size_count: N_ELEMENTS( euvccam_WVGA_sizes ),
	bpp: 16,
	buffer_size: 1024 * 768 * 2,
	fourcc: FOURCC( 'Y', 'U', 'Y', 'V' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8206_framerates ), 
      format_8206_framerates,
      format_8206_framerate_map,
      1024 * 768 * 2, 
      NULL, 
   },
      
};

static double format_8207_framerates[] = {    30, 15, 7.5, 3.75 };
static int    format_8207_framerate_map[] = {  0,  1,   2,    3 };

static struct euvccam_video_format_description formats_8207_color[] =
{
   {
      1,
      1,
      { identifier: "RGB24 2592x1944",
	size: {0,0,2592,1944},
	min_size: { 0,0,2592,1944 },
	max_size: { 0,0,2592,1944 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 2592 * 1944 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8207_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592 * 1944, 
      euvccam_colorproc_by8_gr_rgb24_nn/* euvccam_colorproc_by8_rgb24_nn_be */,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
   {
      1,
      1,
      { identifier: "RGB24 2x Binning",
	size: {0,0,2592/2,1944/2},
	min_size: { 0,0,16,16 },
	max_size: { 0,0,2592/2,1944/2 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 2592/2 * 1944/2 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8207_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592/2 * 1944/2, 
      euvccam_colorproc_by8_gr_rgb24_nn/* euvccam_colorproc_by8_rgb24_nn_be */,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
   {
      1,
      1,
      { identifier: "RGB24 4x Binning",
	size: {0,0,2592/4,1944/4},
	min_size: { 0,0,16,16 },
	max_size: { 0,0,2592/4,1944/4 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 2592/4 * 1944/4 * 3,
	fourcc: FOURCC( 'R', 'G', 'B', '3' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8207_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592/4 * 1944/4, 
      euvccam_colorproc_by8_gr_rgb24_nn/* euvccam_colorproc_by8_rgb24_nn_be */,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
   {
      1,
      1,
      { identifier: "Y800 2592x1944",
	size: {0,0,2592,1944},
	min_size: { 0,0,2592,1944 },
	max_size: { 0,0,2592,1944 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 2592 * 1944,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592 * 1944, 
      NULL,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },      
};

static struct euvccam_video_format_description formats_8207_mono[] =
{
   {
      1,
      1,
      { identifier: "Y800",
	size: {0,0,2592,1944},
	min_size: { 0,0,2592,1944 },
	max_size: { 0,0,2592,1944 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 8,
	buffer_size: 2592 * 1944,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8202_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592 * 1944, 
      NULL,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },      
   {
      1,
      1,
      { identifier: "Y800 2x Binning",
	size: {0,0,2592/2,1944/2},
	min_size: { 0,0,16,16 },
	max_size: { 0,0,2592/2,1944/2 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 2592/2 * 1944/2,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8207_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592/2 * 1944/2, 
      NULL,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
   {
      1,
      1,
      { identifier: "Y800 4x Binning",
	size: {0,0,2592/4,1944/4},
	min_size: { 0,0,16,16 },
	max_size: { 0,0,2592/4,1944/4 },
	h_stepping: 0,
	v_stepping: 0, 
	sizes: NULL,
	size_count: 0,
	bpp: 24,
	buffer_size: 2592/4 * 1944/4,
	fourcc: FOURCC( 'Y', '8', '0', '0' ),
	flags: 0, 
	buffer_types: UNICAP_BUFFER_TYPE_SYSTEM | UNICAP_BUFFER_TYPE_USER, 
	system_buffer_count: EUVCCAM_SYSTEM_BUFFER_COUNT,
	buffer_type: 0
      },
      N_ELEMENTS( format_8207_framerates ), 
      format_8207_framerates,
      format_8207_framerate_map,
      2592/4 * 1944/4, 
      NULL,
      EUVCCAM_FORMAT_IS_PARTIAL_SCAN,
   },
};

struct euvccam_devspec euvccam_devspec[] =
{
   { 
      0x8201,
      0, 
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8201 ),
      formats_8201, 
      N_ELEMENTS( properties_8201 ),
      properties_8201,
   },
   {
      0x8202,
      1,
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8202_mono ),
      formats_8202_mono,
      N_ELEMENTS( properties_8202_mono ),
      properties_8202_mono,
   },
   {
      0x8202,
      2,
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8202_color ),
      formats_8202_color,
      N_ELEMENTS( properties_8202_color ),
      properties_8202_color,
   },
   {
      0x8203,
      6,
      0,
      N_ELEMENTS( formats_8203_color ),
      formats_8203_color,
      N_ELEMENTS( properties_8202_color ),
      properties_8202_color,
   },
   {
      0x8204,
      (2<<2)|1,
      0,
      N_ELEMENTS( formats_8204_mono ),
      formats_8204_mono,
      N_ELEMENTS( properties_8202_mono ),
      properties_8202_mono,
   },
   { 
      0x8205,
      (3<<2)|1, 
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8205 ),
      formats_8205, 
      N_ELEMENTS( properties_8201 ),
      properties_8201,
   },
   { 
      0x8206,
      (4<<2), 
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8206 ),
      formats_8206, 
      N_ELEMENTS( properties_8206 ),
      properties_8206,
   },
   { 
      0x8207,
      (5<<2)|2, 
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8207_color ),
      formats_8207_color, 
      N_ELEMENTS( properties_8206 ),
      properties_8206,
   },
   { 
      0x8207,
      (5<<2)|1, 
      EUVCCAM_HAS_AUTO_EXPOSURE | EUVCCAM_HAS_AUTO_GAIN,
      N_ELEMENTS( formats_8207_mono ),
      formats_8207_mono, 
      N_ELEMENTS( properties_8206 ),
      properties_8206,
   },
   {
      0,
   }
};

   