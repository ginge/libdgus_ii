/**
 * @file dgus_lcd.c
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

static uint8_t _ack_mode = ACK_MODE;

static int _handle_packet(char *data, uint8_t cmd, uint8_t len);


static void _delay(int ms) 
{  
    // Storing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not achieved 
    while (clock() < start_time + ((CLOCKS_PER_SEC / 1000) * ms))
        ;
}


/* Callback for a packet received */
static packet_handler_cb _recv_handler;                 /**< packet handler cb once deparsed */
static ser_recv_handler_cb _ser_recv_handler;           /**< function to call to receive a single packet */
static ser_send_handler_cb _ser_send_handler;           /**< function to call for sending data */
static ser_available_handler_cb _ser_avail_handler;     /**< function to get number of available bytes */

/* Internal recv buffers */
static uint8_t recvlen;
static uint8_t recvcmd;
static uint8_t recvdata[RECV_BUFFER_SIZE];              /**< recv buffer */

/**
 * @brief  A packet header that every packet needs 
 * len incudes data + 1 (for cmd byte)
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) dgus_packet_header_t {
  uint8_t header0;
  uint8_t header1;
  uint8_t len;
  uint8_t cmd;
} dgus_packet_header; /**< packet header structure */

struct __attribute__((packed)) dgus_packet {
  dgus_packet_header header;
  union Data {
    uint8_t cdata[SEND_BUFFER_SIZE];
    uint16_t sdata[SEND_BUFFER_SIZE/2];
    uint32_t ldata[SEND_BUFFER_SIZE/4];
  } data;
  uint8_t len;
};

typedef struct __attribute__((packed)) dgus_var_data_t {
  uint16_t address;
  uint16_t data[16];
} dgus_var_data; /**< Header for a VAR command */



void dgus_init(ser_available_handler_cb avail, ser_recv_handler_cb recv, ser_send_handler_cb send, packet_handler_cb packet_handler) {
  _recv_handler = packet_handler;
  _ser_recv_handler = recv;
  _ser_send_handler = send;
  _ser_avail_handler = avail;
  /* Intializes random number generator */
  time_t t;
  srand((unsigned) time(&t));
}

static void _prepare_header(dgus_packet_header *header, uint16_t cmd, uint16_t len) {
  header->header0 = HEADER0;
  header->header1 = HEADER1;
  header->len =len + 1;
  header->cmd = cmd;
}

/**
 * @brief polling wait for data. wait until we timeout
 * 
 * @return Response such as #DGUS_TIMEOUT
 */
static DGUS_RETURN _polling_wait_for_ok() {
  // there is no expected ack, so return like we got one
  if (_ack_mode == ACK_MODE_OK_DISABLED)
    return DGUS_OK;

  int timer = SEND_TIMEOUT;
  int r = 0;
  while(r <= 0) {
    r = dgus_recv_data();
    if (r == PACKET_OK) {
      // we got an OK. What do we want to do with it?
      return DGUS_OK;
    }
    // timeout
    _delay(1);
    if (timer == 0) {
      DEBUG_PRINTF("TIMEOUT ON OK!\n");
      return DGUS_TIMEOUT;
    }
    timer--;
  }
  return DGUS_OK;
}

DGUS_RETURN _polling_wait() {
  int timer = SEND_TIMEOUT;
  while(dgus_recv_data() <= 0) {
    // timeout
    _delay(1);
    if (timer == 0) {
      DEBUG_PRINTF("TIMEOUT!\n");
      return DGUS_TIMEOUT;
    }
    timer--;
  }
  return DGUS_OK;
}


DGUS_RETURN send_data(enum command cmd, dgus_packet *p) {
  _prepare_header(&p->header, cmd, p->len);
  for (int i = 0; i < sizeof(p->header); i++) {
    DEBUG_PRINTF("0x%x ", *((uint8_t *)&p->header + i));
  }
  DEBUG_PRINTF(" | ");

  for (int i = 0; i < p->len; i++) {
    DEBUG_PRINTF("0x%x ", *((uint8_t *)p + i + offsetof(dgus_packet, data)));
  }
  DEBUG_PRINTF("\n");
  if (_ser_send_handler)
    _ser_send_handler((uint8_t *)p, sizeof(p->header) + p->len);

  if (cmd != DGUS_CMD_VAR_R)
    if (_polling_wait_for_ok() == DGUS_TIMEOUT);
      return DGUS_TIMEOUT;

  return DGUS_OK;
}

