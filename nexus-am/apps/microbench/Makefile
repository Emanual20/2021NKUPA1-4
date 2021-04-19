NAME = microbench
SRCS = $(shell find -L ./src/ -name "*.c" -o -name "*.cpp")

INPUT ?= TEST
CFLAGS += -DSETTING_$(INPUT)
CXXFLAGS += -DSETTING_$(INPUT)

export AM_HOME ?= /home/rexxar/PA2021_0308/ics2018/nexus-am
include $(AM_HOME)/Makefile.app
