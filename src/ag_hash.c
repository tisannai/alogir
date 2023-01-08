/*
 *  xxHash - Fast Hash algorithm
 *  Copyright (C) 2012-2016, Yann Collet
 *
 *  BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You can contact the author at :
 *  - xxHash homepage: http://www.xxhash.com
 *  - xxHash source repository : https://github.com/Cyan4973/xxHash
 */

/*
 * MODIFICATIONS by Tero Isannainen, 2018:
 *
 *     * Reformatted code with clang-format.
 *     * Changed naming of functions.
 *     * Support for user's memory functions.
 *     * Removed support for many compilers and architectures.
 *     * Targeted for x86_64 Linux use (GCC or CLANG).
 */

#include <assert.h>
#include <string.h>

#include "ag_hash.h"


#define FORCE_INLINE static

#define AG_HASH_GCC_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ )

#define aghs_rotl32( x, r ) ( ( x << r ) | ( x >> ( 32 - r ) ) )
#define aghs_rotl64( x, r ) ( ( x << r ) | ( x >> ( 64 - r ) ) )


typedef enum
{
    aghs_big_endian = 0,
    aghs_little_endian = 1
} aghs_endianess;
typedef enum
{
    aghs_aligned,
    aghs_unaligned
} aghs_alignment;



AG_HASH_PUBLIC_API unsigned aghs_version_number( void )
{
    return AG_HASH_VERSION_NUMBER;
}



/* ------------------------------------------------------------
 * 32-bit section:
 */

static uint32_t aghs_read32( const void* memPtr )
{
    uint32_t val;
    memcpy( &val, memPtr, sizeof( val ) );
    return val;
}

static uint32_t aghs_swap32( uint32_t x )
{
    return ( ( x << 24 ) & 0xff000000 ) | ( ( x << 8 ) & 0x00ff0000 ) |
           ( ( x >> 8 ) & 0x0000ff00 ) | ( ( x >> 24 ) & 0x000000ff );
}


FORCE_INLINE uint32_t aghs_read_le32_align( const void*    ptr,
                                            aghs_endianess endian,
                                            aghs_alignment align )
{
    if ( align == aghs_unaligned )
        return endian == aghs_little_endian ? aghs_read32( ptr )
                                            : aghs_swap32( aghs_read32( ptr ) );
    else
        return endian == aghs_little_endian ? *(const uint32_t*)ptr
                                            : aghs_swap32( *(const uint32_t*)ptr );
}


// static const uint32_t PRIME32_1 = 2654435761U;
// static const uint32_t PRIME32_2 = 2246822519U;
// static const uint32_t PRIME32_3 = 3266489917U;
// static const uint32_t PRIME32_4 = 668265263U;
// static const uint32_t PRIME32_5 = 374761393U;

// static uint32_t aghs_32_avalanche( uint32_t h32 )
//{
//     h32 ^= h32 >> 15;
//     h32 *= PRIME32_2;
//     h32 ^= h32 >> 13;
//     h32 *= PRIME32_3;
//     h32 ^= h32 >> 16;
//     return ( h32 );
// }

#define aghs_get32bits( p ) aghs_read_le32_align( p, endian, align )




/* ------------------------------------------------------------
 * 64-bit section:
 */


static ag_hash_t aghs_read64( const void* memPtr )
{
    ag_hash_t val;
    memcpy( &val, memPtr, sizeof( val ) );
    return val;
}

static ag_hash_t aghs_swap64( ag_hash_t x )
{
    return ( ( x << 56 ) & 0xff00000000000000ULL ) | ( ( x << 40 ) & 0x00ff000000000000ULL ) |
           ( ( x << 24 ) & 0x0000ff0000000000ULL ) | ( ( x << 8 ) & 0x000000ff00000000ULL ) |
           ( ( x >> 8 ) & 0x00000000ff000000ULL ) | ( ( x >> 24 ) & 0x0000000000ff0000ULL ) |
           ( ( x >> 40 ) & 0x000000000000ff00ULL ) | ( ( x >> 56 ) & 0x00000000000000ffULL );
}


