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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "hardware/gpio.h"

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

const uint topSend[]={0,1,2,3,4,5,6,7,8};
const uint bottomRecieve[]={9,10,11,12,13,14,15,18,19,20};

uint8_t topBottomKey[9][20] = {};
uint32_t currentModifier;

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);


void gpio_init_tasks(){
  for (int i = 0; i < 9;i++){
      gpio_init(topSend[i]);
      gpio_set_dir(topSend[i],GPIO_OUT);
      gpio_pull_down(topSend[i]);
      };
 for (int i = 0; i < 10;i++){
      gpio_init(bottomRecieve[i]);
      gpio_set_dir(bottomRecieve[i],GPIO_IN);
      gpio_pull_down(bottomRecieve[i]);
      };


};

void defineKeys(){
  // [topRow][bottomRow] = the key to return
  topBottomKey[0][9] = HID_KEY_F2;
  topBottomKey[0][10] = HID_KEY_2;
  topBottomKey[0][11] = HID_KEY_W;
  topBottomKey[0][12] = HID_KEY_S;
  topBottomKey[0][13] = HID_KEY_X;
  topBottomKey[0][14] = HID_KEY_F3;
  topBottomKey[0][15] = HID_KEY_3;
  topBottomKey[0][18] = HID_KEY_C;
  topBottomKey[0][19] = HID_KEY_D;
  topBottomKey[0][20] = HID_KEY_E;
  
  topBottomKey[1][9] = HID_KEY_ESCAPE;
  topBottomKey[1][10] = HID_KEY_GRAVE;
  topBottomKey[1][11] = HID_KEY_TAB;
  topBottomKey[1][12] = HID_KEY_CONTROL_LEFT;
  topBottomKey[1][13] = HID_KEY_SHIFT_LEFT;
  topBottomKey[1][14] = HID_KEY_F1;
  topBottomKey[1][15] = HID_KEY_1;
  topBottomKey[1][18] = HID_KEY_Z;
  topBottomKey[1][19] = HID_KEY_A;
  topBottomKey[1][20] = HID_KEY_Q;

  topBottomKey[2][9] = HID_KEY_ARROW_UP;
  topBottomKey[2][10] = HID_KEY_ARROW_RIGHT;
  topBottomKey[2][11] = HID_KEY_ARROW_DOWN;
  topBottomKey[2][12] = HID_KEY_ARROW_LEFT;
  topBottomKey[2][13] = HID_KEY_ALT_LEFT;
  topBottomKey[2][14] = HID_KEY_GUI_LEFT;
  topBottomKey[2][15] = HID_KEY_SPACE;
  topBottomKey[2][18] = HID_KEY_GUI_LEFT;
  topBottomKey[2][19] = HID_KEY_ALT_LEFT;
  topBottomKey[2][20] = HID_KEY_SPACE;

  topBottomKey[3][9] = HID_KEY_PRINT_SCREEN;
  topBottomKey[3][10] = HID_KEY_SCROLL_LOCK;
  topBottomKey[3][11] = HID_KEY_PAUSE;
  topBottomKey[3][13] = HID_KEY_INSERT;
  topBottomKey[3][14] = HID_KEY_HOME;
  topBottomKey[3][15] = HID_KEY_PAGE_UP;
  topBottomKey[3][18] = HID_KEY_DELETE;
  topBottomKey[3][19] = HID_KEY_END;
  topBottomKey[3][20] = HID_KEY_PAGE_DOWN;
  
  topBottomKey[4][9] = HID_KEY_F12;
  topBottomKey[4][10] = HID_KEY_0;
  topBottomKey[4][11] = HID_KEY_P;
  topBottomKey[4][12] = HID_KEY_SEMICOLON;
  topBottomKey[4][13] = HID_KEY_SLASH;
  topBottomKey[4][14] = HID_KEY_BACKSLASH;
  topBottomKey[4][15] = HID_KEY_SHIFT_LEFT;
  topBottomKey[4][18] = HID_KEY_CONTROL_LEFT;
  topBottomKey[4][19] = HID_KEY_ENTER;
  topBottomKey[4][20] = HID_KEY_O;

  topBottomKey[5][9] = HID_KEY_F10;
  topBottomKey[5][10] = HID_KEY_8;
  topBottomKey[5][11] = HID_KEY_I;
  topBottomKey[5][12] = HID_KEY_K;
  topBottomKey[5][13] = HID_KEY_COMMA;
  topBottomKey[5][14] = HID_KEY_F11;
  topBottomKey[5][15] = HID_KEY_9;
  topBottomKey[5][18] = HID_KEY_PERIOD;
  topBottomKey[5][19] = HID_KEY_L;
  topBottomKey[5][20] = HID_KEY_O;
  
  topBottomKey[6][9] = HID_KEY_F8;
  topBottomKey[6][10] = HID_KEY_6;
  topBottomKey[6][11] = HID_KEY_Y;
  topBottomKey[6][12] = HID_KEY_H;
  topBottomKey[6][13] = HID_KEY_N;
  topBottomKey[6][14] = HID_KEY_F9;
  topBottomKey[6][15] = HID_KEY_7;
  topBottomKey[6][18] = HID_KEY_U;
  topBottomKey[6][19] = HID_KEY_J;
  topBottomKey[6][20] = HID_KEY_M;

  topBottomKey[7][9] = HID_KEY_F6;
  topBottomKey[7][10] = HID_KEY_MINUS;
  topBottomKey[7][11] = HID_KEY_BRACKET_LEFT;
  topBottomKey[7][12] = HID_KEY_BACKSLASH;
  topBottomKey[7][13] = HID_KEY_BACKSPACE;
  topBottomKey[7][14] = HID_KEY_F7;
  topBottomKey[7][15] = HID_KEY_EQUAL;
  topBottomKey[7][18] = HID_KEY_BACKSPACE;
  topBottomKey[7][19] = HID_KEY_APOSTROPHE;
  topBottomKey[7][20] = HID_KEY_BRACKET_RIGHT;

  topBottomKey[8][9] = HID_KEY_F4;
  topBottomKey[8][10] = HID_KEY_4;
  topBottomKey[8][11] = HID_KEY_R;
  topBottomKey[8][12] = HID_KEY_F;
  topBottomKey[8][13] = HID_KEY_V;
  topBottomKey[8][14] = HID_KEY_F5;
  topBottomKey[8][15] = HID_KEY_5;
  topBottomKey[8][18] = HID_KEY_B;
  topBottomKey[8][19] = HID_KEY_G;
  topBottomKey[8][20] = HID_KEY_T;


};

