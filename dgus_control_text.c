/**
 * @file dgus_control_text.c
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Text Utilites
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h> 
#include "dgus.h"
#include "dgus_control_text.h"


union eight_adapt {
  uint8_t u8[2];
  uint16_t u16;
};

/* Read the text from an address
 * Reads in 8 bit data format when using 0x02 GBK
 */
DGUS_RETURN dgus_get_text(uint16_t addr, uint8_t *buf, uint8_t len) {
  dgus_packet *d = dgus_packet_init(addr);
  buffer_u16(d, &addr, 1);
  buffer_u8(d, &len, 1);
  send_data(DGUS_CMD_VAR_R, d);
  return _polling_read_16(buf, len);
}

/* Set a variable in the VAR buffer
 * data width is 8 bit when 0x02 GBK format used
 */
DGUS_RETURN dgus_set_text(uint16_t addr, char *text) {
  return dgus_set_text_padded(addr, text, 0);
}

/* Set text and pad the remaining space with empty string */
DGUS_RETURN dgus_set_text_padded(uint16_t addr, char *text, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  if (len == 0)
    len = strlen(text);
  buffer_u8(d, text, len);
  // override the packet len to send the padding
  dgus_packet_set_len(d, len);

  return send_data(DGUS_CMD_VAR_W, d);
}

/* Only work when using SP enabled. addr is SP address */

DGUS_RETURN dgus_get_text_vp(uint16_t addr, uint16_t *vp) {
  return dgus_get_var(addr + offsetof(dgus_control_text_display, vp), vp, 1);
}

DGUS_RETURN dgus_set_text_vp(uint16_t addr, uint16_t vp) {
  return dgus_set_var(addr + offsetof(dgus_control_text_display, vp), vp);
}

DGUS_RETURN dgus_get_text_pos(uint16_t addr, dgus_control_position *pos) {
  uint16_t d[sizeof(dgus_control_position)/2];
  memcpy(d, pos, sizeof(dgus_control_position));
  return dgus_get_var(addr + offsetof(dgus_control_text_display, pos), d, 2);
}

DGUS_RETURN dgus_set_text_pos(uint16_t addr, dgus_control_position pos) {
  uint16_t npos[sizeof(dgus_control_position)];
  memcpy(npos, &npos, sizeof(dgus_control_position));
  uint16_t newaddr = addr + offsetof(dgus_control_text_display, pos);
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &newaddr, 1);
  buffer_u16(d, npos, member_size(dgus_control_text_display, pos) / 2);
  return send_data(DGUS_CMD_VAR_W, d);
}

DGUS_RETURN dgus_get_text_colour(uint16_t addr, uint16_t *colour) {
  return dgus_get_var(addr + offsetof(dgus_control_text_display, colour), colour, 1);
}

DGUS_RETURN dgus_set_text_colour(uint16_t addr, uint16_t colour) {
  return dgus_set_var(addr + offsetof(dgus_control_text_display, colour), colour);
}

DGUS_RETURN dgus_get_text_bounding_size(uint16_t addr, dgus_control_size *size) {
  uint16_t sz[sizeof(dgus_control_size)/2];
  memcpy(sz, size, sizeof(dgus_control_size)/2);
  return dgus_get_var(addr + offsetof(dgus_control_text_display, size), sz, member_size(dgus_control_text_display, size) / 2 );
}

DGUS_RETURN dgus_set_text_bounding_size(uint16_t addr, dgus_control_size size) {
  uint16_t sz[sizeof(dgus_control_size)/2];
  memcpy(sz, &size, sizeof(dgus_control_size));
  uint16_t newaddr = addr + offsetof(dgus_control_text_display, size);
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &newaddr, 1);
  buffer_u16(d, sz, member_size(dgus_control_text_display, size) / 2);
  return send_data(DGUS_CMD_VAR_W, d);
}

DGUS_RETURN dgus_get_text_len(uint16_t addr, uint16_t *len) {
  return dgus_get_var(addr + offsetof(dgus_control_text_display, text_len), len, 1);
}

DGUS_RETURN dgus_set_text_len(uint16_t addr, uint16_t len) {
  return dgus_set_var(addr + offsetof(dgus_control_text_display, text_len), len);
}

DGUS_RETURN dgus_get_text_fonts(uint16_t addr, uint8_t *font0, uint8_t *font1) {
  uint8_t fonts[2];
  DGUS_RETURN r = dgus_get_var(addr + offsetof(dgus_control_text_display, font_0_id), (uint16_t *)&fonts, 1);
  if (r != DGUS_OK) return r;

  *font0 = fonts[0];
  *font1 = fonts[1];
  return r;
}

DGUS_RETURN dgus_set_text_fonts(uint16_t addr, uint8_t font0, uint8_t font1) {
  union eight_adapt f; f.u8[0] = font0; f.u8[1] = font1;
  return dgus_set_var(addr + offsetof(dgus_control_text_display, font_0_id), f.u16);
}

DGUS_RETURN dgus_get_text_font_dots(uint16_t addr, uint8_t *fontx_dots, uint8_t *fonty_dots) {
  uint8_t fonts[2];
  DGUS_RETURN r = dgus_get_var(addr + offsetof(dgus_control_text_display, font_x_dots), (uint16_t *)&fonts, 1);
  if (r != DGUS_OK) return r;

  *fontx_dots = fonts[0];
  *fonty_dots = fonts[1];
  return r;
}

DGUS_RETURN dgus_set_text_font_dots(uint16_t addr, uint8_t fontx_dots, uint8_t fonty_dots) {
  union eight_adapt f; f.u8[0] = fontx_dots; f.u8[1] = fonty_dots;
  return dgus_set_var(addr + offsetof(dgus_control_text_display, font_x_dots), f.u16);
}

DGUS_RETURN dgus_get_text_encode_mode_distance(uint16_t addr, uint8_t *encode_mode, uint8_t *vert_distance, uint8_t *horiz_distance) {
  uint8_t fonts[2];
  DGUS_RETURN r = dgus_get_var(addr + offsetof(dgus_control_text_display, encode_mode), (uint16_t *)&fonts, 1);
  if (r != DGUS_OK) return r;

  *encode_mode = fonts[0];
  *vert_distance = fonts[1];
  return r;
}


DGUS_RETURN dgus_set_text_encode_mode_distance(uint16_t addr, uint8_t encode_mode, uint8_t vert_distance, uint8_t horiz_distance) {
  union eight_adapt f;
  f.u8[0] = encode_mode;
  f.u8[1] = vert_distance;
  return dgus_set_var(addr + offsetof(dgus_control_text_display, encode_mode), f.u16);  
}
