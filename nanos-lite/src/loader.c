#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern size_t get_ramdisk_size();
extern void ramdisk_read(void *, off_t, size_t);

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  int size = get_ramdisk_size();
  void * buff = NULL;
  ramdisk_read(buff, 0, size); 
  return (uintptr_t)DEFAULT_ENTRY;
}
