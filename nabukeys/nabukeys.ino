#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "config.h"
#include "nabukeysconfig.h"
#include "nabukeyslib.h"
#include "nabukeyswrite.h"
#include "usbkeysconfig.h"
#include "usbkeyslib.h"

#ifdef USBK_USE_HOST_SHIELD
#include <hidboot.h>
#include <usbhub.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#endif

void setup() {
  pinMode(NK_SW_HOST_CLIENT_MODE, INPUT_PULLUP);
  pinMode(NK_SW_SERIAL_USB_MODE, INPUT_PULLUP);
  if (digitalRead(NK_SW_HOST_CLIENT_MODE)) {
    if (digitalRead(NK_SW_SERIAL_USB_MODE)) {
      pinMode(NK_SW_SETTING_1, INPUT_PULLUP);
      pinMode(NK_SW_SETTING_2, INPUT_PULLUP);
      if (digitalRead(NK_SW_SETTING_1)) {
        if (digitalRead(NK_SW_SETTING_2)) {
          serial_input_debug();
        } else {
          serial_input_ascii();
        }
      } else {
        if (digitalRead(NK_SW_SETTING_2)) {
          serial_input_hex();
        } else {
          serial_input_raw();
        }
      }
    } else {
      usb_input_mode();
    }
  } else {
    if (digitalRead(NK_SW_SERIAL_USB_MODE)) {
      pinMode(NK_SW_SETTING_1, INPUT_PULLUP);
      pinMode(NK_SW_SETTING_2, INPUT_PULLUP);
      if (digitalRead(NK_SW_SETTING_1)) {
        if (digitalRead(NK_SW_SETTING_2)) {
          serial_output_debug();
        } else {
          serial_output_ascii();
        }
      } else {
        if (digitalRead(NK_SW_SETTING_2)) {
          serial_output_hex();
        } else {
          serial_output_raw();
        }
      }
    } else {
      usb_output_mode();
    }
  }
}

void loop() {}

