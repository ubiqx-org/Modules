#ifndef UBI_CACHE_H
#define UBI_CACHE_H
/* ========================================================================== **
 *                                 ubi_Cache.h
 *
 *  Copyright (C) 1997, 2020 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *
 *  This module implements a generic cache.
 *
 * -------------------------------------------------------------------------- **
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * -------------------------------------------------------------------------- **
 *
 * $Id: ubi_Cache.h; 2020-09-23 19:59:08 -0500; crh$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 0.4  1999/09/22 03:42:24  crh
 * Fixed a minor typo.
 *
 * Revision 0.3  1998/06/03 18:00:15  crh
 * Further fiddling with sys_include.h, which is no longer explicitly
 * included by this module since it is inherited from ubi_BinTree.h.
 *
 * Revision 0.2  1998/06/02 01:36:18  crh
 * Changed include name from ubi_null.h to sys_include.h to make it
 * more generic.
 *
 * Revision 0.1  1998/05/20 04:36:02  crh
 * The C file now includes ubi_null.h.  See ubi_null.h for more info.
 *
 * Revision 0.0  1997/12/18 06:25:23  crh
 * Initial Revision.
 *
 * ========================================================================== **
 *//**
 * @file      ubi_Cache.h
 * @author    Christopher R. Hertel
 * @brief     Simple caching module based on \c ubi_SplayTree.
 * @date      18 Dec 1997
 * @version   \$Id: ubi_Cache.h; 2020-09-23 19:59:08 -0500; crh$
 * @copyright Copyright (C) 1997, 2020 by Christopher R. Hertel
 *
 * @details
 *  This module uses ubi_SplayTree module to implement a simple cache with
 *  the following features:
 *  - The cache may be limited in size by the number of entries and/or the
 *    amount of memory used.  When either limit is exceeded, cache entries
 *    are removed until the cache is again within the given limits.
 *  - Some rough statistical information is kept so that an approximate
 *    "hit ratio" can be calculated.
 *  - There are several functions available that provide access to, and
 *    management of cache size limits, hit ratio, and cache trimming.
 *
 *  The splay tree is used because recently accessed items tend toward the
 *  top of the tree and less recently accessed items tend toward the bottom.
 *  This makes it easy to purge less recently used items should the cache
 *  exceed its limits.
 *
 *  To use this module, you will need to supply a comparison function of
 *  type \c #ubi_trCompFunc and a node-freeing function of type
 *  \c #ubi_trKillNodeRtn.  See ubi_BinTree.h for more information on
 *  these.  (This is all basic ubiqx tree management stuff.)
 *
 * \b Notes
 *  - Cache performance will start to suffer dramatically if the cache
 *    becomes large enough to force the OS to start swapping memory to
 *    disk.  This is because the nodes of the underlying tree will be
 *    scattered across memory in an order that is completely unrelated
 *    to their traversal order.  As more and more of the cache is placed
 *    into swap space, more and more swaps will be required for a simple
 *    lookup or traversal operation.
 *
 *  - Each entry keeps track of the amount of memory it used and the
 *    cache header keeps the total.  This information is provided via
 *    the \c EntrySize parameter in \c #ubi_cachePut(), so it is up to
 *    the caller to make sure that the numbers are accurate.  (The numbers
 *    don't even have to represent bytes used.)
 *
 *    As you consider this, note that the strdup() function--as an
 *    example--will call malloc().  The latter generally allocates a
 *    multiple of the system word size, which may be more than the
 *    number of bytes needed to store the string.
 *
 */

#include "ubi_SplayTree.h"

/* -------------------------------------------------------------------------- **
 * Typedefs...
 */

/**
 * @struct  ubi_cacheRoot
 * @brief   Cache header structure.
 * @details The cache header consists of a binary tree root plus some added
 *          housekeeping fields.
 */
typedef struct
  {
  ubi_trRoot        root;         /**< Splay tree control structure.      */
  ubi_trKillNodeRtn free_func;    /**< Function used to free entries.     */
  unsigned long     max_entries;  /**< Max cache entries.  0 == unlimited */
  unsigned long     max_memory;   /**< Max memory to use.  0 == unlimited */
  unsigned long     mem_used;     /**< Memory currently in use (bytes).   */
  unsigned short    cache_hits;   /**< Incremented on succesful find.     */
  unsigned short    cache_trys;   /**< Incremented on any cache lookup.   */
  } ubi_cacheRoot;