int dgus_recv_data() {
  static int recv_cnt = 0;
  static uint8_t _recv_state = 0;

  if (!_ser_avail_handler || ! _ser_recv_handler)
    return -1;
  
  //_delay(10);
  while (_ser_avail_handler())
  {
      uint8_t d = _ser_recv_handler();

      if (_recv_state == 0) {
        memset(recvdata, 0, sizeof(recvdata));
          recvlen = 0;
          recvcmd = 0;
        // match first header byte
        if (d != HEADER0) {
          recv_cnt = 0;
          return -1;
        }

        _recv_state = 1;
      }
      else if (_recv_state == 1) {
        // match second header byte or 0 for an OK message
        if (d != HEADER1 && d != 0) {
          recv_cnt = 0;
          _recv_state = 0;
          return -1;
        }

        _recv_state = 2;
      }
      // Len. We got the header. next up if length
      else if (_recv_state == 2) {
        recvlen = d;
        _recv_state = 3;
      }
      // command byte
      else if (_recv_state == 3) {
        recvcmd = d;
        _recv_state = 4;
      }
      // data payload next
      else if (_recv_state == 4) {
        recvdata[recv_cnt] = d;
        recv_cnt++;

        if (recv_cnt >= recvlen - 1) {
          // done
          int res = _handle_packet(recvdata, recvcmd, recvlen - 1);
          
          _recv_state = 0;
          recv_cnt = 0;
          return res;
        }
      }
  }

  return 0;
}

/* tail n 8 bit variable to the output buffer */
void buffer_u8(dgus_packet *p, uint8_t *data, size_t len) {
  memcpy(&p->data.cdata[p->len], data, len);
  p->len += len;
}

/* tail n 16 bit variables to the output buffer */
void buffer_u16(dgus_packet *p, uint16_t *data, size_t len) {
  for(int i = 0; i < len; i++) {
    uint16_t pt = data[i];
    uint16_t pn = SWP16(pt);
    memcpy(&p->data.cdata[p->len], &pn, 2);
    p->len +=2;
  }
}

/* tail n 32 bit variables to the output buffer */
void buffer_u32(dgus_packet *p, uint32_t *data, size_t len) {
  for(int i = 0; i < len; i++) {
    uint32_t t = SWP32(data[i]);
    memcpy(&p->data.cdata[p->len], &t, 4);
    p->len += 4;
  }
}

/* tail a 32 bit variable to the output buffer */
void buffer_u32_1(dgus_packet *p, uint32_t data) {
  if (data < 0xFFFF) {
    buffer_u16(p, (uint16_t *)&data, 1);
  }
  else {
    uint32_t t = SWP32(data);
    memcpy(&p->data.cdata[p->len], &t, 4);
    p->len += 4;
  }
}

/* re-init the packet buffer */
dgus_packet *dgus_packet_init() {
  static dgus_packet d;
  memset(&d, 0, sizeof(d));
  d.len = 0;
  return &d;
}

void dgus_packet_set_data(dgus_packet *p, uint8_t offset, uint8_t *data, uint8_t len) {
    memcpy(&p->data.cdata[offset], data, len);
}

DGUS_RETURN dgus_request_var(uint16_t addr, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u8(d, &len, 1);
  send_data(DGUS_CMD_VAR_R, d);

  // async wait for reply
  return DGUS_OK;
}

/* Sync read n 16 bit variables from the VAR register at addr */
DGUS_RETURN dgus_get_var8(uint16_t addr, uint8_t *buf, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u8(d, &len, 1);
  send_data(DGUS_CMD_VAR_R, d);

  DGUS_RETURN r =_polling_wait();
  if (r != DGUS_OK) return r;

  // got a packet
  memcpy(buf, recvdata, len);
  for (int i = 0; i < len; i+=2) {
    uint16_t *bp = (uint16_t *)(&buf[i]);
    *bp = SWP16(*bp);
  }
  
  return DGUS_OK;
}