void serial_input_debug() {
  unsigned long lastTime = 0;
  boolean trackTime;
  boolean hideIdle;
  int ch;
  pinMode(NK_SW_SETTING_3, INPUT_PULLUP);
  pinMode(NK_SW_SETTING_4, INPUT_PULLUP);
  trackTime = !digitalRead(NK_SW_SETTING_3);
  hideIdle = !digitalRead(NK_SW_SETTING_4);
  nk_start_input();
  Serial.begin(9600);
  Serial.println("READY");
  while (true) {
    if ((ch = nk_read()) >= 0) {
      if (hideIdle && (ch == NK_WATCHDOG)) {
        continue;
      }
      if (trackTime) {
        unsigned long time = millis();
        Serial.print(time - lastTime);
        Serial.write(' ');
        lastTime = time;
      }
      if (ch >= 0xE0) {
        switch (ch & NK_FN_STATE) {
          case NK_PRESSED:  Serial.print("PRESSED ");  break;
          case NK_RELEASED: Serial.print("RELEASED "); break;
          default:          Serial.print("??? ");      break;
        }
        Serial.print(ch, HEX);
        Serial.write(' ');
        switch (ch & NK_FN_KEY) {
          case NK_RT_ARROW: Serial.println("Right");    break;
          case NK_LT_ARROW: Serial.println("Left");     break;
          case NK_UP_ARROW: Serial.println("Up");       break;
          case NK_DN_ARROW: Serial.println("Down");     break;
          case NK_FORWARD:  Serial.println("Forward");  break;
          case NK_BACKWARD: Serial.println("Backward"); break;
          case NK_NO:       Serial.println("No");       break;
          case NK_YES:      Serial.println("Yes");      break;
          case NK_SYM:      Serial.println("Sym");      break;
          case NK_PAUSE:    Serial.println("Pause");    break;
          case NK_TV_NABU:  Serial.println("TV/NABU");  break;
          default:          Serial.println("???");      break;
        }
      } else if (ch >= 0xC0) {
        Serial.print("??? ");
        Serial.println(ch, HEX);
      } else if (ch >= 0xA0) {
        Serial.print("JOYSTICK ");
        Serial.print(ch, HEX);
        Serial.write(' ');
        Serial.write((ch & NK_JS_FIRE) ? 'F' : '-');
        Serial.write((ch & NK_JS_UP  ) ? 'U' : '-');
        Serial.write((ch & NK_JS_RT  ) ? 'R' : '-');
        Serial.write((ch & NK_JS_DN  ) ? 'D' : '-');
        Serial.write((ch & NK_JS_LT  ) ? 'L' : '-');
        Serial.println();
      } else if (ch >= 0x90) {
        Serial.print("STATUS ");
        Serial.print(ch, HEX);
        Serial.write(' ');
        switch (ch) {
          case NK_MULTIPLE:    Serial.println("Multiple");    break;
          case NK_FAULTY_RAM:  Serial.println("Faulty RAM");  break;
          case NK_FAULTY_ROM:  Serial.println("Faulty ROM");  break;
          case NK_ILLEGAL_ISR: Serial.println("Illegal ISR"); break;
          case NK_WATCHDOG:    Serial.println("Watchdog");    break;
          case NK_RESET:       Serial.println("Reset");       break;
          default:             Serial.println("???");         break;
        }
      } else if (ch >= 0x80) {
        Serial.print("JOYSTICK ");
        Serial.print(ch, HEX);
        Serial.print(" JS");
        Serial.println((ch & 15) + 1);
      } else {
        Serial.print("ASCII ");
        if (ch < 0x10) Serial.write('0');
        Serial.print(ch, HEX);
        Serial.write(' ');
        switch (ch) {
          case NK_TAB:    Serial.println("Tab");    break;
          case NK_GO:     Serial.println("Go");     break;
          case NK_ESC:    Serial.println("Esc");    break;
          case NK_SPACE:  Serial.println("Space");  break;
          case NK_DELETE: Serial.println("Delete"); break;
          default:
            if (ch < 0x20) {
              Serial.print("Ctrl ");
              Serial.write(ch | '@');
            } else {
              Serial.write(ch);
            }
            Serial.println();
            break;
        }
      }
    }
  }
}

void serial_input_ascii() {
  int sym = 0x00;
  int goKey, deleteKey;
  int yesKey, noKey;
  int ch;
  pinMode(NK_SW_SETTING_3, INPUT_PULLUP);
  pinMode(NK_SW_SETTING_4, INPUT_PULLUP);
  if (digitalRead(NK_SW_SETTING_3)) {
    goKey = NK_GO;
    deleteKey = NK_DELETE;
  } else {
    goKey = 0x0A;
    deleteKey = 0x08;
  }
  if (digitalRead(NK_SW_SETTING_4)) {
    yesKey = 0x06;
    noKey = 0x15;
  } else {
    yesKey = 0x19;
    noKey = 0x0E;
  }
  nk_start_input();
  Serial.begin(9600);
  while (true) {
    if ((ch = nk_read()) >= 0) {
      if (ch >= 0xE0) {
        switch (ch) {
          case NK_SYM | NK_PRESSED:  sym = 0x80; break;
          case NK_SYM | NK_RELEASED: sym = 0x00; break;
          case NK_YES:      Serial.write(sym | yesKey); break;
          case NK_BACKWARD: Serial.write(sym | 0x11); break;
          case NK_FORWARD:  Serial.write(sym | 0x12); break;
          case NK_PAUSE:    Serial.write(sym | 0x13); break;
          case NK_TV_NABU:  Serial.write(sym | 0x14); break;
          case NK_NO:       Serial.write(sym | noKey); break;
          case NK_LT_ARROW: Serial.write(sym | 0x1C); break;
          case NK_RT_ARROW: Serial.write(sym | 0x1D); break;
          case NK_UP_ARROW: Serial.write(sym | 0x1E); break;
          case NK_DN_ARROW: Serial.write(sym | 0x1F); break;
        }
      } else if (ch < 0x80) {
        switch (ch) {
          case NK_GO:     Serial.write(sym | goKey); break;
          case NK_DELETE: Serial.write(sym | deleteKey); break;
          default:        Serial.write(sym | ch);
        }
      }
    }
  }
}

