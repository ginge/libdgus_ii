/**
 * @file dgus_control_curve.c
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
#include "dgus_control_curve.h"

typedef struct curve_data_t {
  uint8_t channel_id;
  uint8_t capacity_words;
  uint8_t used_words;
  uint16_t *data;
} curve_data;

struct curve {
  uint8_t channel_count;
  uint8_t _initted_count;
  curve_data curves[];
};

/* This is the terse format for updating the curves. Seemingly not working on my devices */
typedef struct __attribute__((packed)) dgus_curve_data_t {
  uint8_t channels; // bit field of channels to update
  uint16_t data[16];
} dgus_curve_data;

curve *dgus_curve_buffer_create(uint8_t num_curves, uint8_t datapoint_buffer_len) {
  size_t sz = sizeof(curve) + 
              (sizeof(curve_data) * num_curves);
  curve *c = calloc(1, sz);
  printf("SZ %ld\n", sz);
  if (!c)
    return NULL;

  c->channel_count = num_curves;
  for (int i = 0; i < num_curves; i++) {
    c->curves[i].capacity_words = datapoint_buffer_len;
  }

  return c;
}

void dgus_curve_init_channel(curve *cur, uint8_t channel_id) {
  cur->curves[cur->_initted_count].channel_id = channel_id;
  cur->curves[cur->_initted_count].data = calloc(1, sizeof(uint16_t) * cur->curves[cur->_initted_count].capacity_words);
  cur->_initted_count++;
  return;
}

void dgus_curve_destroy(curve *cur) {
  for (int i = 0; i < cur->channel_count; i++) {
      free(cur->curves[i].data);
  }
  free(cur);
}

DGUS_RETURN dgus_curve_add_data(curve *cur, uint8_t chan_id, uint16_t data) {
  for(int i = 0; i < cur->_initted_count; i++) {
    if (cur->curves[i].channel_id == chan_id) {
      if (cur->curves[i].used_words >= cur->curves[i].capacity_words)
        return DGUS_CURVE_BUFFER_FULL;
      cur->curves[i].data[cur->curves[i].used_words] = data;
      cur->curves[i].used_words++;
      return DGUS_OK;
    }
  }

  return DGUS_CURVE_CHANNEL_NOT_FOUND;
}

DGUS_RETURN dgus_curve_send_data(curve *cur) {
  // build a packet with all of the data for the curve processor

  dgus_packet *d = dgus_packet_init();
  // write the Address
  uint16_t temp16 = 0;
  uint8_t temp8 = 0;

  temp16 = CURVE_ADDRESS;
  buffer_u16(d, &temp16, 1);
  // write the header (5aa5)
  temp16 = CURVE_HEADER;
  buffer_u16(d, &temp16, 1);

  // write the number of channels this payload contains in total. 
  // i.e send to chan 1 and 3 it is a total of "2" channels
  buffer_u8(d, &cur->_initted_count, 1); // num channels
  // always 0
  temp8 = 0;
  buffer_u8(d, &temp8, 1);

  // write each channel and the data
  // [chanid][words][data word]...
  for (int i = 0; i < cur->_initted_count; i++) {
    
    curve_data *cd = &cur->curves[i];
    if (cd->used_words == 0) {
      printf("C \n");
      temp8 = cur->_initted_count - 1;
      // decrement the amount of channels we are sending
      dgus_packet_set_data(d, 4, &temp8, 1);
      //d->data.cdata[4] = cur->_initted_count - 1; 
      continue;
    }

    buffer_u8(d, &cd->channel_id, 1);
    // word count
    buffer_u8(d, &cd->used_words, 1);
    // data
    buffer_u16(d, cd->data, cd->used_words);
  }
  
  send_data(DGUS_CMD_VAR_W, d);

  for (int i = 0; i < cur->_initted_count; i++) {
    cur->curves[i].used_words = 0;
  }
}

DGUS_RETURN dgus_curve_reset(curve *cur, uint8_t chan_id) {
  // actually not clear what to do here. 
}