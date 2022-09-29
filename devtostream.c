/* --------------------------------------------------
 |  NAME
 |    devtostream
 |  PURPOSE
 |    dump all data on a device to a stream.
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
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#define __GNU_LIBRARY__ 1
#include <getopt.h>
#undef __GNU_LIBRARY__
#include "getdevices.h"
#include <sys/unistd.h>
#if defined(SASC)
/* we will handle this ourselves. */
int __regargs chkabort(void) { return 0; }
int __regargs Chk_Abort(void) { return 0; }
#endif
void dev_to_file(char *name, ulong unit, ulong bpb, BPTR file, ulong cb, ulong end);
int check_values(Partition *p, ulong st, ulong end, int exp);
struct option long_options[] = {{"output", required_argument, NULL, 'o'},
                                {"rdb-name", required_argument, NULL, 'n'},
#if defined(EXPERT_VERSION)
                                {"start-block", required_argument, NULL, 's'},
                                {"end-block", required_argument, NULL, 'e'},
                                {"expert-mode", no_argument, NULL, 'x'},
#endif
                                {"device", required_argument, NULL, 'd'},
                                {"unit", required_argument, NULL, 'u'},
                                {"buffer-blocks", required_argument, NULL, 'b'},
                                {"verbose", no_argument, NULL, 'V'},
                                {"debug-mode", no_argument, NULL, 'g'},
                                {"help", no_argument, NULL, 'h'},
                                {"version", no_argument, NULL, 'v'},
                                {"quiet", no_argument, NULL, 'q'},
                                {NULL, 0, NULL, 0}};
char *short_options = "?qvVghn:d:u:b:o:"
#if defined(EXPERT_VERSION)
                      "xs:e:"
#endif
    ;
char *cmd_vers_string = "\0$VERS devtostream 1.99 (dev)";
char *version_string = "devtostream V1.99-dev -- (C) 2022 Roc Valles Domenech, 1993 Christian E. Hopps\n";
char *help_string = "Usage: %s [options]\n"
                    "Options:\n"
                    "    -[vVxghnsedubo] [--output=file] [--rdb-name=partition_name]\n"
                    "    [--device=device_name] [--unit=unit_num] [--version]\n"
#if defined(EXPERT_VERSION)
                    "    [--start-block=block] [--end-block=block] [--expert-mode]\n"
#endif
                    "    [--buffer-blocks=blocks] [--verbose] [--quiet] [--help]\n"
                    "\n"
                    "Number Formats: (where `n\' are alpha-num. digits)\n"
                    "    0[xX]nnn | [xX]nnn | nnn[hH] | $nnn - for Hex\n"
                    "    nnn[oO] - for octal\n"
                    "    nnn[bB] - for binary\n"
                    "    nnn - for decimal (also default for non-recognized)\n"
                    "\n"
                    "    given the above you can also postpend a [MKk] for Megabyte\n"
                    "    Kilobyte and kilobyte respectively. [range checking inuse]";
