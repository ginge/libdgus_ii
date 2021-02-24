#pragma once
#include <stddef.h>
#include <stdint.h>
#include "dgus_reg.h"
#include "dgus.h"


typedef struct __attribute__((packed)) dgus_control_text_display {
  uint16_t vp;
  dgus_control_position pos;
  uint16_t colour;
  dgus_control_size size;
  uint16_t text_len;
  uint8_t font_0_id;
  uint8_t font_1_id;
  uint8_t font_x_dots;
  uint8_t font_y_dots;
  uint8_t encode_mode;
  uint8_t hor_distance;
  uint8_t ver_distance;
  uint8_t reserved0;
} dgus_control_text_display;

/**
 * @brief Read from VAR address as text
 * Reads in 8 bit data format when using 0x02 GBK
 * 
 * @param addr 
 * @param buf 
 * @param len 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text(uint16_t addr, uint8_t *buf, uint8_t len);

/**
 * @brief Write text to the address VAR
 * @warning The DGUS does not do string null termination when rendering. You will need to clear the value beforehand, or use the dgus_set_text()
 * 
 * @param addr address to write to 
 * @param text text to send. Must be null terinated
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text(uint16_t addr, char *text);

/**
 * @brief Write text to the address VAR and clear all text after to field length @param len
 * 
 * @param addr address to write to 
 * @param text text to send. Must be null terinated
 * @param len Field length on the DGUS. This is the VAR size you allocated
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_vp(uint16_t addr, uint16_t *vp);

DGUS_RETURN dgus_set_text_vp(uint16_t addr, uint16_t vp);

DGUS_RETURN dgus_set_text_padded(uint16_t addr, char *text, uint8_t len);

DGUS_RETURN dgus_get_text_pos(uint16_t addr, dgus_control_position *pos);

DGUS_RETURN dgus_set_text_pos(uint16_t addr, dgus_control_position pos);

DGUS_RETURN dgus_get_text_colour(uint16_t addr, uint16_t *colour);

DGUS_RETURN dgus_set_text_colour(uint16_t addr, uint16_t colour);

DGUS_RETURN dgus_get_text_bounding_size(uint16_t addr, dgus_control_size *size);

DGUS_RETURN dgus_set_text_bounding_size(uint16_t addr, dgus_control_size size);

DGUS_RETURN dgus_get_text_len(uint16_t addr, uint16_t *len);

DGUS_RETURN dgus_set_text_len(uint16_t addr, uint16_t len);

DGUS_RETURN dgus_get_text_fonts(uint16_t addr, uint8_t *font0, uint8_t *font1);

DGUS_RETURN dgus_set_text_fonts(uint16_t addr, uint8_t font0, uint8_t font1);

DGUS_RETURN dgus_get_text_font_dots(uint16_t addr, uint8_t *fontx_dots, uint8_t *fonty_dots);

DGUS_RETURN dgus_set_text_font_dots(uint16_t addr, uint8_t fontx_dots, uint8_t fonty_dots);

DGUS_RETURN dgus_get_text_encode_mode_distance(uint16_t addr, uint8_t *encode_mode, uint8_t *vert_distance, uint8_t *horiz_distance);

DGUS_RETURN dgus_set_text_encode_mode_distance(uint16_t addr, uint8_t encode_mode, uint8_t vert_distance, uint8_t horiz_distance);
