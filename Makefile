sdkprefix = /opt/amiga/bin/m68k-amigaos-
CFLAGS = -Os -mcrt=nix20
CC = $(sdkprefix)gcc
style = "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 132}"
.PHONY: all
all: rdbinfo devtostream streamtodev xdevtostream xstreamtodev
devices.o: devices.c
	@echo "* building devices (utility functions)..."
	$(CC) -c $(CFLAGS) $< -o $@
getdevices.o: getdevices.c
	@echo "* building getdevices (utility functions)..."
	$(CC) -c $(CFLAGS) $< -o $@
util.o: util.c
	@echo "* building util (utility functions)..."
	$(CC) -c $(CFLAGS) $< -o $@
rdbinfo.o: rdbinfo.c
	@echo "* building rdbinfo (shows partition table information)..."
	$(CC) -c $(CFLAGS) $< -o $@
rdbinfo: rdbinfo.o util.o getdevices.o devices.o
	@echo "* linking rdbinfo (shows partition table information)..."
	$(CC) $(CFLAGS) $^ -o $@
streamtodev.o: streamtodev.c
	@echo "* building streamtodev (writes a stream into a device)..."
	$(CC) -c $(CFLAGS) $< -o $@
streamtodev: streamtodev.o util.o getdevices.o devices.o
	@echo "* linking streamtodev (writes a stream into a device)..."
	$(CC) $(CFLAGS) $^ -o $@
xstreamtodev.o: streamtodev.c
	@echo "* building xstreamtodev (writes a stream into a device EXPERT)..."
	$(CC) -c $(CFLAGS) -D EXPERT_VERSION=1 $< -o $@
xstreamtodev: xstreamtodev.o util.o getdevices.o devices.o
	@echo "* linking xstreamtodev (writes a stream into a device EXPERT)..."
	$(CC) $(CFLAGS) $^ -o $@
devtostream.o: devtostream.c
	@echo "* building devtostream (dumps a device into a stream)..."
	$(CC) -c $(CFLAGS) $< -o $@
devtostream: devtostream.o util.o getdevices.o devices.o
	@echo "* linking devtostream (dumps a device into a stream)..."
	$(CC) $(CFLAGS) $^ -o $@
xdevtostream.o: devtostream.c
	@echo "* building xdevtostream (dumps a device into a stream EXPERT)..."
	$(CC) -c $(CFLAGS) -D EXPERT_VERSION=1 $< -o $@
xdevtostream: xdevtostream.o util.o getdevices.o devices.o
	@echo "* linking xdevtostream (dumps a device into a stream EXPERT)..."
	$(CC) $(CFLAGS) $^ -o $@
.PHONY: lint
lint:
	@echo "* linting sources (code style)"
	clang-format -i --style=$(style) *.c *.h
.PHONY: clean
clean:
	@echo "* Removing build artifacts..."
	rm -f rdbinfo devtostream xdevtostream streamtodev xstreamtodev *.o
