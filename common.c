#include "util.h"

int check_values(Partition *p, ulong st, ulong end, int exp) {
    unsigned long long offset;
    unsigned long long bytes;
    if (st > end) {
        message("error: Your end block [%lu] is less than your start block [%lu]!\n", st, end);
        return (0);
    }
    if (st < p->start_block || st > p->end_block || end > p->end_block || end < p->start_block) {
        warn_message("ERROR: start and end blocks cannot cross partition boundries.");
        return (0);
    }
    if (st != p->start_block || end != p->end_block) {
        if (exp) {
            message("Please note the values you gave for start and end\n"
                    "do NOT match the partition in question.");
        } else {
            message("error: you must set specify the `expert-mode\' argument to gain\n"
                    "       access inside the partition (ie. not the exact\n"
                    "       same block numbers as the partition's).");
            return (0);
        }
    }
    offset = (unsigned long long)st * p->block_size;
    bytes = (unsigned long long)(end-st) * p->block_size;
    if (offset >= 1ULL << 32 || offset + bytes - 1 >= 1ULL << 32) {
        message("error: Your block range requires 64bit I/O support.");
        return (0);
    }
    return (1);
}

int check_break(void) {
    if (SIGBREAKF_CTRL_C & SetSignal(0, SIGBREAKF_CTRL_C)) {
        return (1);
    }
    return (0);
}