void serial_input_hex() {
  int ch;
  nk_start_input();
  Serial.begin(9600);
  while (true) {
    if ((ch = nk_read()) >= 0) {
      if (ch < 0x10) Serial.write('0');
      Serial.print(ch, HEX);
    }
  }
}

void serial_input_raw() {
  int ch;
  nk_start_input();
  Serial.begin(9600);
  while (true) {
    if ((ch = nk_read()) >= 0) {
      Serial.write(ch);
    }
  }
}

static void usbk_macro_pressed(unsigned int action) {
  for (int e = 0xE0, m = 0x100; e < 0xE8; m <<= 1, e++) {
    if (action & m) {
      usbk_key_pressed(e);
      delay(50);
    }
  }
  usbk_key_pressed(action);
}

static void usbk_macro_released(unsigned int action) {
  usbk_key_released(action);
  for (int e = 0xE0, m = 0x100; e < 0xE8; m <<= 1, e++) {
    if (action & m) {
      delay(50);
      usbk_key_released(e);
    }
  }
}

void usb_input_mode() {
  int ch;
  unsigned char fnKey;
  unsigned char pauseKey;
  unsigned char tvNabuKey;
  unsigned char symKey;
  unsigned int backwardKey;
  unsigned int forwardKey;
  unsigned int yesKey;
  unsigned int noKey;
  boolean fnDown = false;
  pinMode(NK_LED, OUTPUT);
  pinMode(NK_SW_SETTING_1, INPUT_PULLUP);
  pinMode(NK_SW_SETTING_2, INPUT_PULLUP);
  pinMode(NK_SW_SETTING_3, INPUT_PULLUP);
  pinMode(NK_SW_SETTING_4, INPUT_PULLUP);
  digitalWrite(NK_LED, LOW);
  if (digitalRead(NK_SW_SETTING_1)) {
    fnKey = NK_SYM;
    if (digitalRead(NK_SW_SETTING_2)) {
      pauseKey = USBK_RT_ALT;
      tvNabuKey = USBK_RT_META;
    } else {
      pauseKey = USBK_LT_META;
      tvNabuKey = USBK_LT_ALT;
    }
  } else {
    fnKey = NK_TV_NABU;
    if (digitalRead(NK_SW_SETTING_2)) {
      pauseKey = USBK_RT_META;
      symKey = USBK_RT_ALT;
    } else {
      pauseKey = USBK_LT_ALT;
      symKey = USBK_LT_META;
    }
  }
  if (digitalRead(NK_SW_SETTING_4)) {
    backwardKey = USBK_HOME;
    forwardKey = USBK_END;
    if (digitalRead(NK_SW_SETTING_3)) {
      yesKey = USBK_PGUP;
      noKey = USBK_PGDN;
    } else {
      yesKey = ((USBK_MOD_LT_SHIFT << 8) | USBK_Y);
      noKey = ((USBK_MOD_LT_SHIFT << 8) | USBK_N);
    }
  } else {
    backwardKey = USBK_PGUP;
    forwardKey = USBK_PGDN;
    if (digitalRead(NK_SW_SETTING_3)) {
      yesKey = USBK_HOME;
      noKey = USBK_END;
    } else {
      yesKey = ((USBK_MOD_LT_SHIFT << 8) | USBK_BACKSLASH);
      noKey = USBK_BACKSLASH;
    }
  }
  nk_start_input();
  usbk_start_output();
  while (true) {
    if ((ch = nk_read()) >= 0) {
      if (ch == (fnKey | NK_PRESSED)) {
        fnDown = true;
        digitalWrite(NK_LED, HIGH);
      } else if (ch == (fnKey | NK_RELEASED)) {
        fnDown = false;
        digitalWrite(NK_LED, LOW);
      } else if (ch >= 0xE0) {
        switch (ch) {
          case NK_RT_ARROW | NK_PRESSED: usbk_key_pressed(fnDown ? USBK_END : USBK_RIGHT); break;
          case NK_LT_ARROW | NK_PRESSED: usbk_key_pressed(fnDown ? USBK_HOME : USBK_LEFT); break;
          case NK_UP_ARROW | NK_PRESSED: usbk_key_pressed(fnDown ? USBK_PGUP : USBK_UP); break;
          case NK_DN_ARROW | NK_PRESSED: usbk_key_pressed(fnDown ? USBK_PGDN : USBK_DOWN); break;
          case NK_FORWARD  | NK_PRESSED: usbk_macro_pressed(forwardKey); break;
          case NK_BACKWARD | NK_PRESSED: usbk_macro_pressed(backwardKey); break;
          case NK_NO       | NK_PRESSED: usbk_macro_pressed(noKey); break;
          case NK_YES      | NK_PRESSED: usbk_macro_pressed(yesKey); break;
          case NK_SYM      | NK_PRESSED: usbk_key_pressed(symKey); break;
          case NK_PAUSE    | NK_PRESSED: usbk_key_pressed(pauseKey); break;
          case NK_TV_NABU  | NK_PRESSED: usbk_key_pressed(tvNabuKey); break;
          case NK_RT_ARROW | NK_RELEASED: usbk_key_released(fnDown ? USBK_END : USBK_RIGHT); break;
          case NK_LT_ARROW | NK_RELEASED: usbk_key_released(fnDown ? USBK_HOME : USBK_LEFT); break;
          case NK_UP_ARROW | NK_RELEASED: usbk_key_released(fnDown ? USBK_PGUP : USBK_UP); break;
          case NK_DN_ARROW | NK_RELEASED: usbk_key_released(fnDown ? USBK_PGDN : USBK_DOWN); break;
          case NK_FORWARD  | NK_RELEASED: usbk_macro_released(forwardKey); break;
          case NK_BACKWARD | NK_RELEASED: usbk_macro_released(backwardKey); break;
          case NK_NO       | NK_RELEASED: usbk_macro_released(noKey); break;
          case NK_YES      | NK_RELEASED: usbk_macro_released(yesKey); break;
          case NK_SYM      | NK_RELEASED: usbk_key_released(symKey); break;
          case NK_PAUSE    | NK_RELEASED: usbk_key_released(pauseKey); break;
          case NK_TV_NABU  | NK_RELEASED: usbk_key_released(tvNabuKey); break;
        }
      } else if (ch < 0x80) {
        if ((ch = nk_ascii_to_usb(ch, fnDown))) {
          usbk_macro_pressed(ch);
          delay(50);
          usbk_macro_released(ch);
        }
      }
    }
  }
}

