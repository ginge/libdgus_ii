/* Configuration */
/* timeout in ms */
#define SEND_TIMEOUT        200
#define ACK_MODE            ACK_MODE_OK_WAIT
#define RECV_BUFFER_SIZE    32
#define SEND_BUFFER_SIZE    32
#define DEBUG_PRINT_ENABLED 1

//#define DEBUG_PRINTF(...) {}
#define DEBUG_PRINTF(...) { printf(__VA_ARGS__); }

/* Standard packet header signature */
#define HEADER0 0x5A
#define HEADER1 0xA5
