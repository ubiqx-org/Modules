/* ========================================================================== **
 *                                tree-sample.c
 * -------------------------------------------------------------------------- **
 * License: Public Domain
 * Description: ubiqx binary tree implementation test program.
 * $Id: tree-sample.c; 2024-12-11 09:07:17 -0600; crh$
 * -------------------------------------------------------------------------- **
 * Notes:
 *  This is an example program that shows how the ubiqx binary tree modules
 *  can be used.  One key feature of this code is that you can select the
 *  type of binary tree to use (plain, AVL, or Splay) at compile time.
 *
 *  To compile using the plain binary tree:
 *    cc -o test-bintree -I ../modules tree-sample.c ../modules/ubi_BinTree.c
 *  To compile using the AVL tree:
 *    cc -o test-avltree -I ../modules -DUSE_AVL_TREE tree-sample.c \
 *        ../modules/ubi_AVLtree.c ../modules/ubi_BinTree.c
 *  To compile using the Splay tree:
 *    cc  -o test-splay -I ../modules -DUSE_SPLAY_TREE tree-sample.c \
 *        ../modules/ubi_SplayTree.c ../modules/ubi_BinTree.c
 *
 * ========================================================================== **
 */
#include <stdio.h>              /* Standard I/O.     */
#include <string.h>             /* String functions. */
#include <stdlib.h>             /* Standard C library header. */
#include <errno.h>              /* Error handling.   */

#if defined( USE_SPLAY_TREE )
#include "ubi_SplayTree.h"      /* Splay tree module.  */
#elif defined( USE_AVL_TREE )
#include "ubi_AVLtree.h"        /* AVL tree module.    */
#else
#include "ubi_BinTree.h"        /* Binary tree module. */
#endif


/* -------------------------------------------------------------------------- **
 * Constants...
 *
 * Just some numbers we need.
 */

#define NAMESIZE 240
#define bSIZE   1024


/* -------------------------------------------------------------------------- **
 * Typedefs...
 *
 *  Note: ulong is an unsigned long, oolong is a tea.
 *
 *  SampleRec    - This is the sample data record with which we will be
 *                 playing.  It starts with a ubi_trNode structure (so that
 *                 the tree functions can fiddle with it), the rest is data.
 *                 In this case, we're just using a fixed-length character
 *                 array.  In a "real" program, you could have all sorts
 *                 of stuff.
 *  SampleRecPtr - A pointer to a SampleRec.
 */

typedef unsigned long ulong;

typedef struct {
  ubi_trNode Node;
  char       Name[NAMESIZE];
  } SampleRec;

typedef SampleRec *SampleRecPtr;


/* -------------------------------------------------------------------------- **
 * Global Variables...
 *
 *  We'll declare our tree header globally.  The structure is called a
 *  ubi_trRoot, but it's really a housekeeping header with a pointer to the
 *  tree's root node.
 *
 *  Root    - The tree header.
 *  RootPtr - A pointer to the tree header.
 */

static ubi_trRoot    Root;
static ubi_trRootPtr RootPtr = &Root;


/* -------------------------------------------------------------------------- **
 * Functions...
 */

static size_t promptFor( const char *const p, char *const s, size_t sLen )
  /* ------------------------------------------------------------------------ **
   * Prompt for and read a string from <stdin>.
   *
   *  Input:  p     - Pointer to the prompt string.  This will be written to
   *                  <stdout>.
   *          s     - A pointer to a buffer into which the user input will
   *                  be read.  The input is terminated by an EOF, a newline,
   *                  or if the input string exceeds (<sLen> - 1) octets.
   *          sLen  - The size, in bytes, of <s>.  At most, (<sLen> - 1)
   *                  octets will be read from <stdin>.  This function
   *                  always reserves at least one octet of <s> for a
   *                  terminating NUL.
   *
   *  Output: The number of bytes read, which may be zero (0).  A return
   *          value of zero indicates the end of input.
   *
   *  Notes:
   *    - The carriage return character ('\r') is purposefully ignored.  It
   *      is skipped on input and not counted toward the total length of the
   *      string.  It is voidspace.  It evaporates into nothingness.
   *    - If the input includes a newline ('\n'), the newline terminates the
   *      string and is replaced with a NUL.  This differs from the behavior
   *      of fgets(3).
   *
   * ------------------------------------------------------------------------ **
   */
  {
  size_t  st  = 0;
  int     c;

  /* Reserve one octet for the terminating NUL. */
  if( 0 == sLen )
    return( 0 );
  sLen--;

  /* Read octets. */
  while( st < sLen )
    {
    switch( c = fgetc( stdin ) )
      {
      case '\r':
        break;
      case '\n':
      case EOF:
        s[st] = '\0';
        return( st );
      default:
        s[st++] = c;
        break;
      }
    }
  s[st] = '\0';
  return( st );
  } /* promptFor */