/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();
  gpio_init_tasks();
  defineKeys();
   
  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();
    
    hid_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}


bool isModiferKey(uint32_t btn){
  if ((btn == HID_KEY_CONTROL_LEFT)||(btn == HID_KEY_SHIFT_LEFT)||(btn==HID_KEY_ALT_LEFT)){
    return 1;
  }

  return 0;
}
//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t btn)
{

  // skip if hid is not ready yet
  if ( !tud_hid_ready() || isModiferKey(btn) ) return;


    
      // use to avoid send multiple consecutive zero report for keyboard
      static bool has_keyboard_key = false;

      if ( btn )
      {
        uint8_t keycode[6] = { 0 };
        keycode[0] = btn;
        keycode[1] = btn;
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, currentModifier, keycode);
        has_keyboard_key = true;
      }else
      {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
      }
    
   
}

void checkModifers(){
  


  currentModifier = 0;
}

bool scanKeyboard(){
  checkModifers();
  bool istyping = 0;
//don't forget to account for sleep mode.
  for (int i = 0; i < 9;i++){
    gpio_put(topSend[i],1);
    for (int j = 0; j < 10;j++){
      while(gpio_get(bottomRecieve[j])){
        
        //WAKE UP BRO.
        if(tud_suspended()){
          tud_remote_wakeup();
        };

        //lookup which top gpio we're outputting, which bottom gpio we're reading, and then look up which
        //key that corresponds to. Then send that as an HID report
        send_hid_report(REPORT_ID_KEYBOARD,topBottomKey[topSend[i]][bottomRecieve[j]]);
        istyping = 1; 
        };
      };
    //return topSend GPIO pin to low once we're done with it
    gpio_put(topSend[i],0);
  };
  return istyping;
}
void keyboard_task(){
  int n = 0;
  while (n < 1000){
  scanKeyboard();
  n = n+1;
  
  tud_task(); // tinyusb device task
  }
};


// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  
  keyboard_task();
 
}


// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id, board_button_read());
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}


