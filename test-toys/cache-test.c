/* ========================================================================== **
 *                                cache-test.c
 * -------------------------------------------------------------------------- **
 * License: Public Domain
 * Description: Test the ubiqx splay-cache implementation nasal spray.
 * -------------------------------------------------------------------------- **
 * Notes:
 *
 *  $Id%
 *  Overhauled to to be more presentable.
 *
 *  Revision 0.0  1997/12/18 05:55:06  crh
 *  Initial Revision.
 *
 * ========================================================================== **
 */
#include <stdio.h>        /* Standard I/O.              */
#include <string.h>       /* String functions.          */
#include <stdlib.h>       /* Standard C library header. */
#include <errno.h>        /* Error handling.            */

#include "ubi_Cache.h"    /* Module header.             */


/* -------------------------------------------------------------------------- **
 * Typedefs...
 *
 *  Rec     - Cache node structure to hold the key/value pairs.  For this
 *            test program, simple strings are used.
 *  RecPtr  - A pointer to a Rec node.
 */

typedef struct
  {
  ubi_cacheEntry node;      /* Cache node structure.  */
  char          *key;       /* Search key.            */
  char          *content;   /* Cached data.           */
  } Rec;

typedef Rec *RecPtr;


/* -------------------------------------------------------------------------- **
 * Global Variables...
 *
 *  CachePtr  - A cache header, allocated as an array of one element so that
 *              <CachePtr> will be a pointer.
 */

static ubi_cacheRoot CachePtr[1];


/* -------------------------------------------------------------------------- **
 * Functions...
 */

signed int compfunc( ubi_trItemPtr ItemPtr, ubi_trNodePtr NodePtr )
  /* ------------------------------------------------------------------------ **
   * Comparison function (key lookup).
   *
   *  Input:  ItemPtr - A generic pointer to the comparison value.
   *                    This points to a key value.
   *          NodePtr - A pointer to a tree node.  The key stored in the
   *                    node will be compared against the key passed via
   *                    <ItemPtr>.
   *
   *  Output: A signed integer.  As with strcmp(3), a negative value means
   *          that the key indicated by <ItemPtr> was comparitively less
   *          than the key contained in <NodePtr>.  A return value of zero
   *          indicates that the two keys matched, and a positive value
   *          means that the <ItemPtr> key was the greater.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  char *A = (char *)ItemPtr;
  char *B = ((RecPtr)NodePtr)->key;

  return( strcmp( A, B ) );
  } /* compfunc */


void freefunc( ubi_trNodePtr Node )
  /* ------------------------------------------------------------------------ **
   * Free the cache entry *and* the associated strings.
   *
   *  Input:  Node  - A pointer to the node that is to be freed.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  RecPtr r = (RecPtr)Node;

  free( r->key );
  free( r->content );
  free( Node );
  } /* freefunc */


static void dumpcache( void )
  /* ------------------------------------------------------------------------ **
   * Print a dump of the cache contents.
   *
   *  Notes:  Traverse the cache tree in key order.  For each entry, print
   *          the entry count, energy size, key, and value (content).  The
   *          output is sent to <stdout>.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  unsigned long l = 0;    /* Counter.       */
  RecPtr        Entry;    /* Node pointer.  */

  (void)puts( "(Count, Mem)  [Key, Content]" );
  (void)puts( "============================" );
  Entry = (RecPtr)ubi_trFirst( CachePtr->root.root );
  while( Entry )
    {
    (void)printf( "(%ld: %ld)\t[%s, %s]\n",
                  ++l, Entry->node.entry_size, Entry->key, Entry->content );
    Entry = (RecPtr)ubi_trNext( Entry );
    }
  (void)fflush( stdout );
  } /* dumpcache */


static void dumpstate( FILE *outf )
  /* ------------------------------------------------------------------------ **
   * Print the current size (memory used & node count) of the cache.
   *
   *  Input:  outf  - File stream to which to send the output.  If this is
   *                  given as NULL, the output will be sent to <stderr>.
   *
   *  Notes:  This is where I really miss Python's ability to set a default
   *          value for parameters.  It would be a great way to handle
   *          defaulting <outf> to <stderr>.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  if( NULL == outf )
    outf = stderr;

  (void)fprintf( outf, "Memory Used: %ld bytes.\n",   CachePtr->mem_used );
  (void)fprintf( outf, "Cache Size:  %ld entries.\n", CachePtr->root.count );
  } /* dumpstate */


