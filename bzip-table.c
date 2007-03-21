#include <stdio.h>
#include <stdlib.h>

#include "micro-bunzip.h"

/**
 * Read a bzip2 file from stdin and print
 * 1) The block size
 * 2) The starting offset (in BITS) of each block of compressed data
 *
 * This does not completely uncompress the data, so does not do CRC checks,
 * (gaining 60% or so speedup), bzip2 --test can be used to verify files
 * first if desired.
 */
int main( int argc, char*argv[] )
{
    bunzip_data *bd;
    int i;
    unsigned long position_before;
    char * c;

    /* Attempt to open the bzip2 file, if successfull this consumes the
     * entire header and moves us to the start of the first block.
     */
    if ( ! ( i = start_bunzip( &bd, 0, 0, 0 ) ) )
    {
        /* Print the block size */
        fprintf( stdout, "Block size: %d\n", bd->dbufSize );

        for ( ; ; )
        {
            /* Save the start of the block before reading it */
            position_before = bd->inPosBits;

            /* Read one block */
            int i = read_bunzip_data( bd );

            /* Non-zero return value indicates an error, break out */
            if ( i ) break;

            /* Print the position of the first bit in the block header */
            fprintf( stdout, "Block start at: %u\n", position_before );
        }
    }

    /* Free decompression buffer and bzip_data */
    if ( bd->dbuf ) free( bd->dbuf );
    free( bd );

    /* Print error if required */
    if ( i )
    {
        fprintf( stderr, "\n%s\n", bunzip_errors[-i] );
    }

    return i;
}