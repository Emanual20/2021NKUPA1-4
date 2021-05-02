#include "common.h"
#include "syscall.h"

_RegSet* sys_none(_RegSet *r){
  SYSCALL_ARG1(r) = 1;
  return NULL;
}

_RegSet* sys_write(_RegSet *r){
  int fd = SYSCALL_ARG2(r);
  char* buf = (char*) SYSCALL_ARG3(r);
  int len = SYSCALL_ARG4(r);

  if(fd == 1 || fd == 2){
    for(int i = 0; i < len; i++){
      _putc(buf[i]);
    }
    SYSCALL_ARG1(r) = SYSCALL_ARG4(r);
  }
  return NULL;
}

_RegSet* sys_exit(_RegSet *r){
  _halt(SYSCALL_ARG2(r));
  return NULL;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  printf("Now handling syscall ID = %d\n", a[0]);

  switch (a[0]) {
    case SYS_none:{
      return sys_none(r);
      break;
    }
    case SYS_write:{
      return sys_write(r);
      break;
    }
    case SYS_exit:{
      return sys_exit(r);
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}