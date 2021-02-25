#pragma once
/**
 * @file dgus_control_text.h
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Text Utilites
 */
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
} dgus_control_text_display; /**< Register layout for the text SP register */

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
 * @brief Write text to the address VAR and clear all text after to field length @p len
 * 
 * @param addr 
 * @param text
 * @param len
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_padded(uint16_t addr, char *text, uint8_t len);

/**
 * @brief Set the VP pointer address in memory
 * @note addr must be an SP address and SP must be enabled
 * e.g. uint16_t vp; dgus_get_text_vp(0x4000, &vp);
 * 
 * @param addr address to write to 
 * @param vp variable to place result the memory VP address
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_vp(uint16_t addr, uint16_t *vp);

/**
 * @brief Set the virtual pointer VP address for the control
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param vp 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_vp(uint16_t addr, uint16_t vp);

/**
 * @brief Get the position of the text on the screen
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param pos
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_pos(uint16_t addr, dgus_control_position *pos);

/**
 * @brief Set the position of the text on the screen
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param pos
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_pos(uint16_t addr, dgus_control_position pos);

/**
 * @brief Get the colour of the text
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param colour 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_colour(uint16_t addr, uint16_t *colour);

/**
 * @brief Set the colur of the text
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param colour 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_colour(uint16_t addr, uint16_t colour);

/**
 * @brief Set the size of the box containing the text
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param size 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_bounding_size(uint16_t addr, dgus_control_size *size);

/**
 * @brief Get the size of the box containing the text
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param size 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_bounding_size(uint16_t addr, dgus_control_size size);

/**
 * @brief Get the text length
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param len 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_len(uint16_t addr, uint16_t *len);

/**
 * @brief Set the text length
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param len 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_len(uint16_t addr, uint16_t len);

/**
 * @brief Get the current font id
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param font0 
 * @param font1 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_fonts(uint16_t addr, uint8_t *font0, uint8_t *font1);

/**
 * @brief set the font ids
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param font0 
 * @param font1 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_fonts(uint16_t addr, uint8_t font0, uint8_t font1);

/**
 * @brief Get the elipses breakpoint. i.e. how long before we ...
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param fontx_dots 
 * @param fonty_dots 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_font_dots(uint16_t addr, uint8_t *fontx_dots, uint8_t *fonty_dots);

/**
 * @brief Set the elipses breakpoint. i.e. how long before we ...
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param fontx_dots 
 * @param fonty_dots 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_font_dots(uint16_t addr, uint8_t fontx_dots, uint8_t fonty_dots);

/**
 * @brief Set the text encoding mode and vertical spacing
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param encode_mode  0x00=8bit, 0x01=GB2312, 0x02=GBK (most compatible with this lib), 0x03=BIG5, 0x04=SJIS, 0x05=UNICODE
 * @param vert_distance spacing y
 * @param horiz_distance spacing x
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_text_encode_mode_distance(uint16_t addr, uint8_t *encode_mode, uint8_t *vert_distance, uint8_t *horiz_distance);

/**
 * @brief get text encoding mode and vertical spacing
 * @note addr must be an SP address and SP must be enabled
 * 
 * @param addr 
 * @param encode_mode  0x00=8bit, 0x01=GB2312, 0x02=GBK (most compatible with this lib), 0x03=BIG5, 0x04=SJIS, 0x05=UNICODE
 * @param vert_distance spacing y
 * @param horiz_distance spacing x
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_text_encode_mode_distance(uint16_t addr, uint8_t encode_mode, uint8_t vert_distance, uint8_t horiz_distance);
