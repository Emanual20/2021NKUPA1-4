include Makefile.check

APPS = $(dir $(shell find apps tests -mindepth 2 -name "Makefile"))
export AM_HOME ?= /home/rexxar/PA2021_0308/ics2018/nexus-am
export NEMU_HOME ?= /home/rexxar/PA2021_0308/ics2018/nemu
export NAVY_HOME ?= /home/rexxar/PA2021_0308/ics2018/navy-apps

.PHONY: fsimg $(APPS) $(LIBS) clean

fsimg: $(APPS)

$(APPS): %: $(LIBS)
	-$(MAKE) -C $@ install

clean:
	$(foreach app, $(shell ls apps/), $(MAKE) -C $(NAVY_HOME)/apps/$(app) clean ;)
	$(foreach lib, $(shell ls libs/), $(MAKE) -C $(NAVY_HOME)/libs/$(lib) clean ;)
	$(foreach test, $(shell ls tests/), $(MAKE) -C $(NAVY_HOME)/tests/$(test) clean ;)
	rm -f fsimg/bin/*
