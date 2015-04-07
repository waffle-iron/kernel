#include <kernel/keys.h>

#define _2(k) k, k
#define _4(k) k, k, k, k

/** Table for conversion from 'IBM AT-Style Keyboard' to system keys */
unsigned char key_layout_us[128][4] = {
  { '\0', '\0', '\0', '\0' }, // 00
  { _4(KEY_ESCAPE) }, // 01 - Escape
  { '1', '!', '\0', '\0' }, // 02 - Numeric 1
  { '2', '@', '\0', '\0' }, // 03
  { '3', '#', '\0', '\0' }, // 04
  { '4', '$', '\0', '\0' }, // 05
  { '5', '%', '\0', '\0' }, // 06
  { '6', '^', '\0', '\0' }, // 07
  { '7', '&', '\0', '\0' }, // 08
  { '8', '*', '\0', '\0' }, // 09
  { '9', '(', '\0', '\0' }, // 0a
  { '0', ')', '\0', '\0' }, // 0b
  { '-', '_', '\0', '\0' }, // 0c
  { '=', '+', '\0', '\0' }, // d0
  { _2(KEY_BACKSPACE), '\0', '\0' }, // 0e - Backspace
  { _2(KEY_TAB), '\0', '\0' }, // 0f - Tabs
  { 'q', 'Q', '\0', '\0' }, // 10 - Alpha line 1
  { 'w', 'W', '\0', '\0' }, // 11
  { 'e', 'E', '\0', '\0' }, // 12
  { 'r', 'R', '\0', '\0' }, // 13
  { 't', 'T', '\0', '\0' }, // 14
  { 'y', 'Y', '\0', '\0' }, // 15
  { 'u', 'U', '\0', '\0' }, // 16
  { 'i', 'I', '\0', '\0' }, // 17
  { 'o', 'O', '\0', '\0' }, // 18
  { 'p', 'P', '\0', '\0' }, // 19
  { '[', '{', '\0', '\0' }, // 1a
  { ']', '}', '\0', '\0' }, // 1b
  { _2(KEY_ENTER), '\0', '\0' }, // 1c - Enter
  { _2(KEY_CTRL), '\0', '\0' }, // 1d - Left Ctrl
  { 'a', 'A', '\0', '\0' }, // 1e - Alpha line 2
  { 's', 'S', '\0', '\0' }, // 1f
  { 'd', 'D', '\0', '\0' }, // 20
  { 'f', 'F', '\0', '\0' }, // 21
  { 'g', 'G', '\0', '\0' }, // 22
  { 'h', 'H', '\0', '\0' }, // 23
  { 'j', 'J', '\0', '\0' }, // 23
  { 'k', 'K', '\0', '\0' }, // 24
  { 'l', 'L', '\0', '\0' }, // 25
  { ';', ':', '\0', '\0' }, // 27
  { '\'', '"', '\0', '\0' }, // 28
  { '`', '~', '\0', '\0' }, // 29 - Tilde
  { _4(KEY_SH_LF) }, // 2a - Left Shift
  { '\\', '|', '\0', '\0' }, // 2b - Alpha line 3
  { 'z', 'Z', '\0', '\0' }, // 2c
  { 'x', 'X', '\0', '\0' }, // 2d
  { 'c', 'C', '\0', '\0' }, // 2e
  { 'v', 'V', '\0', '\0' }, // 2f
  { 'b', 'B', '\0', '\0' }, // 30
  { 'n', 'N', '\0', '\0' }, // 31
  { 'm', 'M', '\0', '\0' }, // 32
  { ',', '<', '\0', '\0' }, // 33
  { '.', '>', '\0', '\0' }, // 34
  { '/', '?', '\0', '\0' }, // 35
  { _4(KEY_SH_RG) }, // 36 - Right Shift
  { '7', '3', '\0', '\0' }, // 37  Gray *
  { _4(KEY_ALT) }, // 38 - Left Alt
  { ' ', ' ', '\0', '\0' }, // 39 - Space
  { _4(KEY_CAPSLOCK) }, // 3a - Caps Lock
  { _4(KEY_F1) }, // 3b - F1
  { _4(KEY_F2) }, // 3c -
  { _4(KEY_F3) }, // 3d -
  { _4(KEY_F4) }, // 3e -
  { _4(KEY_F5) }, // 3f -
  { _4(KEY_F6) }, // 40 -
  { _4(KEY_F7) }, // 41 -
  { _4(KEY_F8) }, // 42 -
  { _4(KEY_F9) }, // 43 -
  { _4(KEY_F10) }, // 44 - F10
  { _4(KEY_NUMLOCK) }, // 45 - Num Lock
  { _4(KEY_SCROLL) }, // 46 - Scroll Lock
  { _4(KEY_HOME) }, // 47 - Home             PAD_7
  { _4(KEY_ARROW_UP) }, // 48 - Arrow up     PAD_8
  { _4(KEY_PAGE_UP) }, // 49 - Page Up       PAD_9
  { 'a', '4', '\0', '\0' }, // 4a  [Gray Minus]
  { _4(KEY_ARROW_LF) }, // 4b - Arrow left   PAD_4
  { 'c', '4', '\0', '\0' }, // 4c            PAD_5
  { _4(KEY_ARROW_RG) }, // 4d - Arrow right  PAD_6
  { 'e', '4', '\0', '\0' }, // 4e   [Gray Plus]
  { _4(KEY_END) }, // 4f - End               PAD_1
  { _4(KEY_ARROW_DW) }, // 50 - Arrow down   PAD_2
  { _4(KEY_PAGE_DW) }, // 51 - Page Down     PAD_3
  { _4(KEY_INSERT) }, // 52 - Insert         PAD_0
  { _4(KEY_DELETE) }, // 53 - Delete         PAD_DOT
  { _4(KEY_NUMLOCK) }, // 54 - Num. Lock     PrtScr/SysReq
  { '\0', '\0', '\0', '\0' }, // 55
  { '\\', '|', '\0', '\0' }, // 56 - "Before Z"
  { _4(KEY_F11) }, // 57 - F11
  { _4(KEY_F12) }, // 58 - F12
  { '\0', '\0', '\0', '\0' }, // 59
  { _4(KEY_PAUSE) }, // 5a - Pause/Break
  { _4(KEY_HOST) }, // 5b - Left Host
  { _4(KEY_HOST) }, // 5c - Right Host
  { _4(KEY_MENU) }, // 5d - Right Menu
  { '\0', '\0', '\0', '\0' }, // 5e
  { '\0', '\0', '\0', '\0' }, // 5f
  { '\0', '\0', '\0', '\0' }, // 60
  { '\0', '\0', '\0', '\0' }, // 61
  { '\0', '\0', '\0', '\0' }, // 62
  { '\0', '\0', '\0', '\0' }, // 63
  { '\0', '\0', '\0', '\0' }, // 64
  { '\0', '\0', '\0', '\0' }, // 65
  { '\0', '\0', '\0', '\0' }, // 66
  { '\0', '\0', '\0', '\0' }, // 67
  { '\0', '\0', '\0', '\0' }, // 68
  { '\0', '\0', '\0', '\0' }, // 69
  { '\0', '\0', '\0', '\0' }, // 6a
  { '\0', '\0', '\0', '\0' }, // 6b
  { '\0', '\0', '\0', '\0' }, // 6c
  { '\0', '\0', '\0', '\0' }, // 6d
  { '\0', '\0', '\0', '\0' }, // 6e
  { '\0', '\0', '\0', '\0' }, // 6f
  { '\0', '\0', '\0', '\0' }, // 70
  { '\0', '\0', '\0', '\0' }, // 71
  { '\0', '\0', '\0', '\0' }, // 72
  { '\0', '\0', '\0', '\0' }, // 73
  { '\0', '\0', '\0', '\0' }, // 74
  { '\0', '\0', '\0', '\0' }, // 75
  { '\0', '\0', '\0', '\0' }, // 76
  { '\0', '\0', '\0', '\0' }, // 77
  { '\0', '\0', '\0', '\0' }, // 78
  { '\0', '\0', '\0', '\0' }, // 79
  { '\0', '\0', '\0', '\0' }, // 7a
  { '\0', '\0', '\0', '\0' }, // 7b
  { '\0', '\0', '\0', '\0' }, // 7c
  { '\0', '\0', '\0', '\0' }, // 7d
  { '\0', '\0', '\0', '\0' }, // 7e
  { '\0', '\0', '\0', '\0' }, // 7f
};
