// they are: stdio.h stdlib.h and libserialport.h
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h> 
#include <unistd.h>
#include <libserialport.h>
#include "dgus.h"
#include "dgus_control_curve.h"
#include "dgus_control_text.h"

#define BAUD 115200



/* internal */


uint8_t _cur_pg = 0;
int filecount = 18; 
int _sub_idx;
int _sub_depth;
int files_in_folder = 7;
int folders_count = 8;

void _pg(uint8_t pg) {

  char files[32][32] = {
    {"Ultimaker"},
    {"Models"},
    {"Firmware1"},
    {"Firmware2"},
    {"Firmware3"},
    {"Firmware4"},
    {"Firmware5"},
    {"E6"},
    {"File 1 test. testing long1"},
    {"File 1 test. testing long2"},
    {"File 1 test. testing long3"},
    {"File 1 test. testing long4"},
    {"File 1 test. testing long5"},
    {"File 1 test. testing long6"},
    {"File 1 test. testing long7"},
    {"File 2 test. testing long1"},
    {"File 2 test. testing long2"},
    {"File 2 test. testing long3"},
    {"File 2 test. testing long4"},
    {"File 2 test. testing long5"},
    {"File 2 test. testing long6"},
    {"File 2 test. testing long7"},
    {"File 2 test. testing long1"},
    {"File 3 test. testing long2"},
    {"File 3 test. testing long3"},
    {"File 3 test. testing long4"},
  };


  char files1[10][32] = {
    {".."},
    {"pre level"},
    {"reset (1)"},
    {"reset (2)"},
    {"reset (3)"},
  };

  files_in_folder = 7;
  folders_count = 8;

  for (int i = 0; i < 7; i++) {
    dgus_set_icon(0x6300 + i + 1, 0);
  }

  int x = 0;


  char ebuf[32];
  memset(ebuf, 0x20, sizeof(ebuf));
  for (int i = 0; i < files_in_folder; i++) {
    //if (files[i + (pg * files_in_folder)][0] == '\0') {
      //dgus_set_text_padded(0x6200 + x, ebuf, 32);
   // }
    //else {
      char * f = files[i + (pg * files_in_folder)];
      if (_sub_idx == 1) {
        f = files1[i + (pg * files_in_folder)];
        folders_count = 0;
        _sub_depth = 1;
        //filecount = 0;
        //files_in_folder = 5;
      };
      memset(ebuf, 0x20, sizeof(ebuf));
      if (i + (pg * files_in_folder) < folders_count) {
        snprintf(ebuf, 30, "> %s\\", f);
      }
      else {
        snprintf(ebuf, 30, "%s", f);
      }
      dgus_set_text_padded(0x6200 + x, ebuf, 32);
    //}
    x += 32;
  }

  char buf[10];
  sprintf(buf, "%-2d/%-2d", pg + 1, (filecount / 7) + 1);
  dgus_set_text(0x62E0, buf);
}


// platform impl of serial
static struct sp_port *port;



void _a_recv_handler(char *data, uint8_t cmd, uint8_t len, uint16_t addr, uint8_t bytelen) {
  printf("JSNDJNASD\n");
  if (addr == 0x5002 && (uint16_t)*(uint16_t *)data == 1) {
    if (_cur_pg > 0)
      _pg(--_cur_pg);
  }
  else if (addr == 0x5002 && (uint16_t)*(uint16_t *)data == 0x2) {
    
    if ((_cur_pg + 1 * 7) < filecount)
      _pg(++_cur_pg);
  }
  else if (addr == 0x5004 && (uint16_t)*(uint16_t *)data >= 0x01 && (uint16_t)*(uint16_t *)data < 0x08) {
    uint16_t u = (uint16_t)*(uint16_t *)data;

    // ".." clicked
    if (_sub_depth && u == 1) {
      _sub_idx = 0;
      _sub_depth = 0;
      _pg(0);
      return;
    }

    if (u < folders_count) {
      _sub_idx = u;
      _pg(0);
      return;
    }
    for (int i = 0; i < 7; i++) {
      dgus_set_icon(0x610A + i + 1, i + 1 == u);
    }
  }
  else if (addr == 0x5000 && (uint16_t)*(uint16_t *)data == 0x01) {
    //dgus_set_page(2);
  }
  else if (addr == 0x5002 && (uint16_t)*(uint16_t *)data == 0x03) {
    dgus_set_page(4);
  }
  else if (addr == 0x5006 && (uint16_t)*(uint16_t *)data == 0x03) {
    dgus_set_page(7);
  }
  else if (addr == 0x5008 && (uint16_t)*(uint16_t *)data == 0x01) {
    dgus_set_page(7);
  }
  else if (addr == 0x5008 && (uint16_t)*(uint16_t *)data == 0x02) {
    dgus_set_page(9);
  }
  else if (addr == 0x5008 && (uint16_t)*(uint16_t *)data == 0x03) {
    dgus_set_page(8);
  }
}

