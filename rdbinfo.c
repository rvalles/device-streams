/* --------------------------------------------------
 |  NAME
 |    rdbinfo
 |  PURPOSE
 |    get info on all or some RDB devices.
 |
 |  NOTES
 |    only works for RDB partitions.
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
#include <ctype.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/rdargs.h>
#include <signal.h>
#include <string.h>
#define __GNU_LIBRARY__ 1
#include <getopt.h>
#undef __GNU_LIBRARY__
#include "getdevices.h"
#if defined(SASC)
/* we will handle this ourselves. */
int __regargs chkabort(void) { return 0; }
int __regargs Chk_Abort(void) { return 0; }
#endif
void get_info(struct List *dl, char *dev_name, char *part_name, ulong unit, ulong start_block, ulong end_block);
struct option long_options[] = {{"rdb-name", required_argument, NULL, 'n'},
                                {"start-block", required_argument, NULL, 's'},
                                {"end-block", required_argument, NULL, 'e'},
                                {"device", required_argument, NULL, 'd'},
                                {"unit", required_argument, NULL, 'u'},
                                {"verbose", no_argument, NULL, 'V'},
                                {"expert-mode", no_argument, NULL, 'x'},
                                {"debug-mode", no_argument, NULL, 'g'},
                                {"help", no_argument, NULL, 'h'},
                                {"version", no_argument, NULL, 'v'},
                                {NULL, 0, NULL, 0}};
char *short_options = "?vVxghn:s:e:d:u:";
char *cmd_vers_string = "\0$VERS rdbinfo 2.2.1 (2023-03-21)";
char *version_string = "rdbinfo V2.2.1 -- (C) 2023 Roc Valles Domenech, 1993 Christian E. Hopps\n";
char *help_string = "Usage: %s [options]\n"
                    "Options:\n"
                    "    -[vVxghnsedu] [--rdb-name=partition_name] [--expert-mode]\n"
                    "    [--start-block=block] [--end-block=block] [--debug-mode]\n"
                    "    [--device=device_name] [--unit=unit_num] [--version]\n"
                    "    [--verbose] [--help]"
                    "\n"
                    "Number Formats: (where `n\' are alpha-num. digits)\n"
                    "    0[xX]nnn | [xX]nnn | nnn[hH] | $nnn - for Hex\n"
                    "    nnn[oO] - for octal\n"
                    "    nnn[bB] - for binary\n"
                    "    nnn - for decimal (also default for non-recognized)\n"
                    "\n"
                    "    given the above you can also postpend a [MKk] for Megabyte\n"
                    "    Kilobyte and kilobyte respectively. [range checking inuse]";
