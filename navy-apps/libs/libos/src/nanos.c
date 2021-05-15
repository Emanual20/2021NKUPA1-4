#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

// TODO: discuss with syscall interface
#ifndef __ISA_NATIVE__

// FIXME: this is temporary

int _syscall_(int type, uintptr_t a0, uintptr_t a1, uintptr_t a2){
  int ret = -1;
  asm volatile("int $0x80": "=a"(ret): "a"(type), "b"(a0), "c"(a1), "d"(a2));
  return ret;
}

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
}

int _open(const char *path, int flags, mode_t mode) {
  return _syscall_(SYS_open, (uintptr_t)path, (uintptr_t)flags, (uintptr_t)mode);
}

int _write(int fd, void *buf, size_t count){
  _syscall_(SYS_write, fd, (uintptr_t)buf, count);
  // _exit(SYS_write);
}

extern char _end;
static intptr_t brk = (intptr_t)&_end;
void *_sbrk(intptr_t increment){
  intptr_t ori_brk = brk;
  intptr_t new_brk = ori_brk + increment;
  // char a[]="**log**";
  // write(66,a,sizeof(a));
  if(_syscall_(SYS_brk, new_brk, 0, 0) == 0){
    brk = new_brk;
    return (void*) ori_brk;
  }
  else return (void *)-1;
}

int _read(int fd, void *buf, size_t count) {
  return _syscall_(SYS_read, (uintptr_t)fd, (uintptr_t)buf, (uintptr_t)count);
}

int _close(int fd) {
  return _syscall_(SYS_close, (uintptr_t)fd, 0, 0);
}

off_t _lseek(int fd, off_t offset, int whence) {
  return _syscall_(SYS_lseek, (uintptr_t)fd, (uintptr_t)offset, (uintptr_t)whence);
}

// The code below is not used by Nanos-lite.
// But to pass linking, they are defined as dummy functions

// not implement but used
int _fstat(int fd, struct stat *buf) {
  return 0;
}

int execve(const char *fname, char * const argv[], char *const envp[]) {
  assert(0);
  return -1;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  return execve(fname, argv, envp);
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}

char **environ;

time_t time(time_t *tloc) {
  assert(0);
  return 0;
}

int signal(int num, void *handler) {
  assert(0);
  return -1;
}

pid_t _fork() {
  assert(0);
  return -1;
}

int _link(const char *d, const char *n) {
  assert(0);
  return -1;
}

int _unlink(const char *n) {
  assert(0);
  return -1;
}

pid_t _wait(int *status) {
  assert(0);
  return -1;
}

clock_t _times(void *buf) {
  assert(0);
  return 0;
}

int _gettimeofday(struct timeval *tv) {
  assert(0);
  tv->tv_sec = 0;
  tv->tv_usec = 0;
  return 0;
}

int _fcntl(int fd, int cmd, ... ) {
  assert(0);
  return 0;
}

int pipe(int pipefd[2]) {
  assert(0);
  return 0;
}

int dup(int oldfd) {
  assert(0);
  return 0;
}

int dup2(int oldfd, int newfd) {
  assert(0);
  return 0;
}

pid_t vfork(void) {
  assert(0);
  return 0;
}

#endif