uint8_t _serial_bytes_available() {
  return sp_input_waiting(port);
}

char _serial_recv_byte() {
  char c;
  sp_nonblocking_read(port, (void *)&c, 1);
  return c;
}

void _serial_send_data(char *data, size_t len) {
  sp_blocking_write(port, data, len, SEND_TIMEOUT);
}


int main(int argc, char *argv[])
{
    fprintf(stderr,"Start\n");


  int err;
  int i,cmd;
  int count=4096;
// We need a serial port name
  if (argc<2)
    {
    fprintf(stderr,"Usage port\n");
    exit(1);
    }
// Open serial port
  err=sp_get_port_by_name(argv[1],&port);
  if (err==SP_OK)
  err=sp_open(port,SP_MODE_READ_WRITE);
  if (err!=SP_OK)
    {
    fprintf(stderr,"Can't open port %s\n",argv[1]);
    exit(2);
    }
// set Baud rate
  sp_set_baudrate(port,BAUD);
 
// write reset
  cmd=0;

dgus_init(_serial_bytes_available, _serial_recv_byte, _serial_send_data, _a_recv_handler);

dgus_set_page(1);
dgus_set_text_padded(0x6000, "xyz 260.0 000.0 000.0", 32);
dgus_set_text_padded(0x6010, "192/220c 57/60c", 16);
dgus_set_var(0x500A, 999);
dgus_set_text_padded(0x6030, "300/62000", 16);
dgus_set_text_padded(0x6038, "00:24 / 02:16", 16);

/*uint16_t buf[20];
uint8_t *ubuf = (uint8_t *)buf;
memset(buf, 0, sizeof(buf));
//dgus_request_var(0x1036, 1);
dgus_get_var(0x5014, buf, 1);
printf("BUF 0x%02x (%d)\n", buf[0], buf[0]);
dgus_get_text(0x6100, ubuf, 10);
printf("BUF %s\n", (char *)(ubuf));
*/
// file testing
_pg(0);

static int16_t curve_val = 0;
static clock_t start_time = 0;

if (start_time == 0)
  start_time = clock();


curve *cur = dgus_curve_buffer_create(2, 5);
dgus_curve_init_channel(cur, 0);
dgus_curve_init_channel(cur, 1);
int x = 0;
while(x < 200) {
  dgus_recv_data();

  // looping till required time is not achieved 
  if (clock() > start_time + (CLOCKS_PER_SEC/1000) * 500) {
    dgus_curve_add_data(cur, 0, (uint16_t)rand() % 300);
    dgus_curve_add_data(cur, 1, (uint16_t)rand() % 300);
    dgus_curve_send_data(cur);
    printf("CLOCK %ld\n", start_time);
    curve_val = rand() % 1000;
    
    start_time = clock();
    x++;
  }
  
}
uint16_t page = 0;
dgus_get_page(&page);
printf("PAGE %d\n", page);
uint8_t br = 0;
dgus_get_brightness(&br);
printf("BRI %d\n", br);
dgus_set_brightness(50);
sleep(3);
dgus_cmd_system_config conf;
dgus_get_system_config(&conf);

printf("System Config: \n");
printf("  Touch Sense: 0x%02x \n", conf.touch_panel_sensitivity_ro);
printf("  Touch Mode : 0x%02x \n", conf.touch_panel_mode_config_ro);
printf("  CRC        : 0x%02x \n", conf.crc_enabled);
printf("  Audio En   : 0x%02x \n", conf.music_enabled);
printf("  22 bin     : 0x%02x \n", conf.load_22_bin_ro);
printf("  Var Upload : 0x%02x \n", conf.var_auto_upload);
printf("  AudioC     : 0x%02x \n", conf.touch_audio_enabled);
printf("  BL Standby : 0x%02x \n", conf.backight_standby);
printf("  Orientation: 0x%02x \n", conf.screen_orientation);


dgus_system_reset(0);
  putchar('\n');
  i = 0;
  sp_close(port);
  return 0;
}
