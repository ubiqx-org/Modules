/* ========================================================================== **
 *                                 dll-test.c
 * -------------------------------------------------------------------------- **
 * License: Public Domain
 * Description: ubiqx doubly-linked-list test program.
 * $Id: dll-test.c; 2014-11-20 22:27:46 -0600; Christopher R. Hertel$
 * -------------------------------------------------------------------------- **
 * Notes:
 *  This is a simple test program used to verify the workings of the
 *  ubiqx doubly-linked-list module.
 *
 *  To compile (from within the test-toys directory):
 *    cc -I ../modules -o dll-test dll-test.c ../modules/ubi_dLinkList.c
 *
 * ========================================================================== **
 */

#include <stdio.h>          /* gets(3), printf(3), etc.         */
#include <stdlib.h>         /* malloc(3), free(3), etc.         */
#include <string.h>         /* strlen(3), strcpy(3), etc.       */
#include "sys_include.h"    /* May help to improve portability. */
#include "ubi_dLinkList.h"  /* Header for the dLinkList module. */


/* -------------------------------------------------------------------------- **
 * Defined Constants
 */

#define bSIZE 1024          /* Input buffer size, in bytes.     */


/* -------------------------------------------------------------------------- **
 * Typedefs
 *
 *  Entry     - The node structure used to store the input data that we're
 *              handling.  In this case, we're only storing a string.  The
 *              single-byte string is a coding trick.  When the node is
 *              actually mallocated, we'll add the node size to the length
 *              of the string.  The single byte will account for the NUL
 *              terminator.
 *  EntryPtr  - A pointer to an Entry structure.
 */

typedef struct
  {
  ubi_dlNode node;        /* Must always be the first field in the struct.  */
  char       string[1];   /* Both a pointer and a single-byte array.        */
  } Entry;

typedef Entry *EntryPtr;

/* -------------------------------------------------------------------------- **
 * Globals
 *
 *  List  - A linked list header structure, initialized to an empty list.
 *          ubi_dlNewList() is a macro that hides the grody details.
 *          What is actually defined here is an array with only one
 *          element, so <List> can be passed as a pointer.
 */

static ubi_dlNewList( List );


/* -------------------------------------------------------------------------- **
 * Functions
 */

static void printlist( FILE *out, ubi_dlListPtr ListPtr )
  /* ------------------------------------------------------------------------ **
   * Pretty-print the linked list.
   *
   *  Input:  out     - The output stream to which to write.
   *          ListPtr - Pointer to (the header of) the linked list to be
   *                    printed.
   *
   *  Output: <none>
   *
   * ------------------------------------------------------------------------ **
   */
  {
  ubi_dlNodePtr np;
  ubi_dlNodePtr head  = ubi_dlFirst( ListPtr );
  ubi_dlNodePtr tail  = ubi_dlLast( ListPtr );
  unsigned long count = 0;

  if( !head )
    {
    /* Empty tree.  */
    (void)fprintf( out, "head --0\ntail --0\n" );
    if( ubi_dlCount( ListPtr ) )
      (void)fprintf( out, "Error: Internal list count is non-zero: %ld.\n",
                          ubi_dlCount( ListPtr ) );
    return;
    }

  /* Print the start of the linked list, and check for a bogus Prev
   * pointer in the first node.
   */
  if( head->Prev  )
    (void)fprintf( out, "            ^ (error!)\n            |\n" );
  else
    (void)fprintf( out, "            0\n            |\n" );

  /* Traverse the list, printing out each string plus (symbolicly)
   * the node links.
   */
  for( np = head; np; count++, np = ubi_dlNext( np ) )
    {
    int len;

    /* Decide whether to print "head", "tail", "both", or spaces. */
    if( np == head )
      if( head == tail )
        (void)fprintf( out, "both -->" );
      else
        (void)fprintf( out, "head -->" );
    else
      if( np == tail )
        (void)fprintf( out, "tail -->" );
      else
        (void)fprintf( out, "        " );

    /* Print the string; trim it if needed. */
    len = strlen( ((EntryPtr)np)->string );
    len = ((len > 60) ? 60 : len);
    (void)fprintf( out, " %.*s\n", len, ((EntryPtr)np)->string );

    /* Print out the pointers to the next & prev nodes, or list end.  */
    if( np->Next )
      {
      if( np->Next->Prev )
        {
        if( np->Next->Prev == np )
          (void)fprintf( out, "          | ^\n          v |\n" );
        else
          (void)fprintf( out, "          | ? (error!)\n          v |\n" );
        }
      else
        (void)fprintf( out, "          | 0 (error!)\n          v |\n" );
      }
    else
      (void)fprintf( out, "          |\n          0\n" );
    }

  /* Did the node count match?  */
  if( ubi_dlCount( ListPtr ) != count )
    (void)fprintf( out, "Error: Node count mismatch; %ld vs. %ld\n",
                        ubi_dlCount( ListPtr ), count );
  /* Done */
  return;
  } /* printlist */

