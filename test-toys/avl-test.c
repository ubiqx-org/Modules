/* ========================================================================== **
 *                                 avl-test.c
 * -------------------------------------------------------------------------- **
 * License: Public Domain
 * Description: ubiqx AVL tree implementation test program.
 * $Id: avl-test.c; 2014-10-23 16:40:12 -0500; Christopher R. Hertel$
 * -------------------------------------------------------------------------- **
 * Notes:
 *  This is a simple test program used to verify the workings of the AVL tree
 *  module.  See the comments for a description of how it all works.
 *
 *  To compile (from within the test-toys directory):
 *    cc -I ../modules -o avl-test \
 *       avl-test.c ../modules/ubi_AVLtree.c ../modules/ubi_BinTree.c
 *
 * ========================================================================== **
 */

#include <stdio.h>              /* Standard I/O.        */
#include <string.h>             /* String functions.    */
#include <stdlib.h>             /* Standard C library.  */
#include <errno.h>              /* Error handling.      */

#include "ubi_AVLtree.h"        /* AVL tree module.     */


/* -------------------------------------------------------------------------- **
 * Constants...
 *
 * This is a simple test program, so we're using fixed-length strings as our
 * data.  This constant sets the string length (the maximum length includes
 * the terminating NUL byte).
 */

#define NAMESIZE 16


/* -------------------------------------------------------------------------- **
 * Typedefs...
 *
 *  Note: ulong is an unsigned long; oolong is tea.
 *
 *  SampleRec    - This is the sample data record with which we will be
 *                 playing.  It starts with a ubi_trNode structure (so that
 *                 the tree functions can fiddle with it), the rest is data.
 *                 In this case, we're just using a fixed-length character
 *                 array.  In a "real" program, you could have all sorts
 *                 of stuff.  The <Name> field is both the data and the key.
 *
 *  SampleRecPtr - A pointer to a SampleRec.
 */

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

static int CompareFunc( ubi_trItemPtr ItemPtr, ubi_trNodePtr NodePtr )
  /* ------------------------------------------------------------------------ **
   * Compare node values, for sorting and searching.
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
   *
   *  Output: An integer in one of three ranges:
   *          < 0 indicates ItemPtr < (the data contained in) NodePtr
   *          = 0 indicates ItemPtr = (the data contained in) NodePtr
   *          > 0 indicates ItemPtr > (the data contained in) NodePtr
   *
   *  Notes:  In order to sort the data, we will need to be able to compare
   *          the data.  That's what this function does.  A pointer to this
   *          function will be stored in the tree header.
   *
   *          This function is passed a value and a node (via <ItemPtr> and
   *          <NodePtr>).  Internally, the generic <NodePtr> is cast to its
   *          original type so that the value within the node can be
   *          compared against the value indicated by <ItemPtr>.
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

static char *gName( char gender )
  /* ------------------------------------------------------------------------ **
   * This function returns a string representing the gender of the node.
   *
   *  Input:  gender  - One of [ubi_trLEFT, ubi_trRIGHT, ubi_trPARENT].
   *                    Tree nodes each include a <gender> field to
   *                    indicate whether the node is the left or right
   *                    child of its parent.  If the node is at the root
   *                    of the tree, then it is neither left nor right.
   *
   *  Output: Pointer to a static string describing the gender.  The
   *          string will be "<error>" if the input value is not recognized.
   *
   *  Notes:  Each node is either the LEFT or RIGHT child of its parent node.
   *          The root node has no parent, so it's gender is ROOT.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  switch( gender )
    {
    case ubi_trLEFT:   return( " LEFT" );
    case ubi_trPARENT: return( " ROOT" );
    case ubi_trRIGHT:  return( "RIGHT" );
    }
  return( "<error>" );
  } /* gName */

static char *bName( char balance )
  /* ------------------------------------------------------------------------ **
   * In an AVL tree, each node has a balance factor which should be -1, 0, or
   * +1.  The value indicates the difference between the height of the left
   * and right subtrees.  This function returns a string indicating the
   * balance factor.
   * ------------------------------------------------------------------------ **
   */
  {
  switch( balance )
    {
    case ubi_trLEFT:   return( "-" );
    case ubi_trPARENT: return( "=" );
    case ubi_trRIGHT:  return( "+" );
    }
  return( "<error>" );
  } /* bName */

