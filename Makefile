lintstyle = "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 132}"
sdkprefix = /opt/amiga/bin/m68k-amigaos-
crt = nix20
# crt = nix13
# crt = clib2
lha = lha
archive = device-streams.lha
binaries = rdbinfo devtostream streamtodev xdevtostream xstreamtodev
commonobj = util.o getdevices.o devices.o common.o
size = $(sdkprefix)size
#GNU
CC = $(sdkprefix)gcc
CFLAGS = -Os -fomit-frame-pointer -msmall-code -m68000
CFLAGS += -std=c99
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -mcrt=$(crt)
CFLAGS += -D DEBUG_ENABLED_VERSION=1
LDFLAGS += -mcrt=$(crt)
#VBCC (experimental)
# sdkprefix = /opt/amiga/bin/
# CC = $(sdkprefix)vc
# PosixLib = /opt/amiga/share/PosixLib
# # CFLAGS = +aos68k -cpu=68020 -O2 -I$(PosixLib)/include/
# CFLAGS = -cpu=68020 -O2 -I$(PosixLib)/include/
# LDFLAGS= -lposix -lamigas
#Rules
.PHONY: all
all: $(binaries) size
%.o: %.c *.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
rdbinfo: rdbinfo.o $(commonobj)
streamtodev: streamtodev.o $(commonobj)
xstreamtodev: xstreamtodev.o $(commonobj)
devtostream: devtostream.o $(commonobj)
xdevtostream: xdevtostream.o $(commonobj)
xstreamtodev.o: streamtodev.c $(commonobj)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -D EXPERT_VERSION=1 $< -o $@
xdevtostream.o: devtostream.c $(commonobj)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -D EXPERT_VERSION=1 $< -o $@
.PHONY: size
size: $(binaries)
	$(size) $(binaries)
.PHONY: lint
lint:
	@echo "* linting sources (code style)"
	clang-format -i --style=$(lintstyle) *.c *.h
.PHONY: clean
clean:
	@echo "* Removing build artifacts..."
	rm -f $(binaries) *.o $(archive)
$(archive): $(binaries) COPYING README.md
	@echo "* Archiving into lha..."
	mkdir devstreams
	cp -a $(binaries) COPYING README.md devstreams/
	$(lha) a $(lhaflags) $@ devstreams
	rm -r devstreams
	$(lha) v $@
.PHONY: dist
dist: $(archive)
