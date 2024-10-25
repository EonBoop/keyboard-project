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

#include <fenv.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "pico/stdio.h"
#include "tusb.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
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

//could make this a struct, but I'm too far gone
uint8_t topBottomKey[9][21] = {};
uint8_t currentModifier;


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
  topBottomKey[1][12] = 0x01;//KEYBOARD_MODIFIER_LEFTCTRL;//HID_KEY_CONTROL_LEFT;
  topBottomKey[1][13] = 0x02;//KEYBOARD_MODIFIER_LEFTSHIFT;//HID_KEY_SHIFT_LEFT;
  topBottomKey[1][14] = HID_KEY_F1;
  topBottomKey[1][15] = HID_KEY_1;
  topBottomKey[1][18] = HID_KEY_Z;
  topBottomKey[1][19] = HID_KEY_A;
  topBottomKey[1][20] = HID_KEY_Q;

  topBottomKey[2][9] = HID_KEY_ARROW_UP;
  topBottomKey[2][10] = HID_KEY_ARROW_RIGHT;
  topBottomKey[2][11] = HID_KEY_ARROW_DOWN;
  topBottomKey[2][12] = HID_KEY_ARROW_LEFT;
  topBottomKey[2][13] = 0x04;//KEYBOARD_MODIFIER_LEFTALT;//HID_KEY_ALT_LEFT;
  topBottomKey[2][14] = HID_KEY_GUI_LEFT;
  topBottomKey[2][15] = HID_KEY_SPACE;
  topBottomKey[2][18] = HID_KEY_GUI_LEFT;
  topBottomKey[2][19] = 0x40;//KEYBOARD_MODIFIER_LEFTALT;//ID_KEY_ALT_LEFT;
  topBottomKey[2][20] = HID_KEY_SPACE;

  topBottomKey[3][9] = HID_KEY_ARROW_UP;
  topBottomKey[3][10] = HID_KEY_PRINT_SCREEN;
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
  topBottomKey[4][15] = 0x20;//KEYBOARD_MODIFIER_LEFTSHIFT;//HID_KEY_SHIFT_LEFT;
  topBottomKey[4][18] = 0x10;//KEYBOARD_MODIFIER_LEFTCTRL;//HID_KEY_CONTROL_LEFT;
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
  topBottomKey[6][18] = HID_KEY_M;
  topBottomKey[6][19] = HID_KEY_J;
  topBottomKey[6][20] = HID_KEY_U;

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
  if ((btn == 0x01)||(btn == 0x02)||(btn==0x04)||(btn == 0x10)||(btn == 0x20)||(btn==0x40)){
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
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, currentModifier, keycode);
        //tud_hid_keyboard_report(REPORT_ID_KEYBOARD, currentModifier, keycode);
        has_keyboard_key = true;
      }else
      {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
      }
    
   
}
/* this breaks other things so commenting it out
void checkModifers(){
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;


  currentModifier = 0x00;
  //going to do this the manual way and hope it never changes
  
  //left control 0x01
  gpio_put(1,1);
  if (gpio_get(12)){
    currentModifier |= 0x02;
  };
  if (gpio_get(13)){
    currentModifier |= 0x02;
  };
  gpio_put(1,0);

  //left alt 0x04
  gpio_put(2,1);
  if (gpio_get(13)||gpio_get(19)){
    currentModifier |= 0x2;
  };
  gpio_put(2,0);

  gpio_put(4,1);
  if (gpio_get(15)) currentModifier |= 0x02;
  if (gpio_get(18)) currentModifier |= 0x02;
  gpio_put(4,0);

}

*/
bool allowButtonPress(uint8_t topButton, uint8_t bottomButton){
  const  uint32_t debounceInterval_MS = 5;
  static uint32_t topBottomKeyTime[9][21] = {0};
  
  if ((board_millis() - topBottomKeyTime[topButton][bottomButton]) < debounceInterval_MS) return 0;
  
  topBottomKeyTime[topButton][bottomButton] += debounceInterval_MS;
  return 1;
}

void handleModifiers(uint32_t modifierKey){
  


};

