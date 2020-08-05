/* ========================================================================== **
 *                                 ubi_Cache.c
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
 * $Id: ubi_Cache.c; 2020-08-05 16:43:13 -0500; Christopher R. Hertel$
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
 * Revision 0.0  1997/12/18 06:24:33  crh
 * Initial Revision.
 *
 * ========================================================================== **
 */

#include "ubi_Cache.h"    /* Header for *this* module. */

/* -------------------------------------------------------------------------- **
 * Static data...
 */

static char ModuleID[] =
  "$Id: ubi_Cache.c; 2020-08-05 16:43:13 -0500; Christopher R. Hertel$\n";

/* -------------------------------------------------------------------------- **
 * Internal functions...
 */

static void free_entry( ubi_cacheRootPtr CachePtr, ubi_cacheEntryPtr EntryPtr )
  /* ------------------------------------------------------------------------ **
   * Free a ubi_cacheEntry, and adjust the mem_used counter accordingly.
   *
   *  Input:  CachePtr  - A pointer to the cache from which the entry has
   *                      been removed.
   *          EntryPtr  - A pointer to the already removed entry.
   *
   *  Output: none.
   *
   *  Notes:  Remove the entry from the cache before calling this function.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  CachePtr->mem_used -= EntryPtr->entry_size;
  (*CachePtr->free_func)( (void *)EntryPtr );
  } /* free_entry */

static void cachetrim( ubi_cacheRootPtr crptr )
  /* ------------------------------------------------------------------------ **
   * Remove entries from the cache until the number of entries and the amount
   * of memory used are both at or below the maximum.
   *
   *  Input:  crptr - pointer to the cache to be trimmed.
   *
   *  Output: None.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  while( ( crptr->max_entries && (crptr->max_entries < crptr->root.count) )
      || ( crptr->max_memory  && (crptr->max_memory  < crptr->mem_used)   ) )
    {
    if( !ubi_cacheReduce( crptr, 1 ) )
      return;
    }
  } /* cachetrim */


/* -------------------------------------------------------------------------- **
 * Exported functions...
 */

ubi_cacheRootPtr ubi_cacheInit( ubi_cacheRootPtr  CachePtr,
                                ubi_trCompFunc    CompFunc,
                                ubi_trKillNodeRtn FreeFunc,
                                unsigned long     MaxEntries,
                                unsigned long     MaxMemory )
  /** Initialize a cache header structure.
   *
   * @param   CachePtr    A pointer to a \c #ubi_cacheRoot structure that
   *                      is to be initialized.
   * @param   CompFunc    A pointer to the function that will be called
   *                      to compare two cache values.
   * @param   FreeFunc    A pointer to a function that will be called
   *                      to free a cache entry.  If you allocated
   *                      the cache entry using malloc(), then this
   *                      will likely be free().  If you are allocating
   *                      cache entries from a free list, then this will
   *                      likely be a function that returns memory to the
   *                      free list, etc.
   * @param   MaxEntries  The maximum number of entries that will be
   *                      allowed to exist in the cache.  If this limit
   *                      is exceeded, then existing entries will be
   *                      removed from the cache.  A value of zero
   *                      indicates that there is no limit on the number
   *                      of cache entries.  See \c #ubi_cachePut().
   * @param   MaxMemory   The maximum amount of memory, in bytes, to be
   *                      allocated to the cache (excluding the cache
   *                      header).  If this is exceeded, existing entries
   *                      in the cache will be removed until enough memory
   *                      has been freed to meet the condition.  See
   *                      \c #ubi_cachePut().
   *
   * @returns   A pointer to the initialized cache (i.e., the same as
   *            \p CachePtr).
   *
   * \b Notes
   *  - Both MaxEntries and MaxMemory may be changed after the cache has
   *    been created.  See:
   *    - \c #ubi_cacheSetMaxEntries()
   *    - \c #ubi_cacheSetMaxMemory()
   *    - \c #ubi_cacheGetMaxEntries()
   *    - \c #ubi_cacheGetMaxMemory()
   *  - Memory is typically allocated in multiples of the system's word size.
   *    The return value of \c strlen() (for example) does not reflect this;
   *    it will allways be less than or equal to the amount of memory
   *    actually allocated.  Keep this in mind when choosing a value for
   *    MaxMemory.
   */
  {
  if( CachePtr )
    {
    (void)ubi_trInitTree( CachePtr, CompFunc, ubi_trOVERWRITE );
    CachePtr->free_func   = FreeFunc;
    CachePtr->max_entries = MaxEntries;
    CachePtr->max_memory  = MaxMemory;
    CachePtr->mem_used    = 0;
    CachePtr->cache_hits  = 0;
    CachePtr->cache_trys  = 0;
    }
  return( CachePtr );
  } /* ubi_cacheInit */

