NAME = nanos-lite
SRCS = $(shell find -L ./src/ -name "*.c" -o -name "*.cpp" -o -name "*.S")
LIBS = klib

include $(AM_HOME)/Makefile.app

FSIMG_PATH = $(NAVY_HOME)/fsimg
RAMDISK_FILE = build/ramdisk.img

OBJCOPY_FLAG = -R .note.gnu.property -S --set-section-flags .bss=alloc,contents -O binary
# OBJCOPY_FILE = $(NAVY_HOME)/tests/dummy/build/dummy-x86
# 20210501 try this
OBJCOPY_APP = $(NAVY_HOME)/tests/hello
OBJCOPY_FILE = $(OBJCOPY_APP)/build/$(notdir $(OBJCOPY_APP))-$(ISA)

.PHONY: update update-ramdisk-objcopy update-ramdisk-fsimg update-fsimg

update-ramdisk-objcopy:
    # 20210501 add first line, try this 
	$(MAKE) -s -C $(OBJCOPY_APP) ISA=$(ISA)
	$(OBJCOPY) $(OBJCOPY_FLAG) $(OBJCOPY_FILE) $(RAMDISK_FILE)
	touch src/files.h

update-fsimg:
	$(MAKE) -s -C $(NAVY_HOME) ISA=$(ISA)

update-ramdisk-fsimg: update-fsimg
	$(eval FSIMG_FILES := $(shell find $(FSIMG_PATH) -type f))
	@for f in $(FSIMG_FILES); do \
		if $(READELF) -h $$f 2> /dev/null > /dev/null; then \
			$(OBJCOPY) $(OBJCOPY_FLAG) $$f; \
		fi \
	done
	@cat $(FSIMG_FILES) > $(RAMDISK_FILE)
	@wc -c $(FSIMG_FILES) | grep -v 'total$$' | sed -e 's+ $(FSIMG_PATH)+ +' | awk -v sum=0 '{print "\x7b\x22" $$2 "\x22\x2c " $$1 "\x2c " sum "\x7d\x2c";sum += $$1}' > src/files.h

src/syscall.h: $(NAVY_HOME)/libs/libos/src/syscall.h
	ln -sf $^ $@

update: update-ramdisk-fsimg src/syscall.h
	@touch src/initrd.S
