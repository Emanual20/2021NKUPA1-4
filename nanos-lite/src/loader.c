#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern size_t get_ramdisk_size();
extern void ramdisk_read(void *, off_t, size_t);

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  int size = get_ramdisk_size();
  printf("size:%d\n",size);
  ramdisk_read((void *)DEFAULT_ENTRY, 0, size); 
  return (uintptr_t)DEFAULT_ENTRY;
}