static void prunecache( void )
  /* ------------------------------------------------------------------------ **
   * Semi-randomly delete nodes in the cache, just to reduce its size.
   * ------------------------------------------------------------------------ **
   */
  {
  int    mem = 0;
  int    ent = 0;
  int    i;
  RecPtr Entry;
  RecPtr Next;

  /* Walk the entire tree.
   *  Nodes are removed in a pseudo-random fashion.  The value of <i> is
   *  initialized to equal the entry size of the first node.  With each
   *  subsequent node in the traversal, <i> is decremented until it reaches
   *  zero.  When <i> is zero, the current node (which is the <i+1>th node)
   *  is removed, and <i> is reset to the entry size of that node.
   */
  Entry = (RecPtr)ubi_trFirst( CachePtr->root.root );
  i     = Entry ? Entry->node.entry_size : 0;
  while( Entry )
    {
    Next = (RecPtr)ubi_trNext( Entry );
    i--;
    if( i < 1 )
      {
      i = Entry->node.entry_size;
      mem += i;
      ent += 1;
      (void)ubi_cacheDelete( CachePtr, Entry->key );
      }
    Entry = Next;
    }
  (void)fprintf( stderr, "Freed %d bytes, %d entries.\n", mem, ent );
  } /* prunecache */


static void Usage( char *name )
  /* ------------------------------------------------------------------------ **
   * Print a usage message and exit with an error code.
   * ------------------------------------------------------------------------ **
   */
  {
  int          i;
  static char *message[] =
    {
    "Usage: %s <entrymax> <memmax>",
    "\n\tThis program reads input from stdin and loads it into an",
    "\n\tin-memory cache.  The size of the cache may be limited by",
    "\n\tnumber of entries (entrymax) or by the amount of memory that it",
    "\n\tis allowed to consume (memmax).  A value of zero indicates no",
    "\n\tlimit.\n",
    "\n\tThe cache may be filled using the unix 'find' command, eg.:",
    "\n\t\tfind / -print 2>/dev/null | %s 0 0",
    "\n\n",
    NULL
    };

  for( i = 0; message[i]; i++ )
    (void)fprintf( stderr, message[i], name );
  exit( EXIT_FAILURE );
  } /* Usage */


int main( int argc, char *argv[] )
  /* ------------------------------------------------------------------------ **
   * Program main line.
   *
   *  Input:  argc  - You know what this is.
   *          argv  - You know what to do.
   *
   *  Output: EXIT_SUCCESS.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  char s[1024];

  if( argc != 3 )
    Usage( argv[0] );

  /* Initialize the cache header.
   */
  (void)ubi_cacheInit( CachePtr, compfunc, freefunc,
                       atoi( argv[1] ), atoi( argv[2] ) );
  (void)fprintf( stderr, "Cache opened with MaxEntries: %ld MaxMemory: %ld\n",
                 ubi_cacheGetMaxEntries( CachePtr ),
                 ubi_cacheGetMaxMemory( CachePtr ) );

  /* Load the cache.
   */
  while( gets( s ) && strlen(s) )
    {
    int     i;
    char   *p;
    RecPtr  rp;

    rp = (RecPtr)malloc( sizeof( Rec ) );

    /* parse the input. */
    p = strrchr( s, '/' );
    if( !p )
      rp->key = strdup( "" );
    else
      {
      rp->key = strdup( ++p );
      *p = '\0';
      }
    rp->content = strdup( s );

    /* Add up size of the record plus string lengths plus nul bytes. */
    i = sizeof( Rec ) + strlen( rp->key ) + strlen( rp->content ) + 2;

    (void)ubi_cachePut( CachePtr, i, (ubi_cacheEntryPtr)rp, rp->key );
    }

  /* Report on stuff.
   */
  (void)fputs( "Full cache:\n", stderr );
  dumpstate( NULL );

  /* Pseudo-randomly prune the cache. */
  (void)fputs( "\nRemoving semi-random entries:\n", stderr );
  prunecache();
  dumpstate( NULL );

  /* Trim the cache to a maximum of 500 entries.  */
  (void)fputs( "\nRestricting cache size to 500 entries:\n", stderr );
  (void)ubi_cacheSetMaxEntries( CachePtr, 500 );
  dumpstate( NULL );

  /* Trim the cache to a maximum of 32K bytes.  */
  (void)fputs( "\nRestricting cache size to 32K bytes:\n", stderr );
  (void)ubi_cacheSetMaxMemory( CachePtr, 32768 );
  dumpstate( NULL );

  /* Dump the cache.  */
  (void)puts( "\nCacheDump:" );
  dumpcache();

  /* Clear the cache and exit.  */
  ubi_cacheClear( CachePtr );
  return( EXIT_SUCCESS );
  } /* main */

/* ========================================================================== */
