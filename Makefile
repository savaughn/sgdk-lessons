export GDK ?= /opt/toolchains/mars/m68k-elf
include $(GDK)/makefile.gen

run:
	make && ../md-skeleton/blastem/blastem out/rom.bin
	