ubi_cacheRootPtr ubi_cacheClear( ubi_cacheRootPtr CachePtr )
  /** Remove and free all entries in an existing cache.
   *
   * @param   CachePtr  A pointer to the cache that is to be cleared.
   *
   * @returns A pointer to the cache header (i.e., the same as
   *          \p CachePtr).
   *
   * \b Note: This function re-initializes the cache header.
   */
  {
  if( CachePtr )
    {
    (void)ubi_trKillTree( CachePtr, CachePtr->free_func );
    CachePtr->mem_used    = 0;
    CachePtr->cache_hits  = 0;
    CachePtr->cache_trys  = 0;
    }
  return( CachePtr );
  } /* ubi_cacheClear */

void ubi_cachePut( ubi_cacheRootPtr  CachePtr,
                   unsigned long     EntrySize,
                   ubi_cacheEntryPtr EntryPtr,
                   ubi_trItemPtr     Key )
  /** Add an entry to the cache.
   *
   * @param   CachePtr  A pointer to the cache into which the entry
   *                    will be added.
   * @param   EntrySize The size, in bytes, of the memory block indicated
   *                    by EntryPtr.  This will be copied into the
   *                    \c entry_size field of the cache entry.
   * @param   EntryPtr  A pointer to a memory block that begins with a
   *                    \c #ubi_cacheEntry structure.  The entry structure
   *                    should be followed immediately by the data to be
   *                    cached (or a pointer to the cached data).
   * @param   Key       Pointer used to indicate the lookup key within the
   *                    Entry.
   *
   * @returns None.
   *
   * \b Notes:
   *  - After adding the new node, the cache is "trimmed".  This removes
   *    extra nodes if the tree has exceeded it's memory or entry count
   *    limits.  It is unlikely (though not entirely impossible) that the
   *    newly added node will be purged from the cache (assuming a
   *    reasonably large cache), since new nodes in a splay tree (which is
   *    what this module was designed to use) are moved to the top of the
   *    tree and the cache purge process removes nodes from the bottom of
   *    the tree.
   *  - The underlying splay tree is opened in OVERWRITE mode.  If the input
   *    key matches an existing key, the existing entry will be politely
   *    removed from the tree and freed.
   */
  {
  ubi_trNodePtr OldNode;

  EntryPtr->entry_size = EntrySize;
  CachePtr->mem_used  += EntrySize;
  (void)ubi_trInsert( CachePtr, EntryPtr, Key, &OldNode );
  if( OldNode )
    free_entry( CachePtr, (ubi_cacheEntryPtr)OldNode );

  cachetrim( CachePtr );
  } /* ubi_cachePut */

ubi_cacheEntryPtr ubi_cacheGet( ubi_cacheRootPtr CachePtr,
                                ubi_trItemPtr    FindMe )
  /** Attempt to retrieve an entry from the cache.
   *
   * @param   CachePtr  A ponter to the cache that is to be searched.
   * @param   FindMe    A #ubi_trItemPtr that indicates the key for which
   *                    to search.
   *
   * @returns A pointer to the cache entry that was found, or NULL if no
   *          matching entry was found.
   *
   * \b Notes:
   *  - This function also updates the hit ratio counters.
   *    - The counters are unsigned short.  If the number of cache tries
   *      reaches 65534 (0xFFFE), then both the number of tries and the
   *      number of hits are divided by two.  This prevents the counters
   *      from overflowing.  See the comments in #ubi_cacheHitRatio() for
   *      additional notes.
   */
  {
  ubi_trNodePtr FoundPtr;

  FoundPtr = ubi_trFind( CachePtr, FindMe );

  if( FoundPtr )
    CachePtr->cache_hits++;
  CachePtr->cache_trys++;

  if( CachePtr->cache_trys >= 0xFFFE )
    {
    CachePtr->cache_hits >>= 1;
    CachePtr->cache_trys >>= 1;
    }

  return( (ubi_cacheEntryPtr)FoundPtr );
  } /* ubi_cacheGet */

ubi_trBool ubi_cacheDelete( ubi_cacheRootPtr CachePtr, ubi_trItemPtr DeleteMe )
  /** Find and delete the specified cache entry.
   *
   * @param   CachePtr  A pointer to the cache.
   * @param   DeleteMe  The key of the entry to be deleted.
   *
   * @returns TRUE if the entry was found & freed, else FALSE.
   */
  {
  ubi_trNodePtr FoundPtr;

  FoundPtr = ubi_trFind( CachePtr, DeleteMe );
  if( FoundPtr )
    {
    (void)ubi_trRemove( CachePtr, FoundPtr );
    free_entry( CachePtr, (ubi_cacheEntryPtr)FoundPtr );
    return( ubi_trTRUE );
    }
  return( ubi_trFALSE );
  } /* ubi_cacheDelete */

