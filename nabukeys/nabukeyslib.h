#ifndef NABUKEYSLIB_H
#define NABUKEYSLIB_H

#define NK_FN_STATE       0xF0
#define NK_PRESSED        0xE0
#define NK_RELEASED       0xF0

#define NK_FN_KEY         0xEF
#define NK_RT_ARROW       0xE0
#define NK_LT_ARROW       0xE1
#define NK_UP_ARROW       0xE2
#define NK_DN_ARROW       0xE3
#define NK_FORWARD        0xE4
#define NK_BACKWARD       0xE5
#define NK_NO             0xE6
#define NK_YES            0xE7
#define NK_SYM            0xE8
#define NK_PAUSE          0xE9
#define NK_TV_NABU        0xEA

#define NK_JS_DATA        0xA0
#define NK_JS_FIRE        0x10
#define NK_JS_UP          0x08
#define NK_JS_RT          0x04
#define NK_JS_DN          0x02
#define NK_JS_LT          0x01

#define NK_MULTIPLE       0x90
#define NK_FAULTY_RAM     0x91
#define NK_FAULTY_ROM     0x92
#define NK_ILLEGAL_ISR    0x93
#define NK_WATCHDOG       0x94
#define NK_RESET          0x95

#define NK_JS_COMMAND     0x80
#define NK_JS1            0x80
#define NK_JS2            0x81

#define NK_TAB            0x09
#define NK_GO             0x0D
#define NK_ESC            0x1B
#define NK_SPACE          0x20
#define NK_DELETE         0x7F

#define NK_BAUD_RATE      6992
#define NK_WATCHDOG_RATE  3650
#define NK_REPEAT_RATE_0  575
#define NK_REPEAT_RATE_1  295
#define NK_REPEAT_RATE_2  150
#define NK_REPEAT_RATE_3  80
#define NK_REPEAT_RATE_4  50
#define NK_REPEAT_RATE_5  32
#define NK_WRITE_DELAY    10

void nk_start_input();
int nk_available();
int nk_read();

void nk_start_output();
void nk_write(unsigned char packet);
void nk_write_repeat(unsigned char packet);
void nk_write_release();
void nk_write_task();
void nk_write_delay(unsigned long ms);
void nk_write_macro(unsigned int action);

unsigned int nk_ascii_to_usb(unsigned char ch, boolean alt);
unsigned int nk_usb_to_ascii(unsigned char packet, boolean ctrl, boolean shift, boolean capslock, boolean released);

#endif
