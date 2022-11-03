/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/*
    PSX Controller - USB converter 

    based on TinyUSB HID device sample
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/spi.h"
#include "psx_controller.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
void hid_task(void);


static void io_init(void)
{
  // SPI0 at 0.25MHz.
  spi_init(SPI_PORT, 250 * 1000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_ACK, GPIO_FUNC_XIP);
  gpio_set_function(PIN_MODE, GPIO_FUNC_XIP);

  // Chip select is active-low
  gpio_init(PIN_CS);
  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_put(PIN_CS, 1);

  // Ack port
  gpio_init(PIN_ACK);
  gpio_set_dir(PIN_ACK, GPIO_IN);
  gpio_set_pulls(PIN_ACK, true, false);

  // Joy mode 
  gpio_set_dir(PIN_MODE, GPIO_IN);
  gpio_set_pulls(PIN_MODE, true, false);

  spi_set_format(SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

  // LED indicator
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
}

/*------------- MAIN -------------*/
int main()
{
  board_init();
  io_init();

  tusb_init();

  while (1)
  {
    tud_task(); // tinyusb device task
    hid_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void)itf;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  (void)itf;
  (void)report_id;
  (void)report_type;

  // echo back anything we received from host
  tud_hid_report(0, buffer, bufsize);
}

//--------------------------------------------------------------------+
// HID TASK
//--------------------------------------------------------------------+
// PSX report: L D R U  St R3 L3 Se   [] X O ^   R1 L1 R2 L2
//             --------------------   -----------------------
//             report[1] Button1      report[2]  Button2

// XE1AJ-USB : ST SEL E2 E1 D C B A                  


//  HID report XE1AJ     PSX     
//  bit0       A       -   []  L1
//  bit1       B       -   ^   R1  X
//  bit2       C       -   O
//  bit3       D       -   R2
//  bit4       E1      -   N/A
//  bit5       E2      -   N/A
//  bit6       SEL     -   SEL
//  bit7       ST      -   ST

#define IS_BUTTON(button_val, button_const)   (((button_val) & (button_const)) / (button_const))

static void make_button_report_digital(uint8_t *button1, uint8_t *button2)
{

  int joy_mode = gpio_get(PIN_MODE);

  if(joy_mode == false){
    // MegaDrive mini2 special arrange
    *button1 = (IS_BUTTON(*button2, PSX_BUTTON2_RECT)    << 0)  // A
            | (IS_BUTTON(*button2, PSX_BUTTON2_L1)       << 0)  // A
            | (IS_BUTTON(*button2, PSX_BUTTON2_TRIANGLE) << 1)  // B
            | (IS_BUTTON(*button2, PSX_BUTTON2_R1)       << 1)  // B
            | (IS_BUTTON(*button2, PSX_BUTTON2_CROSS)    << 1)  // B
            | (IS_BUTTON(*button2, PSX_BUTTON2_CIRCLE)   << 2)  // C
            | (IS_BUTTON(*button2, PSX_BUTTON2_R2)       << 3)  // D
            | (IS_BUTTON(*button1, PSX_BUTTON1_SELECT)   << 6)
            | (IS_BUTTON(*button1, PSX_BUTTON1_START)    << 7);
  }else{
    // My 6-button joystick arrange
    *button1 = (IS_BUTTON(*button2, PSX_BUTTON2_CROSS)   << 0)
            | (IS_BUTTON(*button2, PSX_BUTTON2_CIRCLE)   << 1)
            | (IS_BUTTON(*button2, PSX_BUTTON2_R1)       << 2)
            | (IS_BUTTON(*button2, PSX_BUTTON2_RECT)     << 3)
            | (IS_BUTTON(*button2, PSX_BUTTON2_TRIANGLE) << 4)
            | (IS_BUTTON(*button2, PSX_BUTTON2_L1)       << 5)
            | (IS_BUTTON(*button1, PSX_BUTTON1_SELECT)   << 6)
            | (IS_BUTTON(*button1, PSX_BUTTON1_START)    << 7);
  }

  *button2 = 0;
}