DGUS_RETURN dgus_get_var(uint16_t addr, uint16_t *buf, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u8(d, &len, 1);
  send_data(DGUS_CMD_VAR_R, d);

  DGUS_RETURN r =_polling_wait();
  if (r != DGUS_OK) return r;

  // got a packet
  memcpy(buf, (uint16_t *)recvdata, len);
  
  return DGUS_OK;
}


/* Set a variable in the VAR buffer
 * data width is 16 bit
 */
DGUS_RETURN dgus_set_var(uint16_t addr, uint32_t data) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u32_1(d, (data));
  return send_data(DGUS_CMD_VAR_W, d);
}

DGUS_RETURN dgus_set_var8(uint16_t addr, uint8_t *data, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u8(d, data, len);
  return send_data(DGUS_CMD_VAR_W, d);
}

DGUS_RETURN dgus_set_cmd(uint16_t addr, uint8_t *data, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u8(d, data, len);
  return send_data(DGUS_CMD_REG_R, d);
}

DGUS_RETURN dgus_get_cmd(uint16_t addr, uint8_t *data, uint8_t len) {
  dgus_packet *d = dgus_packet_init();
  buffer_u16(d, &addr, 1);
  buffer_u8(d, data, len);
  
  send_data(DGUS_CMD_REG_W, d);

  DGUS_RETURN r =_polling_wait();
  if (r != DGUS_OK) return r;

  // got a packet
  for(unsigned long i = 0; i < len * 2; i += 2) {
    data[i]     = recvdata[1 + i];
    data[i + 1] = recvdata[0 + i];
  }
  return DGUS_OK;
}

/* internal */
void dgus_packet_set_len(dgus_packet *p, uint16_t len) {
  p->len = len;
}

uint8_t *dgus_packet_get_recv_buffer() {
  return recvdata;
}

DGUS_RETURN _polling_read_16(uint8_t *buf, uint8_t len) {
  DGUS_RETURN r =_polling_wait();
  if (r != DGUS_OK) return r;

  // got a packet
  for(unsigned long i = 0; i < len * 2; i += 2) {
    uint8_t *recvdata = dgus_packet_get_recv_buffer();
    buf[i]     = recvdata[1 + i];
    buf[i + 1] = recvdata[0 + i];
  }

  return r;
}

/* Handle an incoming packet */
static int _handle_packet(char *data, uint8_t cmd, uint8_t len) {
  uint16_t addr;
  uint8_t bytelen = 0;

  if(len == 0x02 && (cmd == DGUS_CMD_VAR_W || cmd == DGUS_CMD_REG_W) 
      && (data[0] == 'O') && (data[1] == 'K')) {    //response for writing byte 
    DEBUG_PRINTF("OK\n");
    return PACKET_OK;
  }
  else if(cmd == DGUS_CMD_VAR_R) {
    addr = SWP16(*(uint16_t *)data);
    bytelen = data[2];

    for(unsigned long i = 0; i < bytelen * 2; i += 2) {
      data[i] = data[4 + i];
      data[i + 1] = data[3 + i];
    }
  }
  else if(cmd == DGUS_CMD_REG_R) {
    // response for reading the page from the register
    addr = data[0];
    bytelen = data[1];

    for(uint8_t i = 0; i < bytelen; i++) {
        data[i] = data[2 + i];
    }
  }

  DEBUG_PRINTF("CMD 0x%02x : PLEN %d : LEN %d : ADDR 0x%02x : DATA: ", cmd, len, bytelen, addr);
  for (uint8_t i = 0; i < bytelen; i+=2) {
    DEBUG_PRINTF("0x%04x ", (uint16_t)*(uint16_t *)(data + i));
  }
  DEBUG_PRINTF("\n");

  if (_recv_handler) 
    _recv_handler(data, cmd, len, addr, bytelen);

  return bytelen;
}

