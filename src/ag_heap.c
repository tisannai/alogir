/**
 * @file   ag_heap.c
 * @author Tero Isannainen <tero.isannainen@gmail.com>
 * @date   Sun Apr 22 16:12:17 2018
 *
 * @brief  Heap algorithms over containers.
 */

#include "ag_heap.h"


/** First location of data in heap. */
#define AGHP_FIRST 1

/**
 * Return nth heap data. Heap data is accessed starting from 1, but
 * data is stored (as normal) starting from 0.
 */
#define aghp_nth( h, nth ) ( ( ( h )->gr->data )[ (nth)-1 ] )


static int aghp_compare( aghp_t h, const gr_d a, const gr_d b );



aghp_t aghp_new( gr_t gr, gr_compare_fn_p cmp, gr_pos_t dir )
{
    aghp_t h;
    h = gr_malloc( sizeof( aghp_s ) );
    aghp_init( h, gr, cmp, dir );
    return h;
}


void aghp_init( aghp_t h, gr_t gr, gr_compare_fn_p cmp, gr_pos_t dir )
{
    h->gr = gr;
    h->cmp = cmp;
    h->cnt = AGHP_FIRST - AGHP_FIRST;
    h->polar = dir;
}


aghp_t aghp_del( aghp_t h )
{
    gr_free( h );
    return NULL;
}


void aghp_put( aghp_t h, gr_d item )
{
    gr_size_t i;

    if ( h->cnt >= h->gr->used )
        gr_push( &( h->gr ), NULL );

    /*
     * Count is directly used as heap index, since heap indexing
     * starts at 1. Index is reverted back to array index within
     * aghp_nth().
     */
    i = ++h->cnt;

    /* Drop data downwards until proper place for new data is found. */
    while ( i > AGHP_FIRST && aghp_compare( h, aghp_nth( h, i / 2 ), item ) > 0 ) {
        aghp_nth( h, i ) = aghp_nth( h, i / 2 );
        i /= 2;
    }

    /* Add new data. */
    aghp_nth( h, i ) = item;
}


gr_d aghp_get( aghp_t h )
{
    if ( aghp_is_empty( h ) ) {

        return NULL;

    } else {

        gr_size_t i;
        gr_size_t child;

        gr_d ret;
        gr_d last;

        ret = aghp_nth( h, AGHP_FIRST );
        last = aghp_nth( h, h->cnt-- );

        i = AGHP_FIRST;

        /*
         * Copy data upwards until the heap is again in (heap)
         * order.
         */
        while ( i * 2 <= h->cnt ) {

            /* Find the smaller child of two. */
            child = i * 2;
            if ( ( child != h->cnt ) &&
                 ( aghp_compare( h, aghp_nth( h, child + 1 ), aghp_nth( h, child ) ) < 0 ) )
                child++;

            /* Percolate down. */
            if ( aghp_compare( h, last, aghp_nth( h, child ) ) > 0 )
                aghp_nth( h, i ) = aghp_nth( h, child );
            else
                break;

            i = child;
        }

        aghp_nth( h, i ) = last;

        return ret;
    }
}


void aghp_ify( aghp_t h )
{
    for ( gr_size_t i = 1; i <= h->gr->used; i++ ) {
        aghp_put( h, aghp_nth( h, i ) );
    }
}


void aghp_ify_for_sort( aghp_t h )
{
    aghp_inv_polar( h );
    aghp_ify( h );
    aghp_inv_polar( h );
}


void aghp_sort( aghp_t h )
{
    gr_size_t lim;

    lim = h->cnt;

    aghp_inv_polar( h );
    for ( gr_size_t i = 0; i < lim; i++ ) {
        aghp_nth( h, h->cnt + 1 ) = aghp_get( h );
    }
    aghp_inv_polar( h );
}


void aghp_sort_gromer( gr_t gr, gr_compare_fn_p cmp, gr_pos_t dir )
{
    aghp_s hs;
    aghp_init( &hs, gr, cmp, dir );
    aghp_ify_for_sort( &hs );
    aghp_sort( &hs );
}


int aghp_is_empty( aghp_t h )
{
    if ( h->cnt > 0 )
        return 0;
    else
        return 1;
}

void aghp_set_polar( aghp_t h, gr_pos_t polar )
{
    h->polar = polar;
}


void aghp_inv_polar( aghp_t h )
{
    h->polar *= -1;
}


gr_pos_t aghp_get_polar( aghp_t h )
{
    return h->polar;
}



/* ------------------------------------------------------------
 * Internal support:
 */

/**
 * Compare a to b and adjust the compare function result with heap
 * polar.
 *
 * @param h Heap.
 * @param a Reference data.
 * @param b Compare data.
 *
 * @return a > b => 1, a < b => -1, a == b => 0.
 */
static int aghp_compare( aghp_t h, const gr_d a, const gr_d b )
{
    return h->polar * h->cmp( a, b );
}