static void PrintNode( ubi_trNodePtr NodePtr, void *Userdata )
  /* ------------------------------------------------------------------------ **
   * Print out the contents of a record stored in the tree.
   *  Input:  NodePtr  - A pointer to the Node structure part of a record.
   * ------------------------------------------------------------------------ **
   */
  {
  SampleRecPtr  RecPtr = (SampleRecPtr)NodePtr;
  ubi_trNodePtr avlptr = (ubi_trNodePtr)NodePtr; /**/

  (void)printf( "[ %s,    %s   ] %s\n", gName( avlptr->gender ),
                bName( avlptr->balance ), RecPtr->Name );
  } /* PrintNode */

static void KillNode( ubi_trNodePtr NodePtr )
  /* ------------------------------------------------------------------------ **
   * Free the memory associated with a node (free the entire record).
   *
   *  Input:  NodePtr  - A pointer to a node that has been removed from
   *                     the tree.
   *
   *  Notes:  The function you write to free a node structure could become
   *          quite complicated if your data portion is complex.
   *          In this case, it's quite simple.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  free( (void *)NodePtr );
  } /* KillNode */

void Prune( ubi_trRootPtr RootPtr )
  /* ------------------------------------------------------------------------ **
   * Remove arbitrarily selected nodes.
   *
   *  Input:  RootPtr - Pointer to the tree header.
   *
   *  Notes:  This function does a sort-of-arbitrary pruning of the tree.
   *          All nodes in which the second byte of the key is ascii 'u'
   *          are removed.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  ubi_trNodePtr This;

  fputs( "Deleting entries in which the second character is 'u',", stdout );
  fputs( " for no apparent reason.\n", stdout );

  for( This = ubi_trFirst( RootPtr->root );
       (NULL != This);
       This = ubi_trNext( This ) )
    {
    if( 'u' == ((SampleRecPtr)This)->Name[1] )
      {
      printf( "x %s x\n", ((SampleRecPtr)This)->Name );
      KillNode( ubi_trRemove( RootPtr, This ) );
      }
    }
  fflush( stdout );
  } /* Prune */

int Validate( ubi_trNodePtr NodePtr )
  /* ------------------------------------------------------------------------ **
   * This function performs a recursive traversal of the tree, checking the
   * balance of each node to ensure that it is correct.  An error message is
   * displayed if an incorrect balance value is found.  The total height of
   * the tree is returned.
   * ------------------------------------------------------------------------ **
   */
  {
  int left, right;

  if( NULL == NodePtr )
    return( 0 );
  left  = Validate( NodePtr->Link[ubi_trLEFT] );
  right = Validate( NodePtr->Link[ubi_trRIGHT] );
  if( NodePtr->balance != ( (right - left) + ubi_trEQUAL ) )
    printf( "\nNot Valid! %d : %d, %d\n", NodePtr->balance, left, right );
  else
    printf( "." );

  /* return tree height */
  return( 1 + (left > right ? left : right) );
  } /* Validate */

int main( int argc, char *argv[] )
  /* ------------------------------------------------------------------------ **
   * Program main line.
   * ------------------------------------------------------------------------ **
   */
  {
  char          s[1024];
  SampleRecPtr  RecPtr;
  unsigned int  i;
  char         *ModInfo[2];

  /* Print module info. */
  for( i = ubi_trModuleID( 2, ModInfo ); i > 0; )
    (void)printf( "%s", ModInfo[--i] );

  /* Initialize the tree. */
  (void)ubi_trInitTree( RootPtr,      /* Pointer to the tree header           */
                        CompareFunc,  /* Pointer to the comparison function.  */
                        0 );          /* Don't allow overwrites or duplicates.*/
                                      /* See the Insert() notes in BinTree.h. */

  /* Load the tree from stdin. */
  while( gets( s ) && strlen(s) )
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
      /* We disallowed duplicate keys, so the insert failed.
       *  Free the allocated record.
       */
      KillNode( (ubi_trNodePtr)RecPtr );
      }
    }

  /* Delete entries just to see that deleting entries works. */
  Prune( RootPtr );

  /* Now validate the tree. */
  printf( "\nValidating the structure of the AVL tree.\n" );
  printf( "\nTree height is %d\n", Validate( (ubi_trNodePtr)(RootPtr->root) ) );

  /* Now dump the remaining entries, providing balance and gender info. */
  puts( "[gender, balance] content" );
  puts( " ------  -------  -------" );
  (void)ubi_trTraverse( RootPtr, PrintNode, NULL );

  /* Now free all nodes in the tree. */
  (void)ubi_trKillTree( RootPtr,    /* Tree root pointer. */
                        KillNode ); /* Function that frees the node. */

  return( EXIT_SUCCESS );
  } /* main */

/* ========================================================================== */
