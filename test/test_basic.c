#include "unity.h"

#include <postor.h>
#include "ag_heap.h"


/* ------------------------------------------------------------
 * Heap tests:
 */

int rand_within( int limit )
{
    if ( limit > 0 )
        return ( rand() % limit );
    else
        return 0;
}


void aghp_print( po_t po )
{
    int  num;

    for ( po_size_t i = 0; i < po->used; i++ ) {
        num = *( (int*) po->data[ i ] );
        printf( "  %-4ld %d\n", i, num );
    }
}

int aghp_test_cmp( const po_d a, const po_d b )
{
    int ai;
    int bi;

    ai = *( (int*) a );
    bi = *( (int*) b );

    if ( ai > bi ) {
        return 1;
    } else if ( bi > ai ) {
        return -1;
    } else {
        return 0;
    }
}


void test_basic( void )
{
    po_t po;
    int lim;
    int items[ 128 ];

    srand( 1234 );

    for ( int i = 0; i < 128; i++ ) {
        items[ i ] = rand_within( 128 );
    }

    lim = 16;
    po = po_new_sized( NULL, lim );
    
    for ( int i = 0; i < lim; i++ ) {
        po_push( po, &items[ i ] );
    }

    aghp_t h;

    h = aghp_new( po, aghp_test_cmp, 1 );

    /* Sort to ascending order. */
    aghp_ify_for_sort( h );
    aghp_sort( h );

    int prev = -1;
    int cur;

    for ( int i = 0; i < lim; i++ ) {
        cur = *( po_item( h->po, i, int* ) );
        TEST_ASSERT_TRUE( prev <= cur ); 
    }

    h = aghp_del( h );
    
    /* Sort to descending order. */
    aghp_sort_postor( po, aghp_test_cmp, -1 );

    prev = INT_MAX;
    for ( int i = 0; i < lim; i++ ) {
        cur = *( po_item( po, i, int* ) );
        TEST_ASSERT_TRUE( prev >= cur ); 
    }
}


void test_limits( void )
{
    po_t po;
    int lim;
    int items[ 128 ];

    srand( 1234 );

    for ( int i = 0; i < 128; i++ ) {
        items[ i ] = rand_within( 128 );
    }

    lim = 16;
    po = po_new_sized( NULL, lim );
    
    for ( int i = 0; i < lim; i++ ) {
        po_push( po, &items[ i ] );
    }

    aghp_t h;
    h = aghp_new( po, aghp_test_cmp, -1 );
    
    TEST_ASSERT_TRUE( aghp_get_polar( h ) == -1 );
    aghp_set_polar( h, 1 );
    TEST_ASSERT_TRUE( aghp_get_polar( h ) == 1 );

    for ( int i = 0; i < lim; i++ ) {
        aghp_put( h, &items[ i ] );
    }

    for ( int i = 0; i < lim; i++ ) {
        aghp_put( h, &items[ i ] );
    }

    for ( int i = 0; i < 2*lim; i++ ) {
        aghp_get( h );
    }

    TEST_ASSERT_TRUE( aghp_is_empty( h ) );
    aghp_get( h );
    TEST_ASSERT_TRUE( aghp_is_empty( h ) );
}