#define SERIAL_BUFFER_SIZE 80

void serial_output_debug() {
  unsigned char buf[SERIAL_BUFFER_SIZE];
  int ptr = 0;
  int ch;
  nk_start_output();
  Serial.begin(9600);
  Serial.println("READY");
  while (true) {
    if ((ch = Serial.read()) > 0) {
      if (ch == '\n' || ch == '\r') {
        buf[ptr] = 0;
        if      (buf[0] == '!') Serial.println(free_ram());
        else if (buf[0] == '#') /* "comment" / no-op */;
        else if (buf[0] == '$') nk_write_hex  (&buf[1]);
        else if (buf[0] == '&') nk_write_coded(&buf[1]);
        else if (buf[0] == '@') nk_write_ascii(&buf[1]);
        else                    nk_write_ascii(&buf[0]);
        ptr = 0;
      } else if (ptr < (SERIAL_BUFFER_SIZE - 1)) {
        buf[ptr] = ch;
        ptr++;
      }
    }
    nk_write_task();
  }
}

void serial_output_ascii() {
  int ch;
  nk_start_output();
  Serial.begin(9600);
  while (true) {
    if ((ch = Serial.read()) >= 0) {
      if (ch & 0x80) {
        nk_write(NK_SYM | NK_PRESSED);
        nk_write_delay(NK_WRITE_DELAY);
      }
      switch (ch & 0x7F) {
        case 0x06: nk_write_macro(0x3000 | NK_YES);      break;
        case 0x08: nk_write_macro(0x1000 | NK_DELETE);   break;
        case 0x0A: nk_write_macro(0x1000 | NK_GO);       break;
        case 0x0E: nk_write_macro(0x3000 | NK_NO);       break;
        case 0x11: nk_write_macro(0x3000 | NK_BACKWARD); break;
        case 0x12: nk_write_macro(0x3000 | NK_FORWARD);  break;
        case 0x13: nk_write_macro(0x3000 | NK_PAUSE);    break;
        case 0x14: nk_write_macro(0x3000 | NK_TV_NABU);  break;
        case 0x15: nk_write_macro(0x3000 | NK_NO);       break;
        case 0x19: nk_write_macro(0x3000 | NK_YES);      break;
        case 0x1C: nk_write_macro(0x3000 | NK_LT_ARROW); break;
        case 0x1D: nk_write_macro(0x3000 | NK_RT_ARROW); break;
        case 0x1E: nk_write_macro(0x3000 | NK_UP_ARROW); break;
        case 0x1F: nk_write_macro(0x3000 | NK_DN_ARROW); break;
        default:   nk_write_macro(0x1000 | (ch & 0x7F)); break;
      }
      if (ch & 0x80) {
        nk_write(NK_SYM | NK_RELEASED);
        nk_write_delay(NK_WRITE_DELAY);
      }
    }
    nk_write_task();
  }
}

