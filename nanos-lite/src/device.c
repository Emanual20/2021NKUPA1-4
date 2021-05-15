#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  Log("should not reach here now..");
  return 0;
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
  sprintf(dispinfo, "WIDTH:%d..HEIGHT:%d..\n",_screen.width,_screen.height);
}