/** A cache pointer; points to a \c #ubi_cacheRoot structure. */
typedef ubi_cacheRoot *ubi_cacheRootPtr;

/**
 * @struct    ubi_cacheEntry
 * @brief     Cache entry node structure.
 * @details   A cache entry consists of a tree node structure and the size
 *            (in bytes) of the entry data.  The entry size is supplied via
 *            the \p EntrySize parameter of the #ubi_cachePut() function.
 */
typedef struct
  {
  ubi_trNode    node;           /**< Tree node structure.   */
  unsigned long entry_size;     /**< Entry size, in bytes.  */
  } ubi_cacheEntry;

/** Pointer to a ubi_cacheEntry. */
typedef ubi_cacheEntry *ubi_cacheEntryPtr;


/* -------------------------------------------------------------------------- **
 * Macros...
 */

/**
 * @def     ubi_cacheGetMaxEntries( Cptr )
 * @param   Cptr  Pointer to the cache root.
 * @brief   Return the current maximum entries limit for the cache.
 * @returns The current maximum number of entries allowed in the cache.
 * @details Reports the current value of the \c max_entries field in the
 *          cache header.  Zero indicates no maximum.
 */
#define ubi_cacheGetMaxEntries( Cptr ) (((ubi_cacheRootPtr)(Cptr))->max_entries)

/**
 * @def     ubi_cacheGetMaxMemory( Cptr )
 * @param   Cptr  Pointer to the cache root.
 * @brief   Return the current maximum memory limit for the cache.
 * @returns The current maximum memory that may be used by the cache.
 * @details Reports the current value of the \c max_memory field in the
 *          cache header.  Zero indicates no maximum.
 */
#define ubi_cacheGetMaxMemory( Cptr ) (((ubi_cacheRootPtr)(Cptr))->max_memory)

/**
 * @def     ubi_cacheGetEntryCount( Cptr )
 * @param   Cptr  Pointer to the cache root.
 * @returns The number of entries currently in the cache.
 */
#define ubi_cacheGetEntryCount( Cptr ) (((ubi_cacheRootPtr)(Cptr))->root.count)

/**
 * @def     ubi_cacheGetMemUsed( Cptr )
 * @param   Cptr  Pointer to the cache root.
 * @returns The amount of memory currently in use by the cache.
 */
#define ubi_cacheGetMemUsed( Cptr ) (((ubi_cacheRootPtr)(Cptr))->mem_used)

/* -------------------------------------------------------------------------- **
 * Prototypes...
 */

ubi_cacheRootPtr ubi_cacheInit( ubi_cacheRootPtr  CachePtr,
                                ubi_trCompFunc    CompFunc,
                                ubi_trKillNodeRtn FreeFunc,
                                unsigned long     MaxEntries,
                                unsigned long     MaxMemory );

ubi_cacheRootPtr ubi_cacheClear( ubi_cacheRootPtr CachePtr );

void ubi_cachePut( ubi_cacheRootPtr  CachePtr,
                   unsigned long     EntrySize,
                   ubi_cacheEntryPtr EntryPtr,
                   ubi_trItemPtr     Key );

ubi_cacheEntryPtr ubi_cacheGet( ubi_cacheRootPtr CachePtr,
                                ubi_trItemPtr    FindMe );

ubi_trBool ubi_cacheDelete( ubi_cacheRootPtr CachePtr, ubi_trItemPtr DeleteMe );

ubi_trBool ubi_cacheReduce( ubi_cacheRootPtr CachePtr, unsigned long count );

unsigned long ubi_cacheSetMaxEntries( ubi_cacheRootPtr CachePtr,
                                      unsigned long    NewSize );

unsigned long ubi_cacheSetMaxMemory( ubi_cacheRootPtr CachePtr,
                                     unsigned long    NewSize );

int ubi_cacheHitRatio( ubi_cacheRootPtr CachePtr );

/* ========================================================================== */
#endif /* ubi_CACHE_H */
