lintstyle = "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 132}"
sdkprefix = /opt/amiga/bin/m68k-amigaos-
crt = nix20
lha = lha
binaries = rdbinfo devtostream streamtodev xdevtostream xstreamtodev
commonobj = util.o getdevices.o devices.o common.o
size = $(sdkprefix)size
CC = $(sdkprefix)gcc
CFLAGS = -Os -fomit-frame-pointer -msmall-code -m68000
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -mcrt=$(crt)
LDFLAGS += -mcrt=$(crt)
.PHONY: all
all: $(binaries) size
rdbinfo: rdbinfo.o $(commonobj)
streamtodev: streamtodev.o $(commonobj)
xstreamtodev: xstreamtodev.o $(commonobj)
devtostream: devtostream.o $(commonobj)
xdevtostream: xdevtostream.o $(commonobj)
xstreamtodev.o: streamtodev.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -D EXPERT_VERSION=1 $< -o $@
xdevtostream.o: devtostream.c
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
	rm -f $(binaries) *.o devstreams.lha
devstreams.lha: $(binaries)
	@echo "* Archiving into lha..."
	mkdir devstreams
	cp -a $(binaries) devstreams/
	$(lha) a $(lhaflags) $@ devstreams
	rm -r devstreams
	$(lha) v $@