FORCE_INLINE ag_hash_t aghs_read_le64_align( const void*    ptr,
                                             aghs_endianess endian,
                                             aghs_alignment align )
{
    if ( align == aghs_unaligned )
        return endian == aghs_little_endian ? aghs_read64( ptr )
                                            : aghs_swap64( aghs_read64( ptr ) );
    else
        return endian == aghs_little_endian ? *(const ag_hash_t*)ptr
                                            : aghs_swap64( *(const ag_hash_t*)ptr );
}

static const ag_hash_t PRIME64_1 = 11400714785074694791ULL;
static const ag_hash_t PRIME64_2 = 14029467366897019727ULL;
static const ag_hash_t PRIME64_3 = 1609587929392839161ULL;
static const ag_hash_t PRIME64_4 = 9650029242287828579ULL;
static const ag_hash_t PRIME64_5 = 2870177450012600261ULL;

static ag_hash_t aghs_64_round( ag_hash_t acc, ag_hash_t input )
{
    acc += input * PRIME64_2;
    acc = aghs_rotl64( acc, 31 );
    acc *= PRIME64_1;
    return acc;
}

static ag_hash_t aghs_64_merge_round( ag_hash_t acc, ag_hash_t val )
{
    val = aghs_64_round( 0, val );
    acc ^= val;
    acc = acc * PRIME64_1 + PRIME64_4;
    return acc;
}

static ag_hash_t aghs_64_avalanche( ag_hash_t h64 )
{
    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;
    return h64;
}


#define aghs_get64bits( p ) aghs_read_le64_align( p, endian, align )

static ag_hash_t aghs_64_finalize( ag_hash_t      h64,
                                   const void*    ptr,
                                   size_t         len,
                                   aghs_endianess endian,
                                   aghs_alignment align )
{
    const uint8_t* p = (const uint8_t*)ptr;

#define PROCESS1_64            \
    h64 ^= ( *p ) * PRIME64_5; \
    p++;                       \
    h64 = aghs_rotl64( h64, 11 ) * PRIME64_1;

#define PROCESS4_64                                        \
    h64 ^= (ag_hash_t)( aghs_get32bits( p ) ) * PRIME64_1; \
    p += 4;                                                \
    h64 = aghs_rotl64( h64, 23 ) * PRIME64_2 + PRIME64_3;

#define PROCESS8_64                                                   \
    {                                                                 \
        ag_hash_t const k1 = aghs_64_round( 0, aghs_get64bits( p ) ); \
        p += 8;                                                       \
        h64 ^= k1;                                                    \
        h64 = aghs_rotl64( h64, 27 ) * PRIME64_1 + PRIME64_4;         \
    }

    switch ( len & 31 ) {
        case 24:
            PROCESS8_64;
        /* fallthrough */
        case 16:
            PROCESS8_64;
        /* fallthrough */
        case 8:
            PROCESS8_64;
            return aghs_64_avalanche( h64 );

        case 28:
            PROCESS8_64;
        /* fallthrough */
        case 20:
            PROCESS8_64;
        /* fallthrough */
        case 12:
            PROCESS8_64;
        /* fallthrough */
        case 4:
            PROCESS4_64;
            return aghs_64_avalanche( h64 );

        case 25:
            PROCESS8_64;
        /* fallthrough */
        case 17:
            PROCESS8_64;
        /* fallthrough */
        case 9:
            PROCESS8_64;
            PROCESS1_64;
            return aghs_64_avalanche( h64 );

        case 29:
            PROCESS8_64;
        /* fallthrough */
        case 21:
            PROCESS8_64;
        /* fallthrough */
        case 13:
            PROCESS8_64;
        /* fallthrough */
        case 5:
            PROCESS4_64;
            PROCESS1_64;
            return aghs_64_avalanche( h64 );

        case 26:
            PROCESS8_64;
        /* fallthrough */
        case 18:
            PROCESS8_64;
        /* fallthrough */
        case 10:
            PROCESS8_64;
            PROCESS1_64;
            PROCESS1_64;
            return aghs_64_avalanche( h64 );

        case 30:
            PROCESS8_64;
        /* fallthrough */
        case 22:
            PROCESS8_64;
        /* fallthrough */
        case 14:
            PROCESS8_64;
        /* fallthrough */
        case 6:
            PROCESS4_64;
            PROCESS1_64;
            PROCESS1_64;
            return aghs_64_avalanche( h64 );

        case 27:
            PROCESS8_64;
        /* fallthrough */
        case 19:
            PROCESS8_64;
        /* fallthrough */
        case 11:
            PROCESS8_64;
            PROCESS1_64;
            PROCESS1_64;
            PROCESS1_64;
            return aghs_64_avalanche( h64 );

        case 31:
            PROCESS8_64;
        /* fallthrough */
        case 23:
            PROCESS8_64;
        /* fallthrough */
        case 15:
            PROCESS8_64;
        /* fallthrough */
        case 7:
            PROCESS4_64;
        /* fallthrough */
        case 3:
            PROCESS1_64;
        /* fallthrough */
        case 2:
            PROCESS1_64;
        /* fallthrough */
        case 1:
            PROCESS1_64;
        /* fallthrough */
        case 0:
            return aghs_64_avalanche( h64 );
    }

    /* impossible to reach */
    assert( 0 );
    return 0; /* unreachable, but some compilers complain without it */
}


