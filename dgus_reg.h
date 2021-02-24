#pragma once
/**
 * @file dgus_reg.h
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Driver. Register and cointrol definitions
 *
 */


#define member_size(type, member) sizeof(((type *)0)->member)  /**< Get the member size of a struct */
#define SWP16(pt) (pt>>8) | (pt<<8)                            /**< Swap XY bytes to be YX */
#define SWP32(i) ((i&0xff000000)>>24)| ((i&0xff0000)>>8) | ((i&0xff00)<<8) | ((i&0xff)<<24) /**< Swap all bytes in a u32 to be le order */


/* Base addresses and hard coded vars */
#define	PIC_SET_PAGE_BASE    ((unsigned long)0x5A010000)  /**< Base address of the page change */
#define	DGUS_RESET           ((unsigned long)0x55AA5AA5)  /**< Magic reset command for the T5 */
#define	DGUS_RESET_HARD      ((unsigned long)0x55AA5A5A) /**< Magic reset command for the T5 and all onboard periphs */

/* Command addresses. A REG is 8bit, VAR cmd typically 16 */
/**
 * @brief Addresses of the command and variables
 */
enum command {
  DGUS_CMD_REG_W = 0x80,
  DGUS_CMD_REG_R,
  DGUS_CMD_VAR_W,
  DGUS_CMD_VAR_R,
  DGUS_CMD_CURVE_W
};

#define DGUS_RETURN uint8_t /**< Defines the return status of a function */

/**
 * @brief DGUS_RETURN Response to an async request such as dgus_request_var()
 */
#define DGUS_OK                       0   /**< Everything was fine */
#define DGUS_TIMEOUT                  1   /**< Timed out waiting for OK */
#define DGUS_ERROR                    2   /**< Unspecified serious error */
#define DGUS_CURVE_BUFFER_FULL        10  /**< Cannot append any more data to this buffer */
#define DGUS_CURVE_CHANNEL_NOT_FOUND  11  /**< The Curve channel 0-7 was not initialised or found */


/**
 * @brief Layout of the VAR register
 * 
 */
enum dgus_cmd_reg { // word sized registers
  DeviceId = 0x00,
  SystemReset = 0x04,
  OsUpdateCmd = 0x06,
  NorFlashRWCmd = 0x08,
  Reserved0C = 0x0C,
  Ver = 0x0F,
  Rtc = 0x10,
  PicPage = 0x14,
  GuiStatus = 0x15,
  TpStatus = 0x16,
  Reserved1A = 0x1A,
  Vcc = 0x30,
  Led = 0x31,
  Adc01 = 0x32,
  Reserved34 = 0x34,
  FolderName = 0x7C, // Max 8 chars like "DWIN_SET"
  SystemConfig = 0x80,
  LedConfig = 0x82,
  PicSetPage = 0x84,
  Pwm0Set = 0x86,
  Reserved88 = 0x88,
  Pwm0Out = 0x92,
  Reserved93 = 0x93,
  RTCSet = 0x9C,
  MusicPlaySet = 0xA0,
  BmpDownloadNotSupported = 0xA2,
  JpegDownload = 0xA6,
  NANDFlashRWCmd = 0xAA,
  TouchControl = 0xB0,
  SimTouchControl = 0xD4,
  PointerOverlay = 0xD8,
  ReservedDC = 0xDC,
  CrcMemoryCheck = 0xE0,
  ReservedE2 = 0xE2,
  MusicStreaming = 0xF0,
  PaintingInterface = 0xF4,
  DcsBusData = 0x100,
  Undefined200 = 0x200,
  Curve = 0x300,
  Reserved380 = 0x380,
};

/**
 * @brief The position of the control in x,y from the top left of the display
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) dgus_control_position {
  uint16_t x;
  uint16_t y;
} dgus_control_position;        /**< xy position of control */

/**
 * @brief position and size of the control
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) dgus_control_size {
  uint16_t x_top;
  uint16_t y_top;
  uint16_t x_bottom;
  uint16_t y_bottom;
} dgus_control_size;            /**< xy position and size of control */

/* Control specific structures */


typedef struct __attribute__((packed)) var_icon {
  uint16_t vp;
  uint16_t x;
  uint16_t y;
  uint16_t v_min;
  uint16_t v_max;
  uint16_t icon_min;
  uint16_t icon_max;
  uint8_t icon_lib;
  uint8_t mode;
  uint8_t layer_mode;
  uint8_t icon_gamma;
  uint8_t pic_gamma;
  uint8_t filter_set;
} var_icon; /**< Icon Data */

typedef struct __attribute__((packed)) animation_icon {
  uint16_t vp;
  uint16_t x;
  uint16_t y;
  uint16_t reset_icon;
  uint16_t v_stop;
  uint16_t v_start;
  uint16_t icon_stop;
  uint16_t icon_start;
  uint16_t icon_end;
  uint8_t icon_lib;
  uint8_t mode;
  uint8_t layer_mode;
  uint8_t icon_gamma;
  uint8_t pic_gamma;
  uint8_t time;
  uint8_t display_mode;
  uint8_t filter_set;
} animation_icon; /**< Animation Icon Data */

typedef struct __attribute__((packed)) slider_display {
  uint16_t vp;
  uint16_t v_begin;
  uint16_t v_end;
  uint16_t x_begin;
  uint16_t x_end;
  uint16_t icon_id;
  uint16_t y;
  uint8_t x_adj;
  uint8_t mode;
  uint8_t icon_lib;
  uint8_t icon_mode;
  uint8_t vp_data_mode;
  uint8_t layer_mode;
  uint8_t icon_gamma;
  uint8_t pic_gamma;
  uint8_t filter_set;
} slider_display; /**< Slider Data */

