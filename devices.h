/* --------------------------------------------------
 |  NAME
 |    devices
 |  PURPOSE
 |    provide simple routines and access to an exec device.
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
#if !defined(_DEVICES_H)
#define _DEVICES_H
#include "util.h"
#include <exec/devices.h>
#include <exec/io.h>
#include <exec/ports.h>
// Errors
#define IOERR_OPENFAIL 0xFF
#define IOERR_NOCMD 0xFD
// TD64/Trackdisk64
#define io_HighOffset io_Actual
#define TD_READ64 24
#define TD_WRITE64 25
#define TD_SEEK64 26
#define TD_FORMAT64 27
// NSD
#define NSCMD_DEVICEQUERY 0x4000
#define NSCMD_TD_READ64 0xC000
#define NSCMD_TD_WRITE64 0xC001
#define NSCMD_TD_SEEK64 0xC002
#define NSCMD_TD_FORMAT64 0xC003
// NSDeviceQueryResult/nsdqr
typedef struct nsdqr_s {
    unsigned int DevQueryFormat;
    unsigned int SizeAvailable;
    unsigned short DeviceType;
    unsigned short DeviceSubType;
    unsigned short *SupportedCommands; // pointer to 0-terminated array of commands.
} NSDQR;
// 64bit device support level
enum { DEVICE_APILEVEL_UNKNOWN, DEVICE_APILEVEL_32BIT, DEVICE_APILEVEL_NSD, DEVICE_APILEVEL_TD64 };
DeviceData *init_device(char *name, ulong unit, ulong flags, ulong iosize);
int open_device(DeviceData *dd);
void close_device(DeviceData *dd);
void free_device(DeviceData *dd);
#endif /* _DEVICES_H */