FORCE_INLINE ag_hash_t aghs_64_endian_align( const void*    input,
                                             size_t         len,
                                             ag_hash_t      seed,
                                             aghs_endianess endian,
                                             aghs_alignment align )
{
    const uint8_t* p = (const uint8_t*)input;
    const uint8_t* bEnd = p + len;
    ag_hash_t      h64;

    if ( p == NULL ) {
        len = 0;
        bEnd = p = (const uint8_t*)(size_t)32;
    }

    if ( len >= 32 ) {
        const uint8_t* const limit = bEnd - 32;
        ag_hash_t            v1 = seed + PRIME64_1 + PRIME64_2;
        ag_hash_t            v2 = seed + PRIME64_2;
        ag_hash_t            v3 = seed + 0;
        ag_hash_t            v4 = seed - PRIME64_1;

        do {
            v1 = aghs_64_round( v1, aghs_get64bits( p ) );
            p += 8;
            v2 = aghs_64_round( v2, aghs_get64bits( p ) );
            p += 8;
            v3 = aghs_64_round( v3, aghs_get64bits( p ) );
            p += 8;
            v4 = aghs_64_round( v4, aghs_get64bits( p ) );
            p += 8;
        } while ( p <= limit );

        h64 = aghs_rotl64( v1, 1 ) + aghs_rotl64( v2, 7 ) + aghs_rotl64( v3, 12 ) +
              aghs_rotl64( v4, 18 );
        h64 = aghs_64_merge_round( h64, v1 );
        h64 = aghs_64_merge_round( h64, v2 );
        h64 = aghs_64_merge_round( h64, v3 );
        h64 = aghs_64_merge_round( h64, v4 );

    } else {
        h64 = seed + PRIME64_5;
    }

    h64 += (ag_hash_t)len;

    return aghs_64_finalize( h64, p, len, endian, align );
}


ag_hash_t aghs_64_with_seed( const void* input, size_t len, ag_hash_t seed )
{
    return aghs_64_endian_align( input, len, seed, aghs_little_endian, aghs_unaligned );
}


ag_hash_t aghs_64( const void* input, size_t len )
{
    return aghs_64_endian_align( input, len, 0, aghs_little_endian, aghs_unaligned );
}
