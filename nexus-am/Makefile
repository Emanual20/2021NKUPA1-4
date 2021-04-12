include Makefile.check

# AM_HOME ?= $(shell pwd)
export AM_HOME ?= /home/rexxar/PA2021_0308/ics2018/nexus-am
export NEMU_HOME ?= /home/rexxar/PA2021_0308/ics2018/nemu
export NAVY_HOME ?= /home/rexxar/PA2021_0308/ics2018/navy-apps

all:
	@$(MAKE) -C am

# clean everything
ALLMAKE = $(dir $(shell find . -mindepth 2 -name "Makefile"))
$(ALLMAKE):
	-@$(MAKE) -s -C $@ clean

clean: $(ALLMAKE)

.PHONY: all clean $(ALLMAKE)