char *opt_outfile_name;
char *opt_rdb_name;
char *opt_device_name;
ulong opt_unit = -1;        /* -1 for invalid */
ulong opt_start_block = -1; /* -1 for invalid. */
ulong opt_end_block = -1;   /* -1 for invalid */
ulong opt_verbose;
ulong opt_expert;
ulong opt_debug;
ulong opt_quiet = 0;
ulong number_of_buffer_blocks = 128;
FILE *mout;
FILE *min;
int main(int argc, char **argv) {
    int ret = 0;
    int opt;
    int opt_quit = 0;
    int opt_version = 0;
    int opt_help = 0;
    int longind = 0;
    struct List *dl;
    BPTR file = 0;
    signal(SIGINT, SIG_IGN);
    mout = stdout;
    min = stdin;
    if (!argc) {
        printf("%s\nStart from Workbench not supported. Please start from Shell.\n", version_string);
        return 0;
    }
    while (EOF != (opt = getopt_long(argc, argv, short_options, long_options, &longind))) {
        switch (opt) {
        case 'q':
            opt_quiet = 1;
            break;
        case 'v':
            opt_version = 1;
            opt_quit = 1;
            break;
        case 'V':
            opt_verbose = 1;
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
        case 'o':
            opt_outfile_name = optarg;
            break;
        case 'b':
            if (!(string_to_number(optarg, &number_of_buffer_blocks))) {
                opt_quit = 1;
                opt_help = 1;
                ret = 20;
            }
            break;
#if defined(EXPERT_VERSION)
        case 'x':
            opt_expert = 1;
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
#endif /* EXPERT_VERSION */
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
    if (opt_quiet && opt_expert) {
        message("--quiet-mode (-q) and --expert-mode (-x) not allowed at same time.\n");
        opt_quit = 1;
        ret = 20;
    }
    if (opt_version) {
        message(version_string, argv[0]);
    }
    if (opt_help) {
        message(help_string, argv[0]);
    }
    if (opt_quit) {
        return (ret);
    }
    if (!opt_outfile_name) {
        mout = fopen("*", "w+");
        if (!mout) {
            return (20);
        }
        file = Output();
    }
    /* there should be NO messages before this point!! */
    dl = get_drive_list();
    if (dl) {
        Partition *p = find_partition(dl, opt_device_name, opt_rdb_name, opt_unit, opt_start_block, opt_end_block);
        if (p) {
            if (opt_outfile_name) {
                file = Open((unsigned char *)opt_outfile_name, MODE_NEWFILE);
            }
            if (file && !isatty(file)) {
                ulong st, end;
                if (!opt_quiet) {
                    message("found partition: \"%s\" capacity: %llu.%llu Megs", p->name,
                            megs((unsigned long long)p->total_blocks * p->block_size),
                            tenths_of_a_meg((unsigned long long)p->total_blocks * p->block_size));
                    message("start block: %lu  end block: %lu total blocks: %lu", p->start_block, p->end_block, p->total_blocks);
                    message("block Size: %lu", p->block_size);
                }
                st = opt_start_block;
                end = opt_end_block;
                if (st == (ulong)-1) {
                    st = p->start_block;
                }
                if (end == (ulong)-1) {
                    end = p->end_block;
                }
                if (check_values(p, st, end, opt_expert)) {
                    int accepted = 1; // default if opt_quiet
                    if (!opt_quiet) {
                        message("dumping: start block: %lu to end block: %lu [size: %lluK]\n", st, end,
                                ((unsigned long long)end - st) * p->unit->bytes_per_block / 1024);
                        accepted = 'y' == tolower(ask_bool('N', 'y', "write from partition \"%s\" to file \"%s\"", p->name,
                                                           opt_outfile_name ? opt_outfile_name : "stdout"));
                    }
                    if (accepted)
                        dev_to_file(p->unit->name, p->unit->unit, p->unit->bytes_per_block, file, st, end);
                    else
                        message("ok, quiting...");
                }
            } else if (file && isatty(file)) {
                warn_message("Pipes and re-direction will work but interactive\n"
                             "input/output is prohibited.");
            }
            if (file && opt_outfile_name) {
                Close(file);
            }
        } else {
            warn_message("could not locate a partition with your specs.");
        }
        free_drive_list(dl);
    }
    if (!opt_outfile_name) {
        fclose(mout);
    }
    return (0);
}
void dev_to_file(char *name, ulong unit, ulong bpb, BPTR file, ulong cb, ulong end) {
    DeviceData *dd = alloc_device(name, unit, 0, sizeof(struct IOStdReq));
    if (dd) {
        ulong num_buffers = number_of_buffer_blocks;
        ulong total_blocks = end - cb + 1;
        ulong bw = 0;
        void *buffer = zmalloc(num_buffers * bpb);
        if (buffer) {
            while (cb <= end) {
                if (num_buffers > (end - cb + 1)) {
                    num_buffers = (end - cb + 1);
                }
                if (check_break()) {
                    break;
                }
                /* Read from device. */
                if (!opt_quiet) {
                    fprintf(mout, "reading: 0x%08lx -> 0x%08lx  [%3lu%%] \r", cb, cb + num_buffers - 1,
                            ((bw + (num_buffers / 2)) * 100 / total_blocks));
                    fflush(mout);
                }
                if (bpb * num_buffers != device_read(dd, (unsigned long long)cb * bpb, num_buffers * bpb, buffer)) {
                    fprintf(mout, "\n");
                    warn_message("couldn't complete operation, read failed.");
                    break;
                }
                /* Write to file. */
                bw += num_buffers;
                if (!opt_quiet) {
                    fprintf(mout, "writing: 0x%08lx -> 0x%08lx  [%3lu%%] \r", cb, cb + num_buffers - 1, (bw * 100 / total_blocks));
                    fflush(mout);
                }
                if (bpb * num_buffers != (unsigned int)Write(file, buffer, bpb * num_buffers)) {
                    fprintf(mout, "\n");
                    warn_message("couldn't complete operation, write failed.");
                    break;
                }
                cb += num_buffers;
            }
            zfree(buffer);
            fprintf(mout, "\n");
        } else {
            warn_message("couldn't allocate io for operation.");
        }
        free_device(dd);
    } else {
        warn_message("couldn't open device \"%s\" unit: %lu for operation.", name, unit);
    }
}
