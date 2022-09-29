/* --------------------------------------------------
 |  NAME
 |    getdevices
 |  PURPOSE
 |    header for getdevices.c
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
 |   chopps - Oct 9, 1993: Created.
 +--------------------------------------------------- */
#if !defined(_GETDEVICES_H)
#define _GETDEVICES_H
#include "devices.h"
#include "util.h"
struct List *get_drive_list(void);
void free_drive_list(struct List *l);
int add_name_to_drive_list(struct List *l, char *dev_name);
char *get_hard_drive_device_name(struct DosList *dl);
#endif /* _GETDEVICES_H */