ubi_trBool ubi_cacheReduce( ubi_cacheRootPtr CachePtr, unsigned long count )
  /** Remove \p count entries from the cache.
   *
   * @param CachePtr  A pointer to the cache which is to be reduced in size.
   *                      size.
   * @param count     The number of entries to remove.
   *
   * @returns TRUE if \p count entries were removed, else FALSE.
   *          FALSE indicates that there were fewer than \p count entries
   *          in the cache, and that the cache is now empty.
   *
   * \b Notes:
   *  - This function forces a reduction in the number of cache entries
   *    without requiring that the \c MaxMemory or \c MaxEntries values be
   *    changed.
   *  - Cache entries are removed by looking at the "bottom" of the cache.
   *    That is, an attempt is made to remove the least recently used (LRU)
   *    nodes in the cache first.
   */
  {
  ubi_trNodePtr NodePtr;

  while( count )
    {
    NodePtr = ubi_trLeafNode( CachePtr->root.root );
    if( NULL == NodePtr )
      return( ubi_trFALSE );
    else
      {
      (void)ubi_trRemove( CachePtr, NodePtr );
      free_entry( CachePtr, (ubi_cacheEntryPtr)NodePtr );
      }
    count--;
    }
  return( ubi_trTRUE );
  } /* ubi_cacheReduce */

unsigned long ubi_cacheSetMaxEntries( ubi_cacheRootPtr CachePtr,
                                      unsigned long    NewSize )
  /** Change the maximum number of entries allowed to exist in the cache.
   *
   * @param   CachePtr  A pointer to the cache to be modified.
   * @param   NewSize   The new maximum number of cache entries.
   *
   * @returns The maximum number of entries previously allowed to exist in
   *          the cache.
   *
   * \b Notes:
   *  - If the new size is less than the old size, this function will trim
   *    the cache (remove excess entries, if any).
   *  - A value of zero indicates an unlimited number of entries.
   */
  {
  unsigned long oldsize = CachePtr->max_entries;      /* Save the old value.  */

  CachePtr->max_entries = NewSize;                    /* Apply the new value. */
  if( (NewSize < oldsize) || (NewSize && !oldsize) )  /* If size is smaller,  */
    cachetrim( CachePtr );                            /*   remove excess.     */
  return( oldsize );
  } /* ubi_cacheSetMaxEntries */

unsigned long ubi_cacheSetMaxMemory( ubi_cacheRootPtr CachePtr,
                                     unsigned long    NewSize )
  /** Change the maximum memory to be used for storing cache entries.
   *
   * @param   CachePtr  A pointer to the cache to be modified.
   * @param   NewSize   The new cache memory size.
   *
   * @returns The previous maximum memory size.
   *
   * \b Notes:
   *  - If the new size is less than the old size, this function will trim
   *    the cache (remove excess entries).
   *  - A value of zero (0) indicates that the cache has no memory limit.
   */
  {
  unsigned long oldsize = CachePtr->max_memory;       /* Save the old value.  */

  CachePtr->max_memory = NewSize;                     /* Apply the new value. */
  if( (NewSize < oldsize) || (NewSize && !oldsize) )  /* If size is smaller,  */
    cachetrim( CachePtr );                            /*   remove excess.     */
  return( oldsize );
  } /* ubi_cacheSetMaxMemory */

int ubi_cacheHitRatio( ubi_cacheRootPtr CachePtr )
  /** Return the cache hit ratio (times 10000).
   *
   *  Returns a value that is 10,000 times the weighted average hit ratio
   *  for the cache.
   *
   * @param   CachePtr  Pointer to the cache to be queried.
   *
   * @returns An integer equal to <tt>(10000 * hits) / tries</tt>.
   *
   * \b Notes:
   *  - The ratio is described as "weighted" because both the numerator
   *    and denominator (hits and tries) are divided by two if tries is
   *    in danger of overflowing.  Dividing both by two maintains the
   *    ratio , but also means that subsequent increments to either
   *    counter will have twice as much significance.
   *  - The value returned by this function will be in the range
   *    [0..10000] because ( 0 <= cache_hits <= cache_trys ) will always
   *    be true.
   *  - If the number of cache tries is zero (0), the ratio must be 0/0,
   *    which is interpreted as "no cache hits", and zero (0) is returned.
   */
  {
  if( CachePtr->cache_trys )
    return( (int)((10000 * (long)(CachePtr->cache_hits))
                         / (long)(CachePtr->cache_trys)) );
  return( 0 );
  } /* ubi_cacheHitRatio */

/* -------------------------------------------------------------------------- */
