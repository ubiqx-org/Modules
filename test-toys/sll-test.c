/* ========================================================================== **
 *                                 sll-test.c
 *
 * License: Public Domain
 * Description: A demo/test program for the ubi_sLinkList module.
 * $Id: sll-test.c; 2014-11-21 11:04:16 -0600; Christopher R. Hertel$
 * -------------------------------------------------------------------------- **
 * What it does...
 *
 *  This program demonstrates the workings and usage of the ubiqx
 *  ubi_sLinkList module, which implements a simple singly-linked list.
 *
 * ========================================================================== **
 */

#include <stdio.h>          /* gets(3), printf(3), etc.         */
#include <stdlib.h>         /* malloc(3), free(3), etc.         */
#include <string.h>         /* strlen(3), strcpy(3), etc.       */
#include "ubi_sLinkList.h"  /* Header for the sLinkList module. */


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
  ubi_slNode node;        /* Must always be the first field in the struct.  */
  char       string[1];   /* It's a pointer!  No, it's a single-byte array! */
  } Entry;

typedef Entry *EntryPtr;


/* -------------------------------------------------------------------------- **
 * Globals
 *
 *  List  - A linked list header structure, initialized to an empty list.
 *          ubi_slNewList() is a macro that hides the grody details.
 *          What is actually defined here is an array with only one
 *          element, so <List> can be passed as a pointer.
 */

static ubi_slNewList( List );


/* -------------------------------------------------------------------------- **
 * Functions
 */

static void printlist( FILE *out, ubi_slListPtr ListPtr )
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
  ubi_slNodePtr np;
  ubi_slNodePtr head  = ubi_slFirst( ListPtr );
  ubi_slNodePtr tail  = ubi_slLast( ListPtr );
  unsigned long count = 0;

  if( !head )
    {
    /* Empty tree.  */
    fprintf( out, "head --0\n  ^\n  |\ntail\n" );
    if( ubi_slCount( ListPtr ) )
      (void)fprintf( out, "Error: Internal list count is non-zero: %ld.\n",
                          ubi_slCount( ListPtr ) );
    return;
    }

  /* Traverse the list, printing out each string plus (symbolicly)
   * the node links.
   */
  for( np = head; np; count++, np = ubi_slNext( np ) )
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

    /* Print out the pointers to the next node, or list end.  */
    if( np->Next )
      (void)fprintf( out, "          |\n          V\n" );
    else
      (void)fprintf( out, "          |\n          0\n" );
    }

  /* Did the node count match?  */
  if( ubi_slCount( ListPtr ) != count )
    (void)fprintf( out, "Error: Node count mismatch; %ld vs. %ld\n",
                        ubi_slCount( ListPtr ), count );
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
    "This simple program is used to test the ubi_sLinkList module.",
    "",
    "Lines of text are read from <stdin> and stored, in order, in a singly",
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
  unsigned long i, j, keepcount;
  size_t        pos;
  char          bufr[bSIZE];
  EntryPtr      entryptr;
  ubi_slNodePtr n;

  /* Find out whether help is required. */
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
      (void)ubi_slAddTail( List, &(entryptr->node) );
      }
    }
  (void)fprintf( stderr, "...loaded %ld of %ld strings.\n",
                         ubi_slCount( List ), i );

  /* Dump the list. */
  printlist( stdout, List );
  (void)fprintf( stderr, "Printed %ld entries.\n\n", ubi_slCount( List ) );

  /* Test removals */
  keepcount = ubi_slCount( List );

  /* Remove the last entry in the list, if there is one.  This is difficult.  */
  j = keepcount - 1;
  n = ubi_slFirst( List );
  for( i = 1; (n) && (i < j); i++ )     /* Search for the penultimate entry.  */
    n = ubi_slNext( n );
  /* Did we find the penultimate entry?  If so, remove and free its successor.*/
  if( n && ubi_slNext( n ) )
    free( ubi_slRemNext( List, n ) );

  /* Remove the second item in the list, if there is one. */
  n = ubi_slFirst( List );
  if( n && ubi_slNext( n ) )
    free( ubi_slRemNext( List, n ) );

  /* Remove the head of the list, if it exists. */
  if( (n = ubi_slRemHead( List )) )
    free( n );

  /* Report the results.  */
  i = keepcount - ubi_slCount( List );
  (void)fprintf( stderr, "Removed %ld node%s.\n", i, (1==i)?"":"s" );

  /* Dump the list, again.  */
  printlist( stdout, List );
  (void)fprintf( stderr, "Printed %ld entries.\n\n", ubi_slCount( List ) );

  /* Free the list. */
  while( (n = ubi_slRemHead( List )) )
    free( n );
  (void)fprintf( stderr, "Freed all entries.\n" );

  /* All done. */
  return( EXIT_SUCCESS );
  } /* main */

/* ========================================================================== */
