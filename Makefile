sdkprefix = /opt/amiga/bin/m68k-amigaos-
CFLAGS = -Os -mcrt=nix20
CC = $(sdkprefix)gcc
style = "{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 132}"
.PHONY: all
all: rdbinfo
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
.PHONY: lint
lint:
	@echo "* linting sources (code style)"
	clang-format -i --style=$(style) *.c *.h
.PHONY: clean
clean:
	@echo "* Removing build artifacts..."
	rm -f rdbinfo devtostream xdevtostream streamtodev xstreamtodev *.o
