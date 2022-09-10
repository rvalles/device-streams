#include <devices/hardblocks.h>
struct device {
    struct Node node;
    struct List units;
    char *name; /* name of exec device. */
};
/* structure that holds all info on this paticular unit for an exec device. */
typedef struct unit_s {
    struct Node node;
    struct List parts;
    struct RigidDiskBlock *rdb;
    char *name; /* just a pointer to the */
    /* lists data. */
    ulong rdb_at;           /* what block the rdb is at. */
    ulong unit;             /* unit number of drive. */
    ulong flags;            /* unit number of drive. */
    ulong total_blocks;     /* total blocks on drive. */
    ulong cylinders;        /* number of cylinders. */
    ulong heads;            /* number of heads. */
    ulong blocks_per_track; /* number of blocks per head */
    /* per cylinder. */
    ulong bytes_per_block; /* number of bytes per block. */
} Unit;
typedef struct partition_s {
    struct Node node;
    Unit *unit;               /* back pointer. */
    struct PartitionBlock pb; /* partition block. */
    char *name;               /* name of the partition. */
    ulong start_block;        /* block that partition */
    /* starts on. */
    ulong end_block; /* block that partition ends */
    /* on. */
    ulong total_blocks; /* total number of blocks for */
    /* this partition (e-s+1) */
    ulong block_size; /* size of blocks for this partition. */
} Partition;
struct device_data {
    struct MsgPort *port;
    struct IORequest *io;
    char *name;
    ulong unit;
    ulong flags;
    int open;
};
/* START: "getdevices.c" */
struct List *get_drive_list(void);
void free_drive_list(struct List *l);
int add_name_to_drive_list(struct List *l, char *dev_name);
char *get_hard_drive_device_name(struct DosList *dl);
ulong checksum(ulong sl, ulong *buf);
void do_unit(struct device *dev, struct device_data *dd);
void free_unit(Unit *u);
void get_partitions(struct device_data *dd, Unit *u);
void free_partition(Partition *p);
Partition *find_partition(struct List *dl, char *dev_name, char *part_name, ulong unit, ulong start_block, ulong end_block);
/* END: "getdevices.c" */
/* START: "devices.c" */
struct device_data *alloc_device(char *name, ulong unit, ulong flags, ulong iosize);
void free_device(struct device_data *dd);
int open_device(struct device_data *dd);
void close_device(struct device_data *dd);
ulong device_read(struct device_data *dd, ulong offset, ulong bytes, void *buffer);
ulong device_write(struct device_data *dd, ulong offset, ulong bytes, void *buffer);
int device_do_command(struct device_data *dd, UWORD command);
/* END: "devices.c" */
/* START: "util.c" */
int string_to_number(char *s, unsigned long *num);
char *stripws(char *s);
char *fgetline(FILE *fp);
int flush_to_eol(FILE *fp);
char *concat_strings(const char *before, const char *after);
void free_string(char *string);
char *alloc_string(char *s);
int ask_bool(int def, int other, char *f, ...);
void *zmalloc(size_t b);
void zfree(void *mem);
struct Node *find_name(struct List *l, char *s);
void verbose_message(char *f, ...);
void debug_message(char *f, ...);
void verbose_debug_message(char *f, ...);
void message(char *f, ...);
void warn_message(char *f, ...);
void vmessage(char *f, va_list ap);
/* END: "util.c" */
/* START: "system" */
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
extern struct Library *DOSBase;
/* END: "system" */
