#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  int key = _read_key();
  bool is_down = false;
  if(key & 0x8000 ) {
    key ^= 0x8000;
    is_down = true;
  }
  if(key == _KEY_NONE) {
    uint32_t ut = _uptime();
    sprintf(buf, "t %d\n", ut);
  } 
  else {
    sprintf(buf, "%s %s\n", is_down ? "kd" : "ku", keyname[key]);
  }
  
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  // strncpy(buf, dispinfo + offset, len);
  memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
  int r = (offset/4) / _screen.width;
  int c = (offset/4) % _screen.width;
  _draw_rect((uint32_t*)buf, c, r, len/4, 1);
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d..\n",_screen.width,_screen.height);
}