static unsigned char digit_value(unsigned char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
  if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
  return 0xFF;
}

void serial_output_hex() {
  boolean inValue = false;
  unsigned char value;
  unsigned char ch;
  nk_start_output();
  Serial.begin(9600);
  while (true) {
    if ((ch = digit_value(Serial.read())) < 16) {
      if (inValue) {
        inValue = false;
        nk_write(value | ch);
        nk_write_delay(NK_WRITE_DELAY);
        value = 0;
      } else {
        inValue = true;
        value = ch << 4;
      }
    }
    nk_write_task();
  }
}

void serial_output_raw() {
  int ch;
  nk_start_output();
  Serial.begin(9600);
  while (true) {
    if ((ch = Serial.read()) >= 0) {
      nk_write(ch);
      nk_write_delay(NK_WRITE_DELAY);
    }
    nk_write_task();
  }
}

void usb_output_mode() {
  unsigned char modState = 0;
  unsigned char ledState = 0;
  unsigned int e;
  nk_start_output();
  usbk_start_input();
  while (true) {
    if ((e = usbk_read_keys())) {
      if (e & USBK_EVENT_MOD_STATE) {
        char prevSym = modState & (USBK_MOD_ANY_ALT | USBK_MOD_ANY_META);
        char nextSym = e        & (USBK_MOD_ANY_ALT | USBK_MOD_ANY_META);
        if (nextSym && !prevSym) nk_write(NK_SYM | NK_PRESSED);
        if (prevSym && !nextSym) nk_write(NK_SYM | NK_RELEASED);
        modState = e;
      } else if (e & USBK_EVENT_KEY_DOWN) {
        if ((e & 0xFF) == USBK_CAPS_LOCK) {
          ledState ^= USBK_LED_CAPS_LOCK;
          usbk_write_leds(ledState);
        }
        if ((e = nk_usb_to_ascii(
          e,
          modState & USBK_MOD_ANY_CTRL,
          modState & USBK_MOD_ANY_SHIFT,
          ledState & USBK_LED_CAPS_LOCK,
          false
        ))) {
          nk_write_macro(e);
        }
      } else if (e & USBK_EVENT_KEY_UP) {
        if ((e = nk_usb_to_ascii(
          e,
          modState & USBK_MOD_ANY_CTRL,
          modState & USBK_MOD_ANY_SHIFT,
          ledState & USBK_LED_CAPS_LOCK,
          true
        ))) {
          nk_write_macro(e);
        }
      }
    }
    nk_write_task();
  }
}

int free_ram() {
  extern int __heap_start, * __brkval; int v;
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}