static void make_button_report_analog(uint8_t *button1, uint8_t *button2)
{
  // MegaDrive mini2 special arrange
  *button1 = (IS_BUTTON(*button2, PSX_BUTTON2_RECT)     << 0)  // A
           | (IS_BUTTON(*button2, PSX_BUTTON2_L1)       << 0)  // A
           | (IS_BUTTON(*button2, PSX_BUTTON2_TRIANGLE) << 1)  // B
           | (IS_BUTTON(*button2, PSX_BUTTON2_R1)       << 1)  // B
           | (IS_BUTTON(*button2, PSX_BUTTON2_CROSS)    << 1)  // B
           | (IS_BUTTON(*button2, PSX_BUTTON2_CIRCLE)   << 2)  // C
           | (IS_BUTTON(*button2, PSX_BUTTON2_R2)       << 3)  // D
           | (IS_BUTTON(*button1, PSX_BUTTON1_SELECT)   << 6)
           | (IS_BUTTON(*button1, PSX_BUTTON1_START)    << 7);

  *button2 = 0;
}

void hid_task(void)
{
  // Poll every 1ms
  const uint32_t interval_ms = 1;
  static uint32_t start_ms = 0;
  static uint8_t second_report = 0;
  static uint8_t report_len = 0;

  if (board_millis() - start_ms < interval_ms)
    return; // not enough time
  start_ms += interval_ms;

  if (!tud_hid_ready())
    return;

  // Read PSX pad data
  uint8_t pad_id;
  uint8_t psx_recv[22];
  uint8_t joyreport[7];

  report_len = 0;
  memset(joyreport, 0, sizeof(joyreport));

  bool result = get_psx_pad_data(psx_recv, &pad_id);

  // Build HID report according to the controller type
  uint8_t dir;
  uint8_t button1, button2;

  // extract stick/button info
  dir = (psx_recv[1] & 0xf0) >> 4;  // L D R U 
  button1 = psx_recv[1];
  button2 = psx_recv[2];

  switch (pad_id)
  {
  case PSX_CTRLID_DIGITAL:
    // build HID report for digital mode
    make_button_report_digital(&button1, &button2);
    joyreport[0] = button1;
    joyreport[1] = 0;
    joyreport[2] = 0;
    joyreport[3] = ((dir & 0x2) ? 0xff : 0x00) | ((dir & 0xa) ? 0x00 : 0x80);   // Stick     X
    joyreport[4] = ((dir & 0x4) ? 0xff : 0x00) | ((dir & 0x5) ? 0x00 : 0x80);   // Stick     Y
    joyreport[5] = 0x80;   // Throttle  X  .. not used
    joyreport[6] = 0x80;   // Throttle  Y  .. not used

    report_len = 7;
    break;

  case PSX_CTRLID_DUAL_ANALOG:
    // build HID report for analog mode
    make_button_report_analog(&button1, &button2);

    // psx 3: X2
    // psx 4: Y2
    // psx 5: X1
    // psx 6: Y1

    joyreport[0] = button1;
    joyreport[1] = 0;
    joyreport[2] = 0;
    joyreport[3] = psx_recv[5];   // Stick     X  - PSX Left X
    joyreport[4] = psx_recv[6];   // Stick     Y  - PSX Left Y
    joyreport[5] = psx_recv[3];   // Throttle  X  - PSX Right X
    joyreport[6] = psx_recv[4];   // Throttle  Y  - PSX Right Y

    report_len = 7;
    break;

  default:
    break;
  }

  // Send joystick Report (ID:1) if any
  if(report_len > 0){
    gpio_put(25, 1);   // USB detected & PSX pad comm success
    tud_hid_report(0, joyreport, report_len);
  }
}
