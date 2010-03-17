#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "micro-bunzip.h"

#define BUF_SIZE 8192

/**
 * Seek the bunzip_data `bz` to a specific position in bits `pos` by lseeking
 * the underlying file descriptor and priming the buffer with appropriate
 * bits already consumed. This probably only makes sense for seeking to the
 * start of a compressed block.
 */
unsigned int seek_bits( bunzip_data *bd, unsigned long pos )
{
    off_t n_byte = pos / 8;
    char n_bit = pos % 8;

    // Seek the underlying file descriptor
    if ( lseek( bd->in_fd, n_byte, SEEK_SET ) != n_byte )
    {
        return -1;
    }

    // Init the buffer at the right bit position
    bd->inbufBitCount = bd->inbufPos = bd->inbufCount = 0;
    get_bits( bd, n_bit );

    // // Update the bit position counter to match
    // bd->inPosBits = pos;

    return pos;
}

/* Open, seek to block at pos, and uncompress */

int uncompressblock( int src_fd, unsigned long pos )
{
    bunzip_data *bd;
    int status;
    int gotcount;
    char outbuf[BUF_SIZE];

    if ( !( status = start_bunzip( &bd, src_fd, 0, 0 ) ) )
    {
        seek_bits( bd, pos );

        /* Fill the decode buffer for the block */
        if ( ( status = get_next_block( bd ) ) )
            goto seek_bunzip_finish;

        /* Init the CRC for writing */
        bd->writeCRC = 0xffffffffUL;

        /* Zero this so the current byte from before the seek is not written */
        bd->writeCopies = 0;

        /* Decompress the block and write to stdout */
        for ( ; ; )
        {
            gotcount = read_bunzip( bd, outbuf, BUF_SIZE );
            if ( gotcount < 0 )
            {
                status = gotcount;
                break;
            }
            else if ( gotcount == 0 )
            {
                break;
            }
            else
            {
                write( 1, outbuf, gotcount );
            }
        }
    }

seek_bunzip_finish:

    if ( bd->dbuf ) free( bd->dbuf );
    free( bd );

    return status;
}

int main( int argc, char *argv[] )
{
    unsigned long pos = atol( argv[1] );
    int status = uncompressblock( 0, pos );
    if ( status )
        fprintf( stderr, "\n%s\n", bunzip_errors[-status] );
}