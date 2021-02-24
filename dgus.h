#pragma once
/**
 * @file dgus.h
 * @author Barry Carter
 * @date 01 Jan 2021
 * @brief DGUS II LCD Driver
 *
 */
#include "dgus_reg.h"
#include "dgus_util.h"


/**
 * @brief A "OK" ACK packet was received from dgus_packet_recv()
 */
#define PACKET_OK -1

/**
 * @brief Set when the firmware does NOT have ACK (OK packets) enabled
 */
#define ACK_MODE_OK_DISABLED 0
/**
 * @brief Wait for OK packets on send
 */
#define ACK_MODE_OK_WAIT     1


/* Callback for a packet received */
/**
 * @brief Packet Parsing handler. When a valid packet is recieved, this callback is invoked
 */
typedef void (*packet_handler_cb)(char *data, uint8_t cmd, uint8_t len, uint16_t addr, uint8_t bytelen);

/**
 * @brief recv a single char from the usart
 */
typedef char (*ser_recv_handler_cb)(void);

/**
 * @brief Send Handler. On platforms such as Arduino, this would likely be SERIALx.write(p, sz)
 */
typedef void (*ser_send_handler_cb)(char *data, size_t len);

/**
 * @brief Emount of bytes of serial data is available to be read. For devices with abacking buffer such as arduino, this maybe > 1. Most other devices will return byte by byte
 */
typedef uint8_t (*ser_available_handler_cb)(void);


/**
 * @brief Opaque reference to a packet 
 */
typedef struct dgus_packet dgus_packet;

/**
 * @brief Initialise the DGUS LCD interface
 * 
 * Callbacks allow platform independance.
 * On Arduino like platforms, avail would be "SerialPort.available()"
 * 
 * @param avail ser_available_handler_cb callback for the amount of bytes available 
 * @param recv function that receives bytes from the serial port
 * @param send function that sends bytes over the serial port
 * @param packet_handler function that should be called when the lcd has sent a packet to be processed
 */
void dgus_init(ser_available_handler_cb avail, ser_recv_handler_cb recv, ser_send_handler_cb send, packet_handler_cb packet_handler);

/**
 * @brief Receive and process data from the serial port.
 * Call this in your main loop
 * 
 * @return int returns < 1 error, 0 for no data
 */
int dgus_recv_data();

/**
 * @brief Append 1 byte len bytes to the send buffer in 8 bit format
 * 
 * @param p #dgus_packet packet
 * @param data data to write
 * @param len length of data bytes to write
 */
void buffer_u8(dgus_packet *p, uint8_t *data, size_t len);

/**
 * @brief Append 2 bytes len bytes to the send buffer in 16 bit format
 * 
 * @param p #dgus_packet packet
 * @param data data to write
 * @param len length of data bytes to write
 */
void buffer_u16(dgus_packet *p, uint16_t *data, size_t len);

/**
 * @brief Append 4 bytes len bytes to the send buffer in 2x word format
 * 
 * @param p #dgus_packet packet
 * @param data data to write
 * @param len length of data bytes to write
 */
void buffer_u32(dgus_packet *p, uint32_t *data, size_t len);

/**
 * @brief Append 4 bytes 1 long of data to the send buffer in 8 bit format
 * 
 * @param p #dgus_packet packet
 * @param data data to write
 */
void buffer_u32_1(dgus_packet *p, uint32_t data);

/**
 * @brief Write 8 bites of data at position n in the buffer
 * 
 * @param p #dgus_packet packet
 * @param offset where in the buffer to write
 * @param data data to write
 * @param len length of data bytes to write
 */
void dgus_packet_set_data(dgus_packet *p, uint8_t offset, uint8_t *data, uint8_t len);

/**
 * @brief actually send the packet p with write cmd
 * 
 * @param cmd command type such as DGUS_CMD_VAR_W
 * @param p packet handle
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN send_data(enum command cmd, dgus_packet *p);

/* Var commands */
/* Request the variable and immediately return.
 * your own handler implementation should deal directly with the data
 */

/**
 * @brief Async request a variable It is up to you to read the response
 * 
 * @param addr VAR Addres to read from 
 * @param len number of bytes to read
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_request_var(uint16_t addr, uint8_t len);

/**
 * @brief Read @p len bytes into @p buf at address @p addr
 * 
 * @param addr 
 * @param buf 
 * @param len 
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_get_var(uint16_t addr, uint16_t *buf, uint8_t len);

/**
 * @brief Set @p data variable into @p addr on the DGUS
 * 
 * @param addr address of the VAR data in word (u16) format
 * @param data data to send
 * @return Response such as #DGUS_TIMEOUT
 */
DGUS_RETURN dgus_set_var(uint16_t addr, uint32_t data);

/**
 * @brief Get @p len 8 bit bytes from the device. no encoding is applied.
 * 
 * @param addr 
 * @param buf 
 * @param len 
 * @return DGUS_RETURN 
 */
DGUS_RETURN dgus_get_var8(uint16_t addr, uint8_t *buf, uint8_t len);

/**
 * @brief Set @p len 8 bit bytes to the device. no encoding is applied.
 * 
 * @param addr 
 * @param data 
 * @param len 
 * @return DGUS_RETURN 
 */
DGUS_RETURN dgus_set_var8(uint16_t addr, uint8_t *data, uint8_t len);

/**
 * @brief Manually initialise the packet sending buffer
 * 
 * @return dgus_packet* Opaque pointer to the packet buffer
 */
dgus_packet *dgus_packet_init();



/* internal utility */
/**
 * @brief Override a packets current length.
 * 
 * @param p 
 * @param len 
 */
void dgus_packet_set_len(dgus_packet *p, uint16_t len);

/**
 * @brief Get a pointer the current data recv buffer
 * 
 * @return uint8_t* 
 */
uint8_t *dgus_packet_get_recv_buffer();

/**
 * @brief Wait for the OK to return or data to arrive
 * 
 * @return uint8_t 
 */
uint8_t _polling_wait();

/**
 * @brief Poll and read a 16 bit return value
 * 
 * @param buf 
 * @param len 
 * @return DGUS_RETURN 
 */
DGUS_RETURN _polling_read_16(uint8_t *buf, uint8_t len);

#include "dgus_config.h"
