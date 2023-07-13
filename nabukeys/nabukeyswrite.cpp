#include <Arduino.h>
#include "nabukeyslib.h"
#include "nabukeyswrite.h"

static unsigned char digit_value(unsigned char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
  if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
  return 0xFF;
}

void nk_write_hex(unsigned char * s) {
  boolean inValue = false;
  unsigned char value;
  unsigned char ch;
  while ((ch = *s)) {
    if ((ch = digit_value(ch)) < 16) {
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
    s++;
  }
}

void nk_write_ascii(unsigned char * s) {
  unsigned char ch;
  while ((ch = *s)) {
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
    s++;
  }
}

static unsigned int nk_write_coded_action(unsigned char state, unsigned char ch) {
  if (state >= 0xE0) {
    // state contains two octal digits (111xxyyy)
    // ch contains the third octal digit (in ASCII)
    return 0x1000 | ((state & 0x1F) << 3) | (digit_value(ch) & 0x07);
  }
  if (state >= 0xD0) {
    // state contains one hex digit (1101xxxx)
    // ch contains the second hex digit (in ASCII)
    return 0x1000 | ((state & 0x0F) << 4) | (digit_value(ch) & 0x0F);
  }
  if (state >= 0xC0) {
    // state contains one octal digit (1100??xx)
    // ch contains a second octal digit (in ASCII)
    return 0x80E0 | ((state & 0x03) << 3) | (digit_value(ch) & 0x07);
  }
  if (state == 0x0D) {
    // ch contains one hex digit (in ASCII)
    return 0x80D0 | (digit_value(ch) & 0x0F);
  }
  if (state == 0x0C) {
    // ch contains an uncontrolled character
    return 0x1000 | (ch & 0x1F);
  }
  if (state == 0x0B) {
    // backslash escaped control character
    switch (ch) {
      case '@': return 0x1000; // null
      case 'a': return 0x1007; // bell
      case 'b': return 0x1008; // bs
      case 't': return 0x1009; // ht
      case 'n': return 0x100A; // lf
      case 'v': return 0x100B; // vt
      case 'f': return 0x100C; // ff
      case 'r': return 0x100D; // cr
      case 'o': return 0x100E; // so
      case 'i': return 0x100F; // si
      case 'z': return 0x101A; // eof
      case 'e': return 0x101B; // esc
      case 'd': return 0x107F; // del
      case 'c': return 0x800C; // control
      case 'x': return 0x800D; // hex
      case '0': return 0x80C0; // octal 0xy
      case '1': return 0x80C1; // octal 1xy
      case '2': return 0x80C2; // octal 2xy
      case '3': return 0x80C3; // octal 3xy
      case 'R': return 0x3000 | NK_RT_ARROW;
      case 'L': return 0x3000 | NK_LT_ARROW;
      case 'U': return 0x3000 | NK_UP_ARROW;
      case 'D': return 0x3000 | NK_DN_ARROW;
      case 'F': return 0x3000 | NK_FORWARD;
      case 'B': return 0x3000 | NK_BACKWARD;
      case 'N': return 0x3000 | NK_NO;
      case 'Y': return 0x3000 | NK_YES;
      case 'S': return 0x1000 | NK_SYM;
      case 's': return 0x2000 | NK_SYM;
      case 'P': return 0x3000 | NK_PAUSE;
      case 'T': return 0x3000 | NK_TV_NABU;
      case 'J': return 0x8008; // joystick command
      case 'E': return 0x8009; // error code
      case 'j': return 0x800A; // joystick data
      case 'H': return 0x8004; // start repeat
      case 'h': return 0x0800; // end repeat
      default:  return 0x1000 | ch;
    }
  }
  if (state == 0x0A) {
    // joystick data
    return 0x10A0 | (ch & 0x1F);
  }
  if (state == 0x09) {
    // error code
    if ((ch = digit_value(ch)) != 0xFF) ch--;
    return 0x1090 | (ch & 0x0F);
  }
  if (state == 0x08) {
    // joystick command
    if ((ch = digit_value(ch)) != 0xFF) ch--;
    return 0x1080 | (ch & 0x0F);
  }
  if (state == 0x04) {
    // start repeat
    return 0x0400 | ch;
  }
  if (ch == '\\') return 0x800B;
  return 0x1000 | ch;
}

void nk_write_coded(unsigned char * s) {
  unsigned char state = 0;
  unsigned char ch;
  unsigned int action;
  while ((ch = *s)) {
    if (ch >= 0x20 && ch < 0x7F) {
      action = nk_write_coded_action(state, ch);
      nk_write_macro(action);
      if (action & 0x8000) {
        state = action;
      } else {
        state = 0;
      }
    }
    s++;
  }
}
