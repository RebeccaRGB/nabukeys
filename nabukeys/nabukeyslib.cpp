#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "nabukeysconfig.h"
#include "nabukeyslib.h"
#include "usbkeyslib.h"

static SoftwareSerial nabu(NK_PIN_R, NK_PIN_D);

void nk_start_input() {
  // disable keyboard
  pinMode(NK_PIN_Q, OUTPUT);
  digitalWrite(NK_PIN_Q, LOW);
  // set transceiver to receive mode
  pinMode(NK_PIN_E, OUTPUT);
  digitalWrite(NK_PIN_E, LOW);
  // open serial port
  nabu.begin(NK_BAUD_RATE);
  // enable keyboard
  delay(100);
  digitalWrite(NK_PIN_Q, HIGH);
}

int nk_available() {
  return nabu.available();
}

int nk_read() {
  return nabu.read();
}

static unsigned char repeatPacket = 0;
static unsigned long repeatRate = 0;
static unsigned long repeatTime = 0;
static unsigned long watchdogTime = 0;

void nk_start_output() {
  // disable keyboard
  pinMode(NK_PIN_Q, OUTPUT);
  digitalWrite(NK_PIN_Q, LOW);
  // set transceiver to transmit mode
  pinMode(NK_PIN_E, OUTPUT);
  digitalWrite(NK_PIN_E, HIGH);
  // open serial port
  nabu.begin(NK_BAUD_RATE);
  // enable keyboard
  delay(50);
  digitalWrite(NK_PIN_Q, HIGH);
  delay(50);
  nabu.write(NK_RESET);
}

void nk_write(unsigned char packet) {
  nabu.write(packet);
}

void nk_write_repeat(unsigned char packet) {
  nabu.write(packet);
  repeatPacket = packet;
  repeatRate = NK_REPEAT_RATE_0;
  repeatTime = millis();
}

void nk_write_release() {
  repeatPacket = 0;
  repeatRate = 0;
  repeatTime = 0;
}

void nk_write_task() {
  unsigned long currentTime = millis();
  if (repeatRate) {
    if ((currentTime - repeatTime) >= repeatRate) {
      nabu.write(repeatPacket);
      repeatRate = (
        (repeatRate >= NK_REPEAT_RATE_0) ? NK_REPEAT_RATE_1 :
        (repeatRate >= NK_REPEAT_RATE_1) ? NK_REPEAT_RATE_2 :
        (repeatRate >= NK_REPEAT_RATE_2) ? NK_REPEAT_RATE_3 :
        (repeatRate >= NK_REPEAT_RATE_3) ? NK_REPEAT_RATE_4 :
        NK_REPEAT_RATE_5
      );
      repeatTime = currentTime;
    }
  }
  if ((currentTime - watchdogTime) >= NK_WATCHDOG_RATE) {
    nabu.write(NK_WATCHDOG);
    watchdogTime = currentTime;
  }
}

void nk_write_delay(unsigned long ms) {
  unsigned long time = millis();
  while ((millis() - time) < ms) {
    nk_write_task();
  }
}

void nk_write_macro(unsigned int action) {
  if (action & 0x0100) {
    nk_write(NK_JS1);
    nk_write_delay(NK_WRITE_DELAY);
  }
  if (action & 0x0200) {
    nk_write(NK_JS2);
    nk_write_delay(NK_WRITE_DELAY);
  }
  if (action & 0x0400) {
    nk_write_repeat(action);
    nk_write_delay(NK_WRITE_DELAY);
  }
  if (action & 0x0800) {
    nk_write_release();
    nk_write_delay(NK_WRITE_DELAY);
  }
  if (action & 0x1000) {
    nk_write(action);
    nk_write_delay(NK_WRITE_DELAY);
  }
  if (action & 0x2000) {
    nk_write(action | NK_RELEASED);
    nk_write_delay(NK_WRITE_DELAY);
  }
  if (action & 0x4000) {
    nk_write(NK_JS_DATA);
    nk_write_delay(NK_WRITE_DELAY);
  }
}

