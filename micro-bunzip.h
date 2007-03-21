#ifndef __MICRO_BUNZIP_H__
#define __MICRO_BUNZIP_H__

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ---- Duplicated from micro-bzip.c -------------------------------------- */

/* Constants for huffman coding */
#define MAX_GROUPS   6
#define GROUP_SIZE     50  /* 64 would have been more efficient */
#define MAX_HUFCODE_BITS  20  /* Longest huffman code allowed */
#define MAX_SYMBOLS   258  /* 256 literals + RUNA + RUNB */
#define SYMBOL_RUNA   0
#define SYMBOL_RUNB   1

/* Status return values */
#define RETVAL_OK      0
#define RETVAL_LAST_BLOCK    (-1)
#define RETVAL_NOT_BZIP_DATA   (-2)
#define RETVAL_UNEXPECTED_INPUT_EOF  (-3)
#define RETVAL_UNEXPECTED_OUTPUT_EOF (-4)
#define RETVAL_DATA_ERROR    (-5)
#define RETVAL_OUT_OF_MEMORY   (-6)
#define RETVAL_OBSOLETE_INPUT   (-7)

/* Other housekeeping constants */
#define IOBUF_SIZE   4096

// char *bunzip_errors[]={NULL,"Bad file checksum","Not bzip data",
//   "Unexpected input EOF","Unexpected output EOF","Data error",
//    "Out of memory","Obsolete (pre 0.9.5) bzip format not supported."};

extern char *bunzip_errors[];

/* This is what we know about each huffman coding group */
struct group_data
{
    int limit[MAX_HUFCODE_BITS], base[MAX_HUFCODE_BITS], permute[MAX_SYMBOLS];
    char minLen, maxLen;
};

/* Structure holding all the housekeeping data, including IO buffers and
   memory that persists between calls to bunzip */
typedef struct
{
    /* For I/O error handling */
    jmp_buf jmpbuf;
    /* Input stream, input buffer, input bit buffer */
    int in_fd, inbufCount, inbufPos;
    unsigned char *inbuf;
    unsigned int inbufBitCount, inbufBits;
    /* Output buffer */
    char outbuf[IOBUF_SIZE];
    int outbufPos;
    /* The CRC values stored in the block header and calculated from the data */
    unsigned int crc32Table[256], headerCRC, dataCRC, totalCRC;
    /* Intermediate buffer and its size (in bytes) */
    unsigned int *dbuf, dbufSize;
    /* State for interrupting output loop */
    int writePos, writeRun, writeCount, writeCurrent;

    /* These things are a bit too big to go on the stack */
    unsigned char selectors[32768];   /* nSelectors=15 bits */
    struct group_data groups[MAX_GROUPS]; /* huffman coding tables */
    /* james@bx.psu.edu: Keep track of position in in_fd in bits */
    unsigned long inPosBits;
}
bunzip_data;

/* ---- Forward declarations for micro-bzip.c ---------------------------- */

extern int read_bunzip_data( bunzip_data *bd );

#endif