#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "micro-bunzip.h"

/**
 * Seek the bunzip_data `bz` to a specific position in bits `pos` by lseeking
 * the underlying file descriptor and priming the buffer with appropriate
 * bits already consuming. This probably only makes sense for seeking to the
 * start of a compressed block.
 */
static unsigned int seek_bits( bunzip_data *bd, unsigned long pos )
{
    off_t n_byte = pos / 8;
    char n_bit = pos % 8;

    // Seek the underlying file descriptor
    lseek( bd->in_fd, n_byte, SEEK_SET );

    // Init the buffer at the right bit position
    bd->inbufBitCount = bd->inbufPos = bd->inbufCount = 0;
    get_bits( bd, n_bit );

    // Update the bit position counter to match
    bd->inPosBits = pos;
}

/**
 * Attempt to uncompress exactly one block of data from the bunzip_data `bz`
 * to the output buffer `outbuf`. The bunzip_data should be primed at the
 * start of a block (see `seek_bits`), and outbuf should be large enough
 * to accomodate `bd->dbufSize` bytes (the block size of the bzip file).
 */
int uncompress_block_to_buffer( bunzip_data *bd, char* outbuf )
{
    write_bunzip_data( bd, -1, outbuf, bd->dbufSize );
}
