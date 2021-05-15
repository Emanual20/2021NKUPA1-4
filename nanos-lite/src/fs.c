#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void *buf, off_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* path, int flags, int mode){
  Log("Pathname: %s", path);
  for(int i=0;i<NR_FILES;i++){
    if(strcmp(file_table[i].name, path) == 0){
      return i;
    }
  }
  assert(0);
  return -1;
}

size_t fs_filesz(int fd){
  return file_table[fd].size;
}

ssize_t fs_read(int fd, void* buf, size_t len){
  ssize_t f_size = fs_filesz(fd);
  if(file_table[fd].open_offset + len > f_size){
    len = f_size - file_table[fd].open_offset;
  }
  Log("Reading %s..open_offset:%d,disk_offset:%d,len:%d",
      file_table[fd].name,
      file_table[fd].open_offset,
      file_table[fd].disk_offset,
      len);
  switch(fd){
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR:{
      return 0;
      break;
    }
    // TODO: maybe remain some bugs ..
    default:{
      ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
    }
  }
  return len;
}

ssize_t fs_write(int fd, const void* buf, size_t len){
  ssize_t f_size = fs_filesz(fd);
  switch(fd){
    case FD_STDOUT:
    case FD_STDERR:{
      for(int i=0;i<len;i++){
        _putc(((char*)buf)[i]);
      }
      break;
    }
    case FD_FB:{
      // TODO: maybe have bugs
      break;
    }
    default:{
      if(file_table[fd].open_offset + len > f_size){
        len = f_size - file_table[fd].open_offset;
      }
      Log("Writing %s..open_offset:%d,disk_offset:%d,len:%d",
      file_table[fd].name,
      file_table[fd].open_offset,
      file_table[fd].disk_offset,
      len);
      ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
    }
  }
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence){
  off_t ret = -1;
  switch(whence){
    case SEEK_SET:{
      if(offset>=0 && offset<=file_table[fd].size){
        file_table[fd].open_offset = offset;
        ret = file_table[fd].open_offset;
      }
      break;
    }
    case SEEK_CUR:{
      if(offset+file_table[fd].open_offset>=0 && offset+file_table[fd].open_offset<=file_table[fd].size){
        file_table[fd].open_offset += offset;
        ret = file_table[fd].open_offset;
      }
      break;
    }
    case SEEK_END:{
      file_table[fd].open_offset = file_table[fd].size + offset;
      ret = file_table[fd].open_offset;
      break;
    }
  }
  return ret;
}

int fs_close(int fd){
  return 0;
}