char *opt_rdb_name;
char *opt_device_name;
ulong opt_unit = -1;        /* -1 for any */
ulong opt_start_block = -1; /* -1 for any */
ulong opt_end_block = -1;   /* -1 for any */
ulong opt_verbose;
ulong opt_expert;
ulong opt_debug;
FILE *mout;
FILE *min;
int main(int argc, char **argv) {
    int ret = 0;
    int opt;
    int opt_quit = 0;
    int opt_version = 0;
    int opt_help = 0;
    int longind = 0;
    signal(SIGINT, SIG_IGN);
    mout = stdout;
    min = stdin;
    if (!argc) {
        printf("%s\nStart from Workbench not supported. Please start from Shell.\n", version_string);
        return (ret);
    }
    struct List *dl;
    while (EOF != (opt = getopt_long(argc, argv, short_options, long_options, &longind))) {
        switch (opt) {
        case 'v':
            opt_version = 1;
            opt_quit = 1;
            break;
        case 'V':
            opt_verbose = 1;
            break;
        case 'x':
            opt_expert = 1;
            break;
        case '?':
        case 'h':
            opt_help = 1;
            opt_quit = 1;
            break;
        case 'n':
            opt_rdb_name = optarg;
            break;
        case 'd':
            opt_device_name = optarg;
            break;
        case 's':
            if (!(string_to_number(optarg, &opt_start_block))) {
                opt_quit = 1;
                opt_help = 1;
                ret = 20;
            }
            break;
        case 'e':
            if (!(string_to_number(optarg, &opt_end_block))) {
                opt_quit = 1;
                opt_help = 1;
                ret = 20;
            }
            break;
        case 'u':
            if (!(string_to_number(optarg, &opt_unit))) {
                opt_quit = 1;
                opt_help = 1;
                ret = 20;
            }
            break;
        case 'g':
            opt_debug = 1;
        }
    }
    if (opt_version) {
        message(version_string, argv[0]);
    }
    if (opt_help) {
        message(help_string, argv[0], short_options, 2 + strlen(argv[0]), "", 2 + strlen(argv[0]), "", 2 + strlen(argv[0]), "");
    }
    if (opt_quit) {
        return (ret);
    }
    dl = get_drive_list();
    if (dl) {
        get_info(dl, opt_device_name, opt_rdb_name, opt_unit, opt_start_block, opt_end_block);
        free_drive_list(dl);
    }
    return (ret);
}
void get_info(struct List *dl, char *dev_name, char *part_name, ulong unit, ulong start_block, ulong end_block) {
    struct Node *dn, *un, *pn;
    int ctrlc = 0;
    unsigned long long bytes;
    /* walk list of devices. */
    for (dn = dl->lh_Head; (!ctrlc) && dn->ln_Succ; ctrlc = check_break(), dn = dn->ln_Succ) {
        struct device *d = ptrfrom(struct device, node, dn);
        if (dev_name == NULL || (!strcasecmp(dev_name, d->name))) {
            /* walk list of units. */
            for (un = d->units.lh_Head; (!ctrlc) && un->ln_Succ; ctrlc = check_break(), un = un->ln_Succ) {
                Unit *u = ptrfrom(Unit, node, un);
                int unit_printed = 0;
                if (unit == (ulong)-1 || (u->unit == unit)) {
                    /* walk list of partitions. */
                    for (pn = u->parts.lh_Head; (!ctrlc) && pn->ln_Succ; ctrlc = check_break(), pn = pn->ln_Succ) {
                        Partition *p = ptrfrom(Partition, node, pn);
                        if (part_name && strcasecmp(p->name, part_name)) {
                            continue;
                        }
                        if (start_block != (ulong)-1 && start_block != p->start_block) {
                            continue;
                        }
                        if (end_block != (ulong)-1 && end_block != p->end_block) {
                            continue;
                        }
                        if (unit_printed == 0) {
                            bytes = (unsigned long long)u->total_blocks * u->bytes_per_block;
                            message("Device: \"%s\"  Unit: %lu  Capacity: %llu.%llu Megs", u->name, u->unit, megs(bytes),
                                    tenths_of_a_meg(bytes));
                            message("DiskVendor: %.8s DiskProduct %.16s "
                                    "DiskRevision: %.4s",
                                    u->rdb->rdb_DiskVendor, u->rdb->rdb_DiskProduct, u->rdb->rdb_DiskRevision);
                            message("Cylinders: %lu  Heads: %lu  "
                                    "Blks-p-Trk: %lu "
                                    "[Blks-p-Cyl: %lu]",
                                    u->cylinders, u->heads, u->blocks_per_track, u->heads * u->blocks_per_track);
                            message("Total Blocks: %lu  Block Size %lu", u->total_blocks, u->bytes_per_block);
                            if (bytes >= 1ULL << 32)
                                message("64bit");
                            unit_printed = 1;
                        }
                        bytes = (unsigned long long)p->total_blocks * p->block_size;
                        message("\n--| Partition: \"%s\" Capacity: %llu.%llu Megs", p->name, megs(bytes), tenths_of_a_meg(bytes));
                        message("--| Start Block: %lu  End Block: %lu "
                                "Total Blocks: %lu",
                                p->start_block, p->end_block, p->total_blocks);
                        message("--| Block Size: %lu", p->block_size);
                        if (bytes >= 1ULL << 32 || (unsigned long long)p->block_size * p->end_block >= 1ULL << 32)
                            message("--| 64bit");
                    }
                    if (unit_printed) {
                        message("###");
                    }
                }
            }
        }
    }
}
