lintstyle = "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 132}"
sdkprefix = /opt/amiga/bin/m68k-amigaos-
crt=nix20
size = $(sdkprefix)size
CC = $(sdkprefix)gcc
CFLAGS = -Os -fomit-frame-pointer -msmall-code -m68000
CFLAGS += -Wall
CFLAGS += -mcrt=$(crt)
LDFLAGS += -mcrt=$(crt)
.PHONY: all
all: rdbinfo devtostream streamtodev xdevtostream xstreamtodev size
rdbinfo: rdbinfo.o util.o getdevices.o devices.o
streamtodev: streamtodev.o util.o getdevices.o devices.o
xstreamtodev: xstreamtodev.o util.o getdevices.o devices.o
devtostream: devtostream.o util.o getdevices.o devices.o
xdevtostream: xdevtostream.o util.o getdevices.o devices.o
xstreamtodev.o: streamtodev.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -D EXPERT_VERSION=1 $< -o $@
xdevtostream.o: devtostream.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -D EXPERT_VERSION=1 $< -o $@
.PHONY: sizes
size:
	$(size) rdbinfo devtostream streamtodev xdevtostream xstreamtodev
.PHONY: lint
lint:
	@echo "* linting sources (code style)"
	clang-format -i --style=$(lintstyle) *.c *.h
.PHONY: clean
clean:
	@echo "* Removing build artifacts..."
	rm -f rdbinfo devtostream xdevtostream streamtodev xstreamtodev *.o