static const unsigned int PROGMEM asciiToUSB[] = {
  (USBK_MOD_LT_CTRL << 8) | (USBK_MOD_LT_SHIFT << 8) | USBK_2,
  (USBK_MOD_LT_CTRL << 8) | USBK_A,
  (USBK_MOD_LT_CTRL << 8) | USBK_B,
  (USBK_MOD_LT_CTRL << 8) | USBK_C,
  (USBK_MOD_LT_CTRL << 8) | USBK_D,
  (USBK_MOD_LT_CTRL << 8) | USBK_E,
  (USBK_MOD_LT_CTRL << 8) | USBK_F,
  (USBK_MOD_LT_CTRL << 8) | USBK_G,
  (USBK_MOD_LT_CTRL << 8) | USBK_H,
  USBK_TAB,
  (USBK_MOD_LT_CTRL << 8) | USBK_J,
  (USBK_MOD_LT_CTRL << 8) | USBK_K,
  (USBK_MOD_LT_CTRL << 8) | USBK_L,
  USBK_RETURN,
  (USBK_MOD_LT_CTRL << 8) | USBK_N,
  (USBK_MOD_LT_CTRL << 8) | USBK_O,
  (USBK_MOD_LT_CTRL << 8) | USBK_P,
  (USBK_MOD_LT_CTRL << 8) | USBK_Q,
  (USBK_MOD_LT_CTRL << 8) | USBK_R,
  (USBK_MOD_LT_CTRL << 8) | USBK_S,
  (USBK_MOD_LT_CTRL << 8) | USBK_T,
  (USBK_MOD_LT_CTRL << 8) | USBK_U,
  (USBK_MOD_LT_CTRL << 8) | USBK_V,
  (USBK_MOD_LT_CTRL << 8) | USBK_W,
  (USBK_MOD_LT_CTRL << 8) | USBK_X,
  (USBK_MOD_LT_CTRL << 8) | USBK_Y,
  (USBK_MOD_LT_CTRL << 8) | USBK_Z,
  USBK_ESC,
  (USBK_MOD_LT_CTRL << 8) | (USBK_MOD_LT_SHIFT << 8) | USBK_COMMA,
  (USBK_MOD_LT_CTRL << 8) | USBK_RT_BRACKET,
  (USBK_MOD_LT_CTRL << 8) | (USBK_MOD_LT_SHIFT << 8) | USBK_6,
  (USBK_MOD_LT_CTRL << 8) | (USBK_MOD_LT_SHIFT << 8) | USBK_MINUS,
  USBK_SPACE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_1,
  (USBK_MOD_LT_SHIFT << 8) | USBK_QUOTE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_3,
  (USBK_MOD_LT_SHIFT << 8) | USBK_4,
  (USBK_MOD_LT_SHIFT << 8) | USBK_5,
  (USBK_MOD_LT_SHIFT << 8) | USBK_7,
  USBK_QUOTE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_9,
  (USBK_MOD_LT_SHIFT << 8) | USBK_0,
  (USBK_MOD_LT_SHIFT << 8) | USBK_8,
  (USBK_MOD_LT_SHIFT << 8) | USBK_EQUAL,
  USBK_COMMA,
  USBK_MINUS,
  USBK_PERIOD,
  USBK_SLASH,
  USBK_0,
  USBK_1,
  USBK_2,
  USBK_3,
  USBK_4,
  USBK_5,
  USBK_6,
  USBK_7,
  USBK_8,
  USBK_9,
  (USBK_MOD_LT_SHIFT << 8) | USBK_SEMICOLON,
  USBK_SEMICOLON,
  (USBK_MOD_LT_SHIFT << 8) | USBK_COMMA,
  USBK_EQUAL,
  (USBK_MOD_LT_SHIFT << 8) | USBK_PERIOD,
  (USBK_MOD_LT_SHIFT << 8) | USBK_SLASH,
  (USBK_MOD_LT_SHIFT << 8) | USBK_2,
  (USBK_MOD_LT_SHIFT << 8) | USBK_A,
  (USBK_MOD_LT_SHIFT << 8) | USBK_B,
  (USBK_MOD_LT_SHIFT << 8) | USBK_C,
  (USBK_MOD_LT_SHIFT << 8) | USBK_D,
  (USBK_MOD_LT_SHIFT << 8) | USBK_E,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F,
  (USBK_MOD_LT_SHIFT << 8) | USBK_G,
  (USBK_MOD_LT_SHIFT << 8) | USBK_H,
  (USBK_MOD_LT_SHIFT << 8) | USBK_I,
  (USBK_MOD_LT_SHIFT << 8) | USBK_J,
  (USBK_MOD_LT_SHIFT << 8) | USBK_K,
  (USBK_MOD_LT_SHIFT << 8) | USBK_L,
  (USBK_MOD_LT_SHIFT << 8) | USBK_M,
  (USBK_MOD_LT_SHIFT << 8) | USBK_N,
  (USBK_MOD_LT_SHIFT << 8) | USBK_O,
  (USBK_MOD_LT_SHIFT << 8) | USBK_P,
  (USBK_MOD_LT_SHIFT << 8) | USBK_Q,
  (USBK_MOD_LT_SHIFT << 8) | USBK_R,
  (USBK_MOD_LT_SHIFT << 8) | USBK_S,
  (USBK_MOD_LT_SHIFT << 8) | USBK_T,
  (USBK_MOD_LT_SHIFT << 8) | USBK_U,
  (USBK_MOD_LT_SHIFT << 8) | USBK_V,
  (USBK_MOD_LT_SHIFT << 8) | USBK_W,
  (USBK_MOD_LT_SHIFT << 8) | USBK_X,
  (USBK_MOD_LT_SHIFT << 8) | USBK_Y,
  (USBK_MOD_LT_SHIFT << 8) | USBK_Z,
  USBK_LT_BRACKET,
  USBK_BACKSLASH,
  USBK_RT_BRACKET,
  (USBK_MOD_LT_SHIFT << 8) | USBK_6,
  (USBK_MOD_LT_SHIFT << 8) | USBK_MINUS,
  USBK_TILDE,
  USBK_A,
  USBK_B,
  USBK_C,
  USBK_D,
  USBK_E,
  USBK_F,
  USBK_G,
  USBK_H,
  USBK_I,
  USBK_J,
  USBK_K,
  USBK_L,
  USBK_M,
  USBK_N,
  USBK_O,
  USBK_P,
  USBK_Q,
  USBK_R,
  USBK_S,
  USBK_T,
  USBK_U,
  USBK_V,
  USBK_W,
  USBK_X,
  USBK_Y,
  USBK_Z,
  (USBK_MOD_LT_SHIFT << 8) | USBK_LT_BRACKET,
  (USBK_MOD_LT_SHIFT << 8) | USBK_BACKSLASH,
  (USBK_MOD_LT_SHIFT << 8) | USBK_RT_BRACKET,
  (USBK_MOD_LT_SHIFT << 8) | USBK_TILDE,
  USBK_BACKSPACE,
};

