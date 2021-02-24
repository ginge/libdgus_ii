/**
 * @file dgus_util.c
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Driver Utility functions
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h> 
#include "dgus.h"
#include "dgus_util.h"

/* Change the page on the DGUS */
DGUS_RETURN dgus_set_page(uint8_t page) {
  return dgus_set_var(PicSetPage, PIC_SET_PAGE_BASE + page);
}

DGUS_RETURN dgus_get_page(uint16_t *page) {
  uint16_t pg = 0;
  DGUS_RETURN r = dgus_get_var(PicPage, &pg, 1);
  *page = pg;
  return r;
}

/* Set the icon ID for the icon at address */
DGUS_RETURN dgus_set_icon(uint16_t icon_addr, uint8_t val) {
  return dgus_set_var(icon_addr, val);
}

DGUS_RETURN dgus_play_sound(uint8_t sound_id, uint8_t section_id, uint8_t volume, uint8_t play_mode) {
  dgus_cmd_music m = {
    .start_id = sound_id,
    .section_id = section_id,
    .volume = volume,
    .play_mode = play_mode
  };
  return dgus_set_var8(MusicPlaySet, (uint8_t *)&m, sizeof(m));
}

DGUS_RETURN dgus_set_volume(uint8_t volume) {
  uint16_t vol = (uint16_t)volume << 8;
  return dgus_set_var8(MusicPlaySet + 1, (uint8_t *)&vol, 2);
}

DGUS_RETURN dgus_get_volume(uint8_t *volume) {
  dgus_cmd_music m;
  DGUS_RETURN r = dgus_get_var8(MusicPlaySet, (uint8_t *)&m, sizeof(m));
  if (r != DGUS_OK)
    return r;

  *volume = m.volume;
  return DGUS_OK;
}

DGUS_RETURN dgus_set_rtc(uint16_t icon_addr, uint8_t val) {
  return 0;
}
DGUS_RETURN dgus_get_rtc(uint16_t icon_addr, uint8_t val) {
  return 0;
}

DGUS_RETURN dgus_set_brightness(uint8_t brightness) {
  uint16_t delay = 1000;
  dgus_cmd_led_config l = {
    .dim_wait_ms = SWP16(delay),
    .backlight_brightness = brightness,
    .backlight_brightness_running = brightness
  };
  return dgus_set_var8(LedConfig, (uint8_t *)&l, sizeof(l));
}

DGUS_RETURN dgus_get_brightness(uint8_t *brightness) {
  dgus_cmd_led_config l;
  DGUS_RETURN r = dgus_get_var8(LedConfig, (uint8_t *)&l, sizeof(l));
  if (r != DGUS_OK)
    return r;


  *brightness = l.backlight_brightness;
  return DGUS_OK;
}

DGUS_RETURN dgus_get_system_config(dgus_cmd_system_config *config) {
  DGUS_RETURN r = dgus_get_var8(SystemConfig, (uint8_t *)config, sizeof(dgus_cmd_system_config));
  
  if (r != DGUS_OK)
    return r;

  return DGUS_OK;
}

DGUS_RETURN dgus_system_reset(uint8_t full_reset) {
  uint32_t r = SWP32(DGUS_RESET);

  if (full_reset)
    r = SWP32(DGUS_RESET_HARD);

  return dgus_set_var8(SystemReset, (uint8_t *)&r, sizeof(r));
}
