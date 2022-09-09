sdkprefix = /opt/amiga/bin/m68k-amigaos-
CFLAGS = -Os -mcrt=nix20
CC = $(sdkprefix)gcc
style = "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 132}"
.PHONY: all
all: rdbinfo devtostream streamtodev xdevtostream xstreamtodev
devices.o: devices.c
	$(CC) -c $(CFLAGS) $< -o $@
getdevices.o: getdevices.c
	$(CC) -c $(CFLAGS) $< -o $@
util.o: util.c
	$(CC) -c $(CFLAGS) $< -o $@
rdbinfo.o: rdbinfo.c
	$(CC) -c $(CFLAGS) $< -o $@
rdbinfo: rdbinfo.o util.o getdevices.o devices.o
	$(CC) $(CFLAGS) $^ -o $@
streamtodev.o: streamtodev.c
	$(CC) -c $(CFLAGS) $< -o $@
streamtodev: streamtodev.o util.o getdevices.o devices.o
	$(CC) $(CFLAGS) $^ -o $@
xstreamtodev.o: streamtodev.c
	$(CC) -c $(CFLAGS) -D EXPERT_VERSION=1 $< -o $@
xstreamtodev: xstreamtodev.o util.o getdevices.o devices.o
	$(CC) $(CFLAGS) $^ -o $@
devtostream.o: devtostream.c
	$(CC) -c $(CFLAGS) $< -o $@
devtostream: devtostream.o util.o getdevices.o devices.o
	$(CC) $(CFLAGS) $^ -o $@
xdevtostream.o: devtostream.c
	$(CC) -c $(CFLAGS) -D EXPERT_VERSION=1 $< -o $@
xdevtostream: xdevtostream.o util.o getdevices.o devices.o
	$(CC) $(CFLAGS) $^ -o $@
.PHONY: lint
lint:
	@echo "* linting sources (code style)"
	clang-format -i --style=$(style) *.c *.h
.PHONY: clean
clean:
	@echo "* Removing build artifacts..."
	rm -f rdbinfo devtostream xdevtostream streamtodev xstreamtodev *.o