static const unsigned int PROGMEM asciiToFnUSB[] = {
  (USBK_MOD_LT_META << 8) | (USBK_MOD_LT_ALT << 8) | USBK_ESC,
  0,
  (USBK_MOD_LT_CTRL << 8) | USBK_F12,
  (USBK_MOD_LT_CTRL << 8) | USBK_F10,
  (USBK_MOD_LT_CTRL << 8) | USBK_F6,
  (USBK_MOD_LT_CTRL << 8) | USBK_F2,
  (USBK_MOD_LT_CTRL << 8) | USBK_F7,
  (USBK_MOD_LT_CTRL << 8) | USBK_F8,
  0,
  USBK_INS,
  0,
  0,
  0,
  USBK_NUM_ENTER,
  0,
  0,
  0,
  0,
  (USBK_MOD_LT_CTRL << 8) | USBK_F3,
  (USBK_MOD_LT_CTRL << 8) | USBK_F5,
  (USBK_MOD_LT_CTRL << 8) | USBK_F4,
  0,
  (USBK_MOD_LT_CTRL << 8) | USBK_F11,
  (USBK_MOD_LT_CTRL << 8) | USBK_F1,
  (USBK_MOD_LT_CTRL << 8) | USBK_F9,
  0,
  0,
  USBK_NUM_CLEAR,
  0,
  0,
  0,
  (USBK_MOD_LT_CTRL << 8) | (USBK_MOD_LT_ALT << 8) | USBK_DEL,
  (USBK_MOD_LT_SHIFT << 8) | USBK_SPACE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_1,
  (USBK_MOD_LT_SHIFT << 8) | USBK_TILDE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_3,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_4,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_5,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_7,
  USBK_TILDE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_9,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_0,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_8,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_PLUS,
  0x85,
  USBK_NUM_MINUS,
  USBK_NUM_PERIOD,
  USBK_BACKSLASH,
  USBK_NUM_0,
  USBK_NUM_1,
  USBK_NUM_2,
  USBK_NUM_3,
  USBK_NUM_4,
  USBK_NUM_5,
  USBK_NUM_6,
  USBK_NUM_7,
  USBK_NUM_8,
  USBK_NUM_9,
  (USBK_MOD_LT_SHIFT << 8) | USBK_MENU,
  USBK_MENU,
  (USBK_MOD_LT_SHIFT << 8) | 0x85,
  USBK_NUM_PLUS,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_PERIOD,
  (USBK_MOD_LT_SHIFT << 8) | USBK_BACKSLASH,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_2,
  USBK_CAPS_LOCK,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F12,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F10,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F6,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F2,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F7,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F8,
  (USBK_MOD_LT_SHIFT << 8) | USBK_SCRLK,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_5,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_1,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_2,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_3,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_0,
  (USBK_MOD_LT_SHIFT << 8) | USBK_PAUSE,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_6,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_EQUAL,
  USBK_RT_CTRL,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F3,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F5,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F4,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_4,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F11,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F1,
  (USBK_MOD_LT_SHIFT << 8) | USBK_F9,
  (USBK_MOD_LT_SHIFT << 8) | USBK_SYSRQ,
  USBK_RT_SHIFT,
  USBK_NUM_SLASH,
  USBK_BACKSLASH,
  USBK_NUM_TIMES,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_6,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_MINUS,
  USBK_TILDE,
  USBK_CAPS_LOCK,
  USBK_F12,
  USBK_F10,
  USBK_F6,
  USBK_F2,
  USBK_F7,
  USBK_F8,
  USBK_SCRLK,
  USBK_NUM_5,
  USBK_NUM_1,
  USBK_NUM_2,
  USBK_NUM_3,
  USBK_NUM_0,
  USBK_PAUSE,
  USBK_NUM_6,
  USBK_NUM_EQUAL,
  USBK_LT_CTRL,
  USBK_F3,
  USBK_F5,
  USBK_F4,
  USBK_NUM_4,
  USBK_F11,
  USBK_F1,
  USBK_F9,
  USBK_SYSRQ,
  USBK_LT_SHIFT,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_SLASH,
  (USBK_MOD_LT_SHIFT << 8) | USBK_BACKSLASH,
  (USBK_MOD_LT_SHIFT << 8) | USBK_NUM_TIMES,
  (USBK_MOD_LT_SHIFT << 8) | USBK_TILDE,
  USBK_DEL,
};