bool stateHandling(int topPin, int bottomPin, int currentState) {
  static int pressCount[9][21] = {0};
  static bool buttonState[9][21] = {0};
  

  int flags = currentState | (buttonState[topPin][bottomPin] << 1);
  //bit shift adds a zero to the right. current state is an int and has eight bits so we need the other bit to make the or operation happen correctly
  //... why didn't I just make current state a bool???

  // Comparing current state vs last state
  // 1 is pressed 0 is open
  if(!isModiferKey(topBottomKey[topPin][bottomPin])){ 
    switch (flags){
      //00
      case 0:
        //open open we do nothing
        break;

      //01
      case 1:
        //closed open we go up one count and update buttonState
        pressCount[topPin][bottomPin] = pressCount[topPin][bottomPin] + 1;
        buttonState[topPin][bottomPin] = 1;
        break;

      //10
      case 2:
      //was closed now open
      //send keypress and reset counter
        if(tud_suspended()){
          tud_remote_wakeup();
          };
          //lookup which top gpio we're outputting, which bottom gpio we're reading, and then look up which
          //key that corresponds to. Then send that as an HID report
          send_hid_report(REPORT_ID_KEYBOARD,topBottomKey[topPin][bottomPin]);
          pressCount[topPin][bottomPin] = 0;
          buttonState[topPin][bottomPin] = 0;
        break;

      //11
      // we're monitoring if it's being held down with prescount here
      case 3:
        pressCount[topPin][bottomPin] = pressCount[topPin][bottomPin] + 1;
        if (pressCount[topPin][bottomPin] > 5) {
          
          pressCount[topPin][bottomPin] = 0;
          
          if(tud_suspended()){
            tud_remote_wakeup();
            };
          while(gpio_get(bottomPin)) send_hid_report(REPORT_ID_KEYBOARD,topBottomKey[topPin][bottomPin]);

        }

        break;
      
      //it feels like a sin to not put this
      default:
        break;
      }; //end of non modifier keys
    } //end of non modifier keys
  
//modifier state handling
  else {
      if(currentState==1) currentModifier |=topBottomKey[topPin][bottomPin];
      else currentModifier &= ~(topBottomKey[topPin][bottomPin]);
    
  /*
    switch (flags) {
      case 0:
        //open open
        //currentModifier &= ~(1 << topBottomKey[topPin][bottomPin]); 
        //maybe remove current modifiers but shouldn't be necissary
        //currentModifier = 0;
        currentModifier = 2;
        break;

      case 1:
            //0 1
            //was open now cloased we go up one count and update buttonState
            // add control key modifiers
        //pressCount[topPin][bottomPin] = pressCount[topPin][bottomPin] + 1;
        buttonState[topPin][bottomPin] = 1;
        currentModifier=2; 
        //currentModifier |= (topBottomKey[topPin][bottomPin]);
        break;

      case 2:
        //1 0 first release
        // remove current control key modifiers
      // update button state
        //currentModifier &= ~(1 << topBottomKey[topPin][bottomPin]); 
        //currentModifier = 0;
        currentModifier = 2;
        buttonState[topPin][bottomPin] = 0;
      case 3:
        //closed closed
        // add modifier to global modifiers
      // this needs to be more sensative than normal keys. ingore button count?
        //currentModifier |= (topBottomKey[topPin][bottomPin]);
      currentModifier=2;
      default:
        break;
    }*/
  };

}

bool scanKeyboard(){
  int topPin;
  int bottomPin;
  int currentState;

  //  checkModifers();
//don't forget to account for sleep mode.
  for (int i = 0; i < 9;i++){
    topPin = topSend[i];
    gpio_put(topPin,1);
    for (int j = 0; j < 10;j++){
      bottomPin = bottomRecieve[j];
      currentState = gpio_get(bottomPin);

      if (allowButtonPress(topPin,bottomPin)) stateHandling(topPin,bottomPin,currentState);

      };
    //return topSend GPIO pin to low once we're done with it
    gpio_put(topSend[i],0);
  };
  tud_task();
}



void keyboard_task(){
  int n = 0;
  while (n < 10){
  scanKeyboard();
  n = n+1;
  
  tud_task(); // tinyusb device task
  }
};
/*
bool scanKeyboard(){
  checkModifers();
  bool istyping = 0;
//don't forget to account for sleep mode.
  for (int i = 0; i < 9;i++){
    gpio_put(topSend[i],1);
    for (int j = 0; j < 10;j++){
      if(gpio_get(bottomRecieve[j])&&allowButtonPress(topSend[i],bottomRecieve[j])){
        
        //WAKE UP BRO.
        if(tud_suspended()){
          tud_remote_wakeup();
        };

        //lookup which top gpio we're outputting, which bottom gpio we're reading, and then look up which
        //key that corresponds to. Then send that as an HID report
        send_hid_report(REPORT_ID_KEYBOARD,topBottomKey[topSend[i]][bottomRecieve[j]]);
      };
      };
    //return topSend GPIO pin to low once we're done with it
    gpio_put(topSend[i],0);
  };
  tud_task();
  return istyping;
}
void keyboard_task(){
  int n = 0;
  while (n < 10){
  scanKeyboard();
  n = n+1;
  
  tud_task(); // tinyusb device task
  }
};
*/

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  scanKeyboard(); 
  //keyboard_task();
 
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


/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();
  gpio_init_tasks();
  defineKeys();
  stdio_init_all();

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();
    scanKeyboard();
    ///hid_task();
  }
}


