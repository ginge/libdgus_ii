#pragma once
/**
 * @file dgus_control_curve.h
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Driver Utility functions
 */

/* Curve */
#include <stddef.h>
#include <stdint.h>
#include "dgus_reg.h"
#include "dgus.h"

/**
 * @brief Opaque reference to a curve
 */
typedef struct curve curve;

#define CURVE_ADDRESS 0x0310   /**< VAR address to write each datapoint to */
#define CURVE_HEADER  0x5AA5   /**< CMD header to enable write mode */

/**
 * @brief SP Structure for realtime curve control
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) realtime_curve {
  uint8_t mode;
  uint8_t reserved_0;
  uint16_t x;
  uint16_t y;
  uint16_t y_central;
  uint16_t vd_central;
  uint16_t colour;
  uint16_t mul_y;
  uint8_t channel;
  uint8_t distrance_horizontal;
  uint8_t pixel_scale;
  uint8_t reserved[5];
} realtime_curve; /**< Curve Data */

/**
 * @brief Initialise a new curve buffer, allocating the memory required.
 * @warning See datasheet regarding memory ranges. If you enable a channel, it will consume VP memory.
 * You cannot re-use this memory for other controls. For example, 
 * channel 0:  0x1000-0x17FF
 * channel 1:  0x1800-0x1FFF
 * ..
 * channel 7:  0x4800-0x4FFF
 * 
 * @note if a channel is not in use, you may re-use the memory. If you use the channel at all, anywhere, you cannot use this memory. i.e. using a different page and reusing curve memory will not work
 * 
 * @param num_curves How many channels are enabled on the DGUS
 * @param datapoint_buffer_len How much data should be send at once
 * @return curve* Opaque reference to the curve
 */
curve *dgus_curve_buffer_create(uint8_t num_curves, uint8_t datapoint_buffer_len);

/**
 * @brief Initialise a channel 
 * 
 * @param cur curve handle
 * @param channel_id channel id we want to send data to
 */
void dgus_curve_init_channel(curve *cur, uint8_t channel_id);

/**
 * @brief Send the data we have aggregated in the curve instance
 * 
 * @param cur the curve handle
 * @return DGUS_RETURN 
 */
DGUS_RETURN dgus_curve_send_data(curve *cur);

/**
 * @brief Append some data to the curve buffer for batch sending
 * 
 * @param cur curve handle
 * @param chan_id channel id
 * @param data data to append
 * @return DGUS_RETURN buffer full etc
 */
DGUS_RETURN dgus_curve_add_data(curve *cur, uint8_t chan_id, uint16_t data);

/**
 * @brief Reset a curve on screen. This will not affect the buffer
 * 
 * @param cur curve handle
 * @param chan_id channel id
 * @return DGUS_RETURN 
 */
DGUS_RETURN dgus_curve_reset(curve *cur, uint8_t chan_id);