static void usage( const char *progname )
  /* ------------------------------------------------------------------------ **
   * Provide a small amount of help, then exit.
   *
   *  Input:  progname  - A string, which should be the program name.
   *
   *  Output: <none>
   *
   * ------------------------------------------------------------------------ **
   */
  {
  int          i;
  static char *msg[] =
    {
    "This simple program is used to test the ubi_dLinkList module.",
    "",
    "Lines of text are read from <stdin> and stored, in order, in a doubly",
    "linked list.  The list will be displayed on <stdout>, but diagnostics",
    "are sent to <stderr>.",
    "",
    NULL
    };

  for( i = 0; (NULL != msg[i]); i++ )
    (void)fprintf( stderr, "%s\n", msg[i] );
  (void)fprintf( stderr, "Example: ls | %s\n", progname );
  exit( EXIT_SUCCESS );
  } /* usage */

int main( int argc, char *argv[] )
  /* ------------------------------------------------------------------------ **
   * Program mainline.
   *
   *  Input:  argc  - Expecting no arguments (should be 1).
   *          argv  - Should contain only the program name.
   *
   *  Output: EXIT_SUCCESS.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  unsigned long i, keepcount;
  size_t        pos;
  char          bufr[bSIZE];
  EntryPtr      entryptr;
  ubi_dlNodePtr n;

  /* See if help is required. */
  if( argc != 1 )
    usage( argv[0] );

  /* Load the list. */
  (void)fprintf( stderr, "Reading from <stdin>...\n" );
  for( i = 0; fgets( bufr, bSIZE, stdin ); i++ )
    {
    /* Replace newline with NUL.  */
    pos = strlen( bufr ) - 1;
    if( (pos >= 0) && ('\n' == bufr[ pos ]) )
      bufr[ pos ] = '\0';
    /* If we can allocate memory, store the string and add the node.  */
    if( (entryptr = (EntryPtr)malloc( sizeof(Entry) + strlen(bufr) )) )
      {
      (void)strcpy( entryptr->string, bufr );
      (void)ubi_dlAddTail( List, (ubi_dlNode *)entryptr );
      }
    }
  (void)fprintf( stderr, "...loaded %ld of %ld strings.\n",
                         ubi_dlCount( List ), i );

  /* Dump the list. */
  printlist( stdout, List );
  (void)fprintf( stderr, "Printed %ld entries.\n\n", ubi_dlCount( List ) );

  /* Test removals...
   */
  keepcount = ubi_dlCount( List );
  /* Remove the second item in the list, if it exists.  */
  n = ubi_dlFirst( List );
  if( n && (n = ubi_dlNext( n )) )
    free( ubi_dlRemThis( List, n ) );
  /* Now remove the head and tail nodes.  */
  if( ubi_dlFirst( List ) )
    free( ubi_dlRemHead( List ) );
  if( ubi_dlFirst( List ) )
    free( ubi_dlRemTail( List ) );
  /* Report the results.  */
  i = keepcount - ubi_dlCount( List );
  (void)fprintf( stderr, "Removed %ld node%s.\n", i, (1==i)?"":"s" );

  /* Dump the list. */
  printlist( stdout, List );
  (void)fprintf( stderr, "Printed %ld entries.\n\n", ubi_dlCount( List ) );

  /* Free the list. */
  while( ubi_dlCount( List ) > 0 )
    free( ubi_dlRemHead( List ) );
  (void)fprintf( stderr, "Freed all entries.\n" );

  return( EXIT_SUCCESS );
  } /* main */

/* ========================================================================== */