static int CompareFunc( ubi_trItemPtr ItemPtr, ubi_trNodePtr NodePtr )
  /* ------------------------------------------------------------------------ **
   * Data comparison function.
   *
   *  Input:  ItemPtr - A pointer to the comparison data record.
   *                    Take a look at the Insert(), Find() and Locate()
   *                    functions.  They all take a parameter of type
   *                    ubi_trItemPtr.  This is a pointer to the data to be
   *                    compared.  In Find() and Locate(), it's a value that
   *                    you want to find within the tree.  In Insert() it's
   *                    a pointer to the part of the new node that contains
   *                    the data to be compared.
   *          NodePtr - This is a pointer to a node in the tree.
   *  Output: An integer in one of three ranges:
   *          < 0 indicates ItemPtr < (the data contained in) NodePtr
   *          = 0 indicates ItemPtr = (the data contained in) NodePtr
   *          > 0 indicates ItemPtr > (the data contained in) NodePtr
   *
   *  Notes:  If we are going to sort the data, we will need to be able to
   *          compare the data.  That's what this function does.  A pointer
   *          to this function will be stored in the tree header.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  char *A, *B;

  A = (char *)ItemPtr;                /* Find the comparison data indicated
                                       * by ItemPtr.  In our case, the data
                                       * is a string, and ItemPtr already
                                       * points to it.  We simply typecast
                                       * ItemPtr to the correct pointer type.
                                       * This is typically what you would
                                       * need to do.
                                       */
  B = ((SampleRecPtr)NodePtr)->Name;  /* Find the data connected to the node
                                       * that is in the tree.  Remember that
                                       * this function gets a pointer to the
                                       * ubi_trNode structure.  Convert the
                                       * Node pointer to a pointer to our
                                       * record type, then find the data item
                                       * within that node.
                                       */
  return( strcmp( A, B ) );           /* Now compare and return the result. */
  } /* CompareFunc */

static void PrintNode( ubi_trNodePtr NodePtr, void *Userdata )
  /* ------------------------------------------------------------------------ **
   * Print out the contents of a record stored in the tree.
   *  Input:  NodePtr  - A pointer to the Node structure part of a record.
   *          UserData - In this program, we're using this to pass in a
   *                     a pointer to an integer.
   * ------------------------------------------------------------------------ **
   */
  {
  SampleRecPtr RecPtr;
  int         *IntPtr;

  RecPtr = (SampleRecPtr)NodePtr; /* RecPtr now points to the entire record. */
  IntPtr = (int *)Userdata;       /* IntPtr points to the integer.           */

  (*IntPtr)++;              /* Increment the integer indicated via Userdata. */
  (void)printf( "%d: %s\n", *IntPtr, RecPtr->Name );
  } /* PrintNode */

static void KillNode( ubi_trNodePtr NodePtr )
  /* ------------------------------------------------------------------------ **
   * Free the memory associated with a node (free the entire record).
   *  Input:  NodePtr  - A pointer to a node in the tree.
   * ------------------------------------------------------------------------ **
   */
  {
  /* This could be very complicated if your data portion includes pointers to
   * all sorts of other allocated memory.  In our case, it's quite simple.
   */
  free( NodePtr );
  } /* KillNode */

int main( int argc, char *argv[] )
  /* ------------------------------------------------------------------------ **
   * Program main line.
   * ------------------------------------------------------------------------ **
   */
  {
  char          s[bSIZE];
  SampleRecPtr  RecPtr;
  int           i;
  char         *ModInfo[2];

  for( i = ubi_trModuleID( 2, ModInfo ); i > 0; )
    (void)printf( "%s", ModInfo[--i] );

  (void)ubi_trInitTree( RootPtr,      /* Pointer to the tree header           */
                        CompareFunc,  /* Pointer to the comparison function.  */
                        0 );          /* Don't allow overwrites or duplicates.*/
                                      /* See the Insert() notes in BinTree.h. */

  while( promptFor( "Input string (blank line to exit)> ", s, bSIZE ) )
    {
    /* Allocate a new node to be added to the tree. */
    RecPtr = (SampleRecPtr)malloc( sizeof(SampleRec) );
    if( !RecPtr )
      {
      perror( "main" );
      exit( EXIT_FAILURE );
      }

    /* Copy the new data into the record. */
    s[NAMESIZE-1] = '\0';             /* Make sure the string is short enough.*/
    (void)strcpy( RecPtr->Name, s );  /* Copy the string into our new record. */

    /* Add the record to the tree. */
    if( !ubi_trInsert( RootPtr,       /* To which tree are we adding this?    */
                       RecPtr,        /* The new node to be added.            */
                       RecPtr->Name,  /* Points to the comparison field.      */
                       NULL )         /* Overwrites are not allowed.          */
      )
      {
      (void)fprintf( stderr, "Error: Duplicate key [%s].  ", s );
      (void)fprintf( stderr, "Record not added.\n" );
      KillNode( (ubi_trNodePtr)RecPtr );
      }
    }

  /* Now that the tree is full, dump it in sorted order. */
  i = 0;
  (void)ubi_trTraverse( RootPtr,    /* Tree root pointer. */
                        PrintNode,  /* Pointer to function that prints out
                                     * record contents.
                                     */
                        &i );       /* Userdata is typically not needed.
                                     * This is an example of what you can
                                     * do with it.
                                     */
  (void)printf( "A total of %d records found.\n", i );

  /* Now free all nodes in the tree. */
  (void)ubi_trKillTree( RootPtr,    /* Tree root pointer. */
                        KillNode ); /* Function that frees the node. */
  return( EXIT_SUCCESS );
  } /* main */

/* ========================================================================== */
