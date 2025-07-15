export GDK ?= /opt/toolchains/mars/m68k-elf
include $(GDK)/makefile.gen

run:
	make && ../md-skeleton/blastem/blastem out/rom.bin

copy:
	make
	@echo "Copying rom.bin to /Volumes/GENESISSD/rom.bin"
	@cp out/rom.bin /Volumes/GENESISSD/rom.bin
	@if [ $$? -ne 0 ]; then \
		echo "Error: Failed to copy rom.bin"; \
		exit 1; \
	fi
	@echo "Successfully copied rom.bin to /Volumes/GENESISSD/rom.bin"
	@diskutil eject /Volumes/GENESISSD
	