#pragma once
/**
 * @file dgus_util.h
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Driver Utility functions
 * @license 
 */
#include <stddef.h>
#include <stdint.h>
#include "dgus_reg.h"
#include "dgus.h"

/**
 * @brief Layout of the music command register
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) dgus_cmd_music {
  uint8_t start_id;                     /**< Start playback of the Wav Id */
  uint8_t section_id;                   /**< Play back on the segment id */
  uint8_t volume;                       /**< Volume level to play 0-0x80 */
  uint8_t play_mode;                    /**< 0 stop, 1 pause, 2 play */
} dgus_cmd_music;                       /**< Music cmd */

/**
 * @brief Brightnedd LED configuration
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) dgus_cmd_led_config {
  uint8_t backlight_brightness_running; /**< Brightness while not in idle (0x0- 0x64) */
  uint8_t backlight_brightness;         /**< Brightness while in idle (0x0- 0x64) */
  uint16_t dim_wait_ms;                 /**< Time before we idle /5ms */
} dgus_cmd_led_config;                  /**< Led config */

/**
 * @brief System configuration information
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) dgus_cmd_system_config {  //0x80
  uint8_t read_write_mode;               /**< 0x0 to read this register, 0x5A to write. Write data at your peril */
  uint8_t touch_panel_sensitivity_ro;    /**< RO Current touchscreen sensitivility value */
  uint8_t touch_panel_mode_config_ro;    /**< RO Touch Mode. Regular DGUS, raw or paint */
  uint8_t crc_enabled:1;                 /**< Serial comms CRC Enabled */
  uint8_t music_enabled:1;               /**< Buzzer on (1)/off(0) */
  uint8_t load_22_bin_ro:1;              /**< RO Load the 22_.bin configuration file */
  uint8_t var_auto_upload:1;             /**< Global control over auto upload. Controls that auto-upload will be disabled) */
  uint8_t touch_audio_enabled:1;         /**< Global audio control over button press audio */
  uint8_t backight_standby:1;            /**< Enable or disbale backlight standby mode */
  uint8_t screen_orientation:2;          /**< Current screen orientation. 00=0, 01 = 90, 10=180, 11=270 */
} dgus_cmd_system_config;                /**< sys config*/


/**
 * @brief Get the currently displayed page ID number
 * 
 * @param page a pointer the page id variable
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_get_page(uint16_t *page);

/**
 * @brief Change DGUS page
 * 
 * @param page page id
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_set_page(uint8_t page);

/**
 * @brief Change an icon to index val
 * 
 * @param icon_addr VP of the icon
 * @param val index to set to
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_set_icon(uint16_t icon_addr, uint8_t val);

/**
 * @brief Get the current brightness for the display
 * 
 * @param brightness value from 0 - 100%
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_get_brightness(uint8_t *brightness);

/**
 * @brief Get the current brightness for the display
 * 
 * @param brightness value from 0 - 100%
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_set_brightness(uint8_t brightness);

/**
 * @brief Get the current volume
 * 
 * @param volume pointer. 0-255 volume
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_get_volume(uint8_t *volume);

/**
 * @brief Set the current volume level
 * 
 * @param volume 0-255
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_set_volume(uint8_t volume);

/**
 * @brief Reset the DGUS display
 * 
 * @param full_reset 0 or 1. 1 will perform a full reset including T5 chip, NAND and all peripherals
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_system_reset(uint8_t full_reset);

/**
 * @brief Read the system configuration variable
 * 
 * @param config 
 * @return #DGUS_RETURN 
 */
DGUS_RETURN dgus_get_system_config(dgus_cmd_system_config *config);