unsigned int nk_ascii_to_usb(unsigned char ch, boolean alt) {
  if (ch & 0x80) {
    return 0;
  } else if (alt) {
    return pgm_read_word(&asciiToFnUSB[ch]);
  } else {
    return pgm_read_word(&asciiToUSB[ch]);
  }
}

unsigned int nk_usb_to_ascii(unsigned char packet, boolean ctrl, boolean shift, boolean capslock, boolean released) {
  switch (packet) {
    case USBK_A: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x01 : ((shift || capslock) ? 'A' : 'a'));
    case USBK_B: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x02 : ((shift || capslock) ? 'B' : 'b'));
    case USBK_C: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x03 : ((shift || capslock) ? 'C' : 'c'));
    case USBK_D: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x04 : ((shift || capslock) ? 'D' : 'd'));
    case USBK_E: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x05 : ((shift || capslock) ? 'E' : 'e'));
    case USBK_F: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x06 : ((shift || capslock) ? 'F' : 'f'));
    case USBK_G: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x07 : ((shift || capslock) ? 'G' : 'g'));
    case USBK_H: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x08 : ((shift || capslock) ? 'H' : 'h'));
    case USBK_I: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x09 : ((shift || capslock) ? 'I' : 'i'));
    case USBK_J: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x0A : ((shift || capslock) ? 'J' : 'j'));
    case USBK_K: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x0B : ((shift || capslock) ? 'K' : 'k'));
    case USBK_L: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x0C : ((shift || capslock) ? 'L' : 'l'));
    case USBK_M: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x0D : ((shift || capslock) ? 'M' : 'm'));
    case USBK_N: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x0E : ((shift || capslock) ? 'N' : 'n'));
    case USBK_O: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x0F : ((shift || capslock) ? 'O' : 'o'));
    case USBK_P: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x10 : ((shift || capslock) ? 'P' : 'p'));
    case USBK_Q: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x11 : ((shift || capslock) ? 'Q' : 'q'));
    case USBK_R: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x12 : ((shift || capslock) ? 'R' : 'r'));
    case USBK_S: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x13 : ((shift || capslock) ? 'S' : 's'));
    case USBK_T: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x14 : ((shift || capslock) ? 'T' : 't'));
    case USBK_U: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x15 : ((shift || capslock) ? 'U' : 'u'));
    case USBK_V: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x16 : ((shift || capslock) ? 'V' : 'v'));
    case USBK_W: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x17 : ((shift || capslock) ? 'W' : 'w'));
    case USBK_X: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x18 : ((shift || capslock) ? 'X' : 'x'));
    case USBK_Y: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x19 : ((shift || capslock) ? 'Y' : 'y'));
    case USBK_Z: return (released ? 0x0800 : 0x0400) | (ctrl ? 0x1A : ((shift || capslock) ? 'Z' : 'z'));
    case USBK_1: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '!' : '1'));
    case USBK_2: return (ctrl && !shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x00 : (shift ? '@' : '2')));
    case USBK_3: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '#' : '3'));
    case USBK_4: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '$' : '4'));
    case USBK_5: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '%' : '5'));
    case USBK_6: return (ctrl && !shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1E : (shift ? '^' : '6')));
    case USBK_7: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '&' : '7'));
    case USBK_8: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '*' : '8'));
    case USBK_9: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '(' : '9'));
    case USBK_0: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? ')' : '0'));
    case USBK_RETURN: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | NK_GO);
    case USBK_ESC: return (ctrl || released) ? 0 : (0x1000 | NK_ESC);
    case USBK_BACKSPACE: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | NK_DELETE);
    case USBK_TAB: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | NK_TAB);
    case USBK_SPACE: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | NK_SPACE);
    case USBK_MINUS: return (ctrl && !shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1F : (shift ? '_' : '-')));
    case USBK_EQUAL: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '+' : '='));
    case USBK_LT_BRACKET: return (ctrl && shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1B : (shift ? '{' : '[')));
    case USBK_RT_BRACKET: return (ctrl && shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1D : (shift ? '}' : ']')));
    case USBK_BACKSLASH: return (ctrl && shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1C : (shift ? '|' : '\\')));
    case USBK_POUND: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '~' : '#'));
    case USBK_SEMICOLON: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? ':' : ';'));
    case USBK_QUOTE: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '"' : '\''));
    case USBK_TILDE: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '~' : '`'));
    case USBK_COMMA: return (ctrl && !shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1C : (shift ? '<' : ',')));
    case USBK_PERIOD: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '>' : '.'));
    case USBK_SLASH: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | (shift ? '?' : '/'));
    case USBK_F1: case USBK_F7: return (released ? 0x2000 : 0x1000) | NK_YES;
    case USBK_F2: case USBK_F8: return (released ? 0x2000 : 0x1000) | NK_NO;
    case USBK_F3: case USBK_F9: return (released ? 0x2000 : 0x1000) | NK_BACKWARD;
    case USBK_F4: case USBK_F10: return (released ? 0x2000 : 0x1000) | NK_FORWARD;
    case USBK_F5: case USBK_F11: return (released ? 0x2000 : 0x1000) | NK_PAUSE;
    case USBK_F6: case USBK_F12: return (released ? 0x2000 : 0x1000) | NK_TV_NABU;
    case USBK_SYSRQ: return (released ? 0x2000 : 0x1000) | NK_TV_NABU;
    case USBK_SCRLK: return (released ? 0x2000 : 0x1000) | NK_PAUSE;
    case USBK_PAUSE: return (released ? 0x2000 : 0x1000) | NK_PAUSE;
    case USBK_INS: return (released ? 0x2000 : 0x1000) | NK_YES;
    case USBK_HOME: return (released ? 0x2000 : 0x1000) | NK_BACKWARD;
    case USBK_PGUP: return (released ? 0x2000 : 0x1000) | NK_BACKWARD;
    case USBK_DEL: return (released ? 0x2000 : 0x1000) | NK_NO;
    case USBK_END: return (released ? 0x2000 : 0x1000) | NK_FORWARD;
    case USBK_PGDN: return (released ? 0x2000 : 0x1000) | NK_FORWARD;
    case USBK_RIGHT: return (released ? 0x2000 : 0x1000) | NK_RT_ARROW;
    case USBK_LEFT: return (released ? 0x2000 : 0x1000) | NK_LT_ARROW;
    case USBK_DOWN: return (released ? 0x2000 : 0x1000) | NK_DN_ARROW;
    case USBK_UP: return (released ? 0x2000 : 0x1000) | NK_UP_ARROW;
    case USBK_NUM_CLEAR: return (ctrl || released) ? 0 : (0x1000 | NK_ESC);
    case USBK_NUM_SLASH: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '/');
    case USBK_NUM_TIMES: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '*');
    case USBK_NUM_MINUS: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '-');
    case USBK_NUM_PLUS: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '+');
    case USBK_NUM_ENTER: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | NK_GO);
    case USBK_NUM_1: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '1');
    case USBK_NUM_2: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '2');
    case USBK_NUM_3: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '3');
    case USBK_NUM_4: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '4');
    case USBK_NUM_5: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '5');
    case USBK_NUM_6: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '6');
    case USBK_NUM_7: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '7');
    case USBK_NUM_8: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '8');
    case USBK_NUM_9: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '9');
    case USBK_NUM_0: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '0');
    case USBK_NUM_PERIOD: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '.');
    case USBK_102ND: return (ctrl && shift) ? 0 : ((released ? 0x0800 : 0x0400) | (ctrl ? 0x1C : (shift ? '|' : '\\')));
    case USBK_MENU: return (released ? 0x2000 : 0x1000) | NK_TV_NABU;
    case USBK_POWER: return (released ? 0x2000 : 0x1000) | NK_TV_NABU;
    case USBK_NUM_EQUAL: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '=');
    case 0x85: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | ',');
    case 0x86: return ctrl ? 0 : ((released ? 0x0800 : 0x0400) | '=');
    default: return 0;
  }
}
