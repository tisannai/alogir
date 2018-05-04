/*
   xxHash - Extremely Fast Hash algorithm
   Header File
   Copyright (C) 2012-2016, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - xxHash source repository : https://github.com/Cyan4973/xxHash
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


#ifndef AG_HASH
#define AG_HASH

#include <stddef.h>
#include <stdint.h>
#include <alogir.h>

#define AG_HASH_PUBLIC_API

/** @cond ag_hash_no_doxygen */

#define AG_HASH_VERSION_MAJOR 0
#define AG_HASH_VERSION_MINOR 6
#define AG_HASH_VERSION_RELEASE 5
#define AG_HASH_VERSION_NUMBER \
    ( AG_HASH_VERSION_MAJOR * 100 * 100 + AG_HASH_VERSION_MINOR * 100 + AG_HASH_VERSION_RELEASE )
AG_HASH_PUBLIC_API unsigned aghs_version_number( void );

/** @endcond ag_hash_no_doxygen */


/**
 * Calculate 64-bit hash value from input.
 * 
 * @param input  Input data.
 * @param length Input data length (in bytes).
 * 
 * @return 64-bit hash.
 */
AG_HASH_PUBLIC_API ag_hash_t aghs_64( const void* input, size_t length );


/**
 * Calculate 654-bit hash value from input using seed.
 * 
 * @param input  Input data.
 * @param length Input data length (in bytes).
 * @param seed   Hash seed.
 * 
 * @return 64-bit hash.
 */
AG_HASH_PUBLIC_API ag_hash_t aghs_64_with_seed( const void* input, size_t length, ag_hash_t seed );


#endif
