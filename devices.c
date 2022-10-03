/* --------------------------------------------------
 |  NAME
 |    devices
 |  PURPOSE
 |    handle exec devices in a standard way.
 |  NOTES
 |
 |  COPYRIGHT
 |    Copyright (C) 1993  Christian E. Hopps
 |
 |    This program is free software; you can redistribute it and/or modify
 |    it under the terms of the GNU General Public License as published by
 |    the Free Software Foundation; either version 2 of the License, or
 |    (at your option) any later version.
 |
 |    This program is distributed in the hope that it will be useful,
 |    but WITHOUT ANY WARRANTY; without even the implied warranty of
 |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |    GNU General Public License for more details.
 |
 |    You should have received a copy of the GNU General Public License
 |    along with this program; if not, write to the Free Software
 |    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 |
 |  HISTORY
 |    chopps - Oct 9, 1993: Created.
 +--------------------------------------------------- */
#include "devices.h"
#include "util.h"
#include <stdbool.h>
bool test_td64(DeviceData *dd) {
    if (dd) {
    }
    return false;
}
bool test_nsd(DeviceData *dd) {
    if (dd) {
    }
    return false;
}
short test_apilevel(DeviceData *dd) {
    if (dd) {
        if (test_td64(dd))
            return DEVICE_APILEVEL_TD64;
        if (test_nsd(dd))
            return DEVICE_APILEVEL_NSD;
        return DEVICE_APILEVEL_32BIT;
    }
    return DEVICE_APILEVEL_UNKNOWN;
}
/* returns structure with device open. */
DeviceData *alloc_device(char *name, ulong unit, ulong flags, ulong iosize) {
    DeviceData *dd = zmalloc(sizeof(*dd));
    if (NULL == dd) {
        return (NULL);
    }
    dd->port = CreateMsgPort();
    if (NULL == dd->port) {
        free_device(dd);
        return (NULL);
    }
    dd->io = CreateIORequest(dd->port, iosize);
    if (NULL == dd->io) {
        free_device(dd);
        return (NULL);
    }
    dd->name = copy_string(name);
    if (NULL == dd->name) {
        free_device(dd);
        return (NULL);
    }
    dd->unit = unit;
    dd->flags = flags;
    if (open_device(dd)) {
        free_device(dd);
        return (NULL);
    }
    dd->apilevel = test_apilevel(dd);
    return (dd);
}
void free_device(DeviceData *dd) {
    if (dd) {
        close_device(dd);
        DeleteIORequest(dd->io);
        DeleteMsgPort(dd->port);
        zfree(dd->name);
    }
}
int open_device(DeviceData *dd) {
    int error = -1;
    if (dd && !dd->open) {
        error = OpenDevice((unsigned char *)dd->name, dd->unit, dd->io, dd->flags);
        if (!error) {
            dd->open = 1;
        } else {
            if (-1 != error) {
                D(debug_message("warn: unable to open \"%s\" unit: %lu flags 0x%lx", dd->name, dd->unit, dd->flags));
                D(debug_message("      reason: error %ld", error));
            }
            dd->open = 0;
        }
    }
    return (error);
}
void close_device(DeviceData *dd) {
    if (dd) {
        if (dd->open) {
            if (!CheckIO(dd->io)) {
                AbortIO(dd->io);
                WaitIO(dd->io);
            }
            CloseDevice(dd->io);
            dd->open = 0;
        }
    }
}
/* returns actual number of bytes read or written, or -1 for error. */
ulong device_readwrite(DeviceData *dd, unsigned long long offset, ulong bytes, void *buffer, bool write) {
    D(debug_message("dd %lu, Offset: 0x%llX, bytes: %lu, buffer: %lu", dd, offset, bytes, buffer));
    if (offset >= 1ULL << 32 || offset + bytes - 1 >= 1ULL << 32) {
        warn_message("64bit I/O request w/o support. Skip and return error.");
        return (-1);
    }
    struct IOStdReq *io = (struct IOStdReq *)dd->io;
    io->io_Length = bytes;
    io->io_Offset = offset; // 32bit. Gets the lower half of offset.
    io->io_Data = buffer;
    if (!device_do_command(dd, write ? CMD_WRITE : CMD_READ)) {
        return (io->io_Actual);
    }
    return (-1);
}
/* returns actual number of bytes read or -1 for error. */
ulong device_read(DeviceData *dd, unsigned long long offset, ulong bytes, void *buffer) {
    return device_readwrite(dd, offset, bytes, buffer, false);
}
/* returns actual number of bytes written or -1 for error. */
ulong device_write(DeviceData *dd, unsigned long long offset, ulong bytes, void *buffer) {
    return device_readwrite(dd, offset, bytes, buffer, true);
}
/* returns the error from DoIO () */
int device_do_command(DeviceData *dd, UWORD command) {
    int error = -1;
    if (dd) {
        if (dd->open) {
            dd->io->io_Command = command;
            error = (int)DoIO(dd->io);
        } else {
            dd->io->io_Error = -1;
        }
    }
    return (error);
}