typedef struct __attribute__((packed)) artistic_var {
  uint16_t vp;
  uint16_t x;
  uint16_t y;
  uint16_t icon_0;
  uint8_t icon_lib;
  uint8_t icon_mode;
  uint8_t integer_digits;
  uint8_t decimal_digits;
  uint8_t vp_mode;
  uint8_t alignment;
  uint8_t layer_mode;
  uint8_t icon_gamma;
  uint8_t pic_gamma;
  uint8_t filter_set;
} artistic_var; /**< Artistic Data */

typedef struct __attribute__((packed)) image_animation {
  uint16_t zero;
  uint16_t pic_begin;
  uint16_t pic_end;
  uint8_t frame_time;
  uint8_t icl_Lib_id;
  uint8_t pic_end_exp;
  uint8_t reserved[16];
} image_animation; /**< Image Animation Data */


typedef struct __attribute__((packed)) icon_rotation {
  uint16_t vp;
  uint16_t icon_id;
  uint16_t icon_xc;
  uint16_t icon_yc;
  uint16_t xc;
  uint16_t yc;
  uint16_t v_begin;
  uint16_t v_end;
  uint16_t al_begin;
  uint16_t al_end;
  uint8_t vp_mode;
  uint8_t lib_id;
  uint8_t mode;
} icon_rotation; /**< Icon Rotation Data */

typedef struct __attribute__((packed)) bit_var_icon {
  uint16_t vp;
  uint16_t vp_aux;
  uint8_t display_mode;
  uint8_t move_mode;
  uint8_t icon_mode;
  uint8_t icon_lib;
  uint16_t icon_0_s;
  uint16_t icon_0_e;
  uint16_t icon_1_s;
  uint16_t icon_1_e;
  uint16_t x;
  uint16_t y;
  uint16_t dismove;
  uint16_t reserved;
} bit_var_icon; /**< Bit Var Icon Data */

typedef struct __attribute__((packed)) data_var {
  uint16_t vp;
  uint16_t x;
  uint16_t y;
  uint16_t colour;
  uint8_t lib_id;
  uint8_t font_width;
  uint8_t alignment;
  uint8_t integer_digits;
  uint8_t decimal_digits;
  uint8_t vp_mode;
  uint8_t len_unit;
  uint8_t string_unit[];  
} data_var; /**< Data Var Data */


typedef struct __attribute__((packed)) rtc_display_digital {
  uint16_t zero;
  uint16_t x;
  uint16_t y;
  uint16_t colour;
  uint8_t lib_id;
  uint8_t font_width;
  uint8_t string_code[]; // max 15
} rtc_display_digital; /**< RTC Data */

typedef struct __attribute__((packed)) rtc_display_analogue {
  uint16_t one; // literally 0x0001
  uint16_t x;
  uint16_t y;
  uint16_t icon_hour;
  uint32_t icon_hour_central;
  uint16_t icon_minute;
  uint32_t icon_minute_central;
  uint16_t icon_second;
  uint32_t icon_second_central;
  uint8_t lib_id;
  uint8_t reserved;
} rtc_display_analogue; /**< RTC Display Data */

typedef struct __attribute__((packed)) hex_data {
  uint16_t vp;
  uint16_t x;
  uint16_t y;
  uint8_t mode;
  uint8_t lib_id;
  uint8_t font_x;
  uint8_t string_code[]; // max 15
} hex_data; /**< Hex Data */

typedef struct __attribute__((packed)) roll_text {
  uint16_t vp;
  uint8_t rolling_mode;
  uint8_t rolling_distance_px;
  uint8_t adjust_mode;
  uint8_t reserved_00;
  uint16_t colour;
  uint16_t x_start;
  uint16_t y_start;
  uint16_t x_end;
  uint16_t y_end;
  uint8_t font_0_id;
  uint8_t font_1_id;
  uint8_t font_x_dots;
  uint8_t font_y_dots;
  uint8_t encode_mode;
  uint8_t text_distance_px;
  uint32_t reserved;
} roll_text; /**< Rolling Text Data */

typedef struct __attribute__((packed)) data_window {
  uint16_t vp;
  uint8_t v_min;
  uint8_t v_max;
  uint8_t integer_digits;
  uint8_t decimal_digits;
  uint8_t data_num;
  uint8_t mode;
  uint16_t x;
  uint16_t y;
  uint16_t adjust_step;
  uint8_t font_x_dots;
  uint8_t font_y_dots;
  uint16_t colour;
  uint8_t font_x_dots1;
  uint8_t font_y_dots1;
  uint16_t colour1;
  uint16_t reserved;
} data_window; /**< Window Data */

typedef struct __attribute__((packed)) basic_graphic {
  uint16_t vp;
  uint8_t area[8];
  uint8_t dashed_line_enable;
  uint8_t dash_set[4];
  uint8_t pixel_scale[13];
} basic_graphic; /**< Graphic Data */

typedef struct __attribute__((packed)) zone_rolling {
  uint16_t x_start;
  uint16_t y_start;
  uint16_t x_end;
  uint16_t y_end;
  uint16_t distance_move;
  uint16_t mode_move;
} zone_rolling; /**< Zone Data */

typedef struct __attribute__((packed)) qr_code {
  uint16_t vp;
  uint16_t x;
  uint16_t y;
  uint16_t unit_pixels;
  uint8_t fix_mode;
  uint8_t reserved_0;
} qr_code; /**< QR Data */

typedef struct __attribute__((packed)) area_brightness {
  uint16_t vp;
  uint16_t x_start;
  uint16_t y_start;
  uint16_t x_end;
  uint16_t y_end;
} area_brightness; /**< Area Brightness Data */
