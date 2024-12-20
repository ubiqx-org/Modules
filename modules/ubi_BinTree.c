/* ========================================================================== **
 *                              ubi_BinTree.c
 *
 *  Copyright (C) 1991-1998, 2014, 2020 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *
 *  This module implements a simple binary tree.
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
 * $Id: ubi_BinTree.c; 2024-12-11 08:53:18 -0600; crh$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 4.16  2014/11/19 crh
 * New code was written, so the copyright dates have been updated.
 *
 * Revision 4.15  2014/11/06 crh
 * Updated comments only.
 *
 * Revision 4.14  2014/10/30 crh
 * Overhauled the algorithm used in ubi_btLeafNode().  The new version does
 * a slightly better job of distributing leaf node selection across the lower
 * levels of the tree.  At least, it appears to do so in testing.
 *
 * Revision 4.13  2005/10/25 01:51:23  crh
 * Changed the inner workings of the ubi_btTraverse() function.  It is now
 * safe to remove nodes from the tree during a ubi_btTraversal.  You can
 * remove them, free() them, whatever.  Just be sure you only touch the
 * *current* node in the tree... the one passed to the user function.
 *
 * Revision 4.12  2004/06/06 04:51:56  crh
 * Fixed a small typo in ubi_BinTree.c (leftover testing cruft).
 * Did a small amount of formatting touchup to ubi_BinTree.h.
 *
 * Revision 4.11  2004/06/06 03:14:09  crh
 * Rewrote the ubi_btLeafNode() function.  It now takes several paths in an
 * effort to find a deeper leaf node.  There is a small amount of extra
 * overhead, but it is limited.
 *
 * Revision 4.10  2000/06/06 20:38:40  crh
 * In the ReplaceNode() function, the old node header was being copied
 * to the new node header using a byte-by-byte copy.  This was causing
 * the 'insure' software testing program to report a memory leak.  The
 * fix was to do a simple assignement: *newnode = *oldnode;
 * This quieted the (errant) memory leak reports and is probably a bit
 * faster than the bytewise copy.
 *
 * Revision 4.9  2000/01/08 23:24:30  crh
 * Clarified a variety of if( pointer ) lines, replacing them with
 * if( NULL != pointer ).  This is more correct, and I have heard
 * of at least one (obscure?) system out there that uses a non-zero
 * value for NULL.
 * Also, speed improvement in Neighbor().  It was comparing pointers
 * when it could have compared two gender values.  The pointer
 * comparison was somewhat indirect (does pointer equal the pointer
 * of the parent of the node pointed to by pointer).  Urq.
 *
 * Revision 4.8  1999/09/22 03:40:30  crh
 * Modified ubi_btTraverse() and ubi_btKillTree().  They now return an
 * unsigned long indicating the number of nodes processed.  The change
 * is subtle.  An empty tree formerly returned False, and now returns
 * zero.
 *
 * Revision 4.7  1998/10/21 06:14:42  crh
 * Fixed bugs in FirstOf() and LastOf() reported by Massimo Campostrini,
 * Istituto Nazionale di Fisica Nucleare, Sezione di Pisa.
 *
 * Revision 4.6  1998/07/25 17:02:10  crh
 * Added the ubi_trNewTree() macro.
 *
 * Revision 4.5  1998/06/04 21:29:27  crh
 * Upper-cased defined constants (eg UBI_BINTREE_H) in some header files.
 * This is more "standard", and is what people expect.  Weird, eh?
 *
 * Revision 4.4  1998/06/03 17:42:46  crh
 * Further fiddling with sys_include.h.  It's now in ubi_BinTree.h which is
 * included by all of the binary tree files.
 *
 * Reminder: Some of the ubi_tr* macros in ubi_BinTree.h are redefined in
 *           ubi_AVLtree.h and ubi_SplayTree.h.  This allows easy swapping
 *           of tree types by simply changing a header.  Unfortunately, the
 *           macro redefinitions in ubi_AVLtree.h and ubi_SplayTree.h will
 *           conflict if used together.  You must either choose a single tree
 *           type, or use the underlying function calls directly.  Compare
 *           the two header files for more information.
 *
 * Revision 4.3  1998/06/02 01:28:43  crh
 * Changed ubi_null.h to sys_include.h to make it more generic.
 *
 * Revision 4.2  1998/05/20 04:32:36  crh
 * The C file now includes ubi_null.h.  See ubi_null.h for more info.
 * Also, the balance and gender fields of the node were declared as
 * signed char.  As I understand it, at least one SunOS or Solaris
 * compiler doesn't like "signed char".  The declarations were
 * wrong anyway, so I changed them to simple "char".
 *
 * Revision 4.1  1998/03/31 06:11:57  crh
 * Thomas Aglassinger sent E'mail pointing out errors in the
 * dereferencing of function pointers, and a missing typecast.
 * Thanks, Thomas!
 *
 * Revision 4.0  1998/03/10 03:19:22  crh
 * Added the AVL field 'balance' to the ubi_btNode structure.  This means
 * that all BinTree modules now use the same basic node structure, which
 * greatly simplifies the AVL module.
 * Decided that this was a big enough change to justify a new major revision
 * number.  3.0 was an error, so we're at 4.0.
 *
 * Revision 2.6  1998/01/24 06:27:46  crh
 * Added ubi_trCount() macro.
 *
 * Revision 2.5  1997/12/23 03:56:29  crh
 * In this version, all constants & macros defined in the header file have
 * the ubi_tr prefix.  Also cleaned up anything that gcc complained about
 * when run with '-pedantic -fsyntax-only -Wall'.
 *
 * Revision 2.4  1997/07/26 04:11:10  crh
 * + Just to be annoying I changed ubi_TRUE and ubi_FALSE to ubi_trTRUE
 *   and ubi_trFALSE.
 * + There is now a type ubi_trBool to go with ubi_trTRUE and ubi_trFALSE.
 * + There used to be something called "ubi_TypeDefs.h".  I got rid of it.
 * + Added function ubi_btLeafNode().
 *
 * Revision 2.3  1997/06/03 05:16:17  crh
 * Changed TRUE and FALSE to ubi_TRUE and ubi_FALSE to avoid conflicts.
 * Also changed the interface to function InitTree().  See the comments
 * for this function for more information.
 *
 * Revision 2.2  1995/10/03 22:00:07  CRH
 * Ubisized!
 *
 * Revision 2.1  95/03/09  23:37:10  CRH
 * Added the ModuleID static string and function.  These modules are now
 * self-identifying.
 *
 * Revision 2.0  95/02/27  22:00:17  CRH
 * Revision 2.0 of this program includes the following changes:
 *
 *     1)  A fix to a major typo in the RepaceNode() function.
 *     2)  The addition of the static function Border().
 *     3)  The addition of the public functions FirstOf() and LastOf(), which
 *         use Border(). These functions are used with trees that allow
 *         duplicate keys.
 *     4)  A complete rewrite of the Locate() function.  Locate() now accepts
 *         a "comparison" operator.
 *     5)  Overall enhancements to both code and comments.
 *
 * I decided to give this a new major rev number because the interface has
 * changed.  In particular, there are two new functions, and changes to the
 * Locate() function.
 *
 * Revision 1.0  93/10/15  22:44:59  CRH
 * With this revision, I have added a set of #define's that provide a single,
 * standard API to all existing tree modules.  Until now, each of the three
 * existing modules had a different function and typedef prefix, as follows:
 *
 *       Module        Prefix
 *     ubi_BinTree     ubi_bt
 *     ubi_AVLtree     ubi_avl
 *     ubi_SplayTree   ubi_spt
 *
 * To further complicate matters, only those portions of the base module
 * (ubi_BinTree) that were superceeded in the new module had the new names.
 * For example, if you were using ubi_SplayTree, the locate function was
 * called "ubi_sptLocate", but the next and previous functions remained
 * "ubi_btNext" and "ubi_btPrev".
 *
 * This was not too terrible if you were familiar with the modules and knew
 * exactly which tree model you wanted to use.  If you wanted to be able to
 * change modules (for speed comparisons, etc), things could get messy very
 * quickly.
 *
 * So, I have added a set of defined names that get redefined in any of the
 * descendant modules.  To use this standardized interface in your code,
 * simply replace all occurances of "ubi_bt", "ubi_avl", and "ubi_spt" with
 * "ubi_tr".  The "ubi_tr" names will resolve to the correct function or
 * datatype names for the module that you are using.  Just remember to
 * include the header for that module in your program file.  Because these
 * names are handled by the preprocessor, there is no added run-time
 * overhead.
 *
 * Note that the original names do still exist, and can be used if you wish
 * to write code directly to a specific module.  This should probably only be
 * done if you are planning to implement a new descendant type, such as
 * red/black trees.  CRH
 *
 *  V0.0 - June, 1991   -  Written by Christopher R. Hertel (CRH).
 *
 * ========================================================================== **
 */

#include "ubi_BinTree.h"  /* Header for this module.   */


/* ========================================================================== **
 * Static data.
 */

static char ModuleID[] =
  "$Id: ubi_BinTree.c; 2024-12-11 08:53:18 -0600; crh$\n";

/* ========================================================================== **
 * Internal (private) functions.
 */

static ubi_btNodePtr qFind( ubi_btCompFunc cmp,
                            ubi_btItemPtr  FindMe,
                   register ubi_btNodePtr  p )
  /* ------------------------------------------------------------------------ **
   * This function performs a non-recursive search of a tree for a node
   * matching a specific key.  It is called "qFind()" because it is
   * faster that TreeFind (below).
   *
   *  Input:
   *     cmp      -  a pointer to the tree's comparison function.
   *     FindMe   -  a pointer to the key value for which to search.
   *     p        -  a pointer to the starting point of the search.  <p>
   *                 is considered to be the root of a subtree, and only
   *                 the subtree will be searched.
   *
   *  Output:
   *     A pointer to a node with a key that matches the key indicated by
   *     FindMe, or NULL if no such node was found.
   *
   *  Note:   In a tree that allows duplicates, the pointer returned *might
   *          not* point to the (sequentially) first occurance of the
   *          desired key.
   * ------------------------------------------------------------------------ **
   */
  {
  int tmp;

  while( (NULL != p)
      && ((tmp = ubi_trAbNormal( (*cmp)(FindMe, p) )) != ubi_trEQUAL) )
    p = p->Link[tmp];

  return( p );
  } /* qFind */

static ubi_btNodePtr TreeFind( ubi_btItemPtr  findme,
                               ubi_btNodePtr  p,
                               ubi_btNodePtr *parentp,
                               char          *gender,
                               ubi_btCompFunc CmpFunc )
  /* ------------------------------------------------------------------------ **
   * TreeFind() searches a tree for a given value (findme).  It will return a
   * pointer to the target node, if found, or NULL if the target node was not
   * found.
   *
   * TreeFind() also returns, via parameters, a pointer to the parent of the
   * target node, and a LEFT or RIGHT value indicating which child of the
   * parent is the target node.  *If the target is not found*, then these
   * values indicate the place at which the target *should be found*.  This
   * is useful when inserting a new node into a tree or searching for nodes
   * "near" the target node.
   *
   * The parameters are:
   *
   *  findme   -  is a pointer to the key information to be searched for.
   *  p        -  points to the root of the tree to be searched.
   *  parentp  -  will return a pointer to a pointer to the !parent! of the
   *              target node, which can be especially usefull if the target
   *              was not found.
   *  gender   -  returns LEFT or RIGHT to indicate which child of *parentp
   *              was last searched.
   *  CmpFunc  -  points to the comparison function.
   *
   * This function is called by ubi_btLocate() and ubi_btInsert().
   * ------------------------------------------------------------------------ **
   */
  {
  register ubi_btNodePtr tmp_p      = p;
  ubi_btNodePtr          tmp_pp     = NULL;
  char                   tmp_gender = ubi_trEQUAL;
  int                    tmp_cmp;

  while( (NULL != tmp_p)
     && (ubi_trEQUAL != (tmp_cmp = ubi_trAbNormal((*CmpFunc)(findme, tmp_p)))) )
    {
    tmp_pp     = tmp_p;                 /* Keep track of previous node. */
    tmp_gender = (char)tmp_cmp;         /* Keep track of sex of child.  */
    tmp_p      = tmp_p->Link[tmp_cmp];  /* Go to child. */
    }
  *parentp = tmp_pp;                /* Return results. */
  *gender  = tmp_gender;
  return( tmp_p );
  } /* TreeFind */

static void ReplaceNode( ubi_btNodePtr *parent,
                         ubi_btNodePtr  oldnode,
                         ubi_btNodePtr  newnode )
  /* ------------------------------------------------------------------------ **
   * Remove node oldnode from the tree, replacing it with node newnode.
   *
   * Input:
   *  parent   - A pointer to he parent pointer of the node to be
   *             replaced.  <parent> may point to the Link[] field of
   *             a parent node, or it may indicate the root pointer at
   *             the top of the tree.
   *  oldnode  - A pointer to the node that is to be replaced.
   *  newnode  - A pointer to the node that is to be installed in the
   *             place of <*oldnode>.
   *
   * Notes:    Don't forget to free oldnode.
   *
   * ------------------------------------------------------------------------ **
   */
  {
  *newnode = *oldnode;  /* Copy node internals to new node. */

  (*parent) = newnode;  /* Old node's parent points to new child. */
  /* Now tell the children about their new step-parent. */
  if( oldnode->Link[ubi_trLEFT] )
    (oldnode->Link[ubi_trLEFT])->Link[ubi_trPARENT] = newnode;
  if( oldnode->Link[ubi_trRIGHT] )
    (oldnode->Link[ubi_trRIGHT])->Link[ubi_trPARENT] = newnode;
  } /* ReplaceNode */

static void SwapNodes( ubi_btRootPtr RootPtr,
                       ubi_btNodePtr Node1,
                       ubi_btNodePtr Node2 )
  /* ------------------------------------------------------------------------ **
   * This function swaps two nodes in the tree.  Node1 will take the place of
   * Node2, and Node2 will fill in the space left vacant by Node 1.
   *
   * Input:
   *  RootPtr  - pointer to the tree header structure for this tree.
   *  Node1    - \
   *              > These are the two nodes which are to be swapped.
   *  Node2    - /
   *
   * Notes:
   *  This function does a three step swap, using a dummy node as a place
   *  holder.  This function is used by ubi_btRemove().
   * ------------------------------------------------------------------------ **
   */
  {
  ubi_btNodePtr *Parent;
  ubi_btNode     dummy;
  ubi_btNodePtr  dummy_p = &dummy;

  /* Replace Node 1 with the dummy, thus removing Node1 from the tree. */
  if( NULL != Node1->Link[ubi_trPARENT] )
    Parent = &((Node1->Link[ubi_trPARENT])->Link[(int)(Node1->gender)]);
  else
    Parent = &(RootPtr->root);
  ReplaceNode( Parent, Node1, dummy_p );

  /* Swap Node 1 with Node 2, placing Node 1 back into the tree. */
  if( NULL != Node2->Link[ubi_trPARENT] )
    Parent = &((Node2->Link[ubi_trPARENT])->Link[(int)(Node2->gender)]);
  else
    Parent = &(RootPtr->root);
  ReplaceNode( Parent, Node2, Node1 );

  /* Swap Node 2 and the dummy, thus placing Node 2 back into the tree. */
  if( NULL != dummy_p->Link[ubi_trPARENT] )
    Parent = &((dummy_p->Link[ubi_trPARENT])->Link[(int)(dummy_p->gender)]);
  else
    Parent = &(RootPtr->root);
  ReplaceNode( Parent, dummy_p, Node2 );
  } /* SwapNodes */

/* -------------------------------------------------------------------------- **
 * These routines allow you to walk through the tree, forwards or backwards.
 */

static ubi_btNodePtr SubSlide( register ubi_btNodePtr P,
                               register int           whichway )
  /* ------------------------------------------------------------------------ **
   * Slide down the side of a subtree.
   *
   * Given a starting node, this function returns a pointer to the LEFT-, or
   * RIGHT-most descendent, *or* (if whichway is PARENT) to the tree root.
   *
   *  Input:  P         - a pointer to a starting place.
   *          whichway  - the direction (LEFT, RIGHT, or PARENT) in which to
   *                      travel.
   *  Output: A pointer to a node that is either the root, or has no
   *          whichway-th child but is within the subtree of P.  Note that
   *          the return value may be the same as P.  The return value *will
   *          be* NULL if P is NULL.
   * ------------------------------------------------------------------------ **
   */
  {

  if( NULL != P )
    while( NULL != P->Link[ whichway ] )
      P = P->Link[ whichway ];
  return( P );
  } /* SubSlide */

static ubi_btNodePtr Neighbor( register ubi_btNodePtr P,
                               register int           whichway )
  /* ------------------------------------------------------------------------ **
   * Given starting point p, return the (key order) next or preceeding node
   * in the tree.
   *
   *  Input:  P         - Pointer to our starting place node.
   *          whichway  - the direction in which to travel to find the
   *                      neighbor, i.e., the RIGHT neighbor or the LEFT
   *                      neighbor.
   *
   *  Output: A pointer to the neighboring node, or NULL if P was NULL.
   *
   *  Notes:  If whichway is PARENT, the results are unpredictable.
   * ------------------------------------------------------------------------ **
   */
  {
  if( P )
    {
    if( NULL != P->Link[ whichway ] )
      return( SubSlide( P->Link[ whichway ], (char)ubi_trRevWay(whichway) ) );
    else
      while( NULL != P->Link[ ubi_trPARENT ] )
        {
        if( whichway == P->gender )
          P = P->Link[ ubi_trPARENT ];
        else
          return( P->Link[ ubi_trPARENT ] );
        }
    }
  return( NULL );
  } /* Neighbor */

static ubi_btNodePtr Border( ubi_btRootPtr RootPtr,
                             ubi_btItemPtr FindMe,
                             ubi_btNodePtr p,
                             int           whichway )
  /* ------------------------------------------------------------------------ **
   * Given starting point p, which has a key value equal to *FindMe, locate
   * the first (index order) node with the same key value.
   *
   * This function is useful in trees that have can have duplicate keys.
   * For example, consider the following tree:
   *     Tree                                                      Traversal
   *       2    If <p> points to the root and <whichway> is RIGHT,     3
   *      / \    then the return value will be a pointer to the       / \
   *     2   2    RIGHT child of the root node.  The tree on         2   5
   *    /   / \    the right shows the order of traversal.          /   / \
   *   1   2   3                                                   1   4   6
   *
   *  Input:  RootPtr   - Pointer to the tree root structure.
   *          FindMe    - Key value for comparisons.
   *          p         - Pointer to the starting-point node.
   *          whichway  - the direction in which to travel to find the
   *                      neighbor, i.e., the RIGHT neighbor or the LEFT
   *                      neighbor.
   *
   *  Output: A pointer to the first (index, or "traversal", order) node with
   *          a Key value that matches *FindMe.
   *
   *  Notes:  If whichway is PARENT, or if the tree does not allow duplicate
   *          keys, this function will return <p>.
   * ------------------------------------------------------------------------ **
   */
  {
  register ubi_btNodePtr q;

  /* Exit if there's nothing that can be done. */
  if( !ubi_trDups_OK( RootPtr ) || (ubi_trPARENT == whichway) )
    return( p );

  /* First, if needed, move up the tree.  We need to get to the root of the
   * subtree that contains all of the matching nodes.
   */
  q = p->Link[ubi_trPARENT];
  while( (NULL != q)
      && (ubi_trEQUAL == ubi_trAbNormal( (*(RootPtr->cmp))(FindMe, q) )) )
    {
    p = q;
    q = p->Link[ubi_trPARENT];
    }

  /* Next, move back down in the "whichway" direction. */
  q = p->Link[whichway];
  while( NULL != q )
    {
    q = qFind( RootPtr->cmp, FindMe, q );
    if( q )
      {
      p = q;
      q = p->Link[whichway];
      }
    }
  return( p );
  } /* Border */


/* ========================================================================== **
 * Exported utilities.
 */

long ubi_btSgn( register long x )
  /** Return the sign of x.
   *
   * @param   x   A signed long integer value.
   *
   * @returns The "sign" of x, represented as follows:
   *            -1 == negative
   *             0 == zero (no sign)
   *             1 == positive
   *
   *          {negative, zero, positive} ==> {-1, 0, 1}.
   *
   * \b Notes
   *  - This utility is provided in order to facilitate the conversion of C
   *    comparison function return values into BinTree direction values:
   *    {LEFT, PARENT, EQUAL}.
   *  - This function is called the #ubi_trAbNormal() conversion macro.
   *  - This is not defined as a macro to avoid evaluating \p x twice.
   */
  {
  return( x ? ((x>0)?1:-1) : 0 );
  } /* ubi_btSgn */

ubi_btNodePtr ubi_btInitNode( ubi_btNodePtr NodePtr )
  /** Initialize a tree node.
   *
   * @param   NodePtr   Pointer to a `ubi_btNode` structure to be initialized.
   *
   * @returns   A pointer to the initialized `ubi_btNode` structure (ie.
   *            the same as the input pointer).
   */
  {
  NodePtr->Link[ ubi_trLEFT ]   = NULL;
  NodePtr->Link[ ubi_trPARENT ] = NULL;
  NodePtr->Link[ ubi_trRIGHT ]  = NULL;
  NodePtr->gender               = ubi_trEQUAL;
  NodePtr->balance              = ubi_trEQUAL;
  return( NodePtr );
  } /* ubi_btInitNode */

ubi_btRootPtr ubi_btInitTree( ubi_btRootPtr   RootPtr,
                              ubi_btCompFunc  CompFunc,
                              char            Flags )
  /** Initialize the fields of a Tree Root header structure.
   *
   * @param   RootPtr   A pointer to an \c #ubi_btRoot structure to be
   *                    initialized.
   * @param   CompFunc  A pointer to a comparison function that will be used
   *                    whenever nodes in the tree must be compared against
   *                    outside values.
   * @param   Flags     One byte's worth of flags.  Flags include
   *                    \c #ubi_trOVERWRITE and \c #ubi_trDUPKEY.
   *
   * @returns   A pointer to the initialized \c #ubi_btRoot structure (ie.
   *            the same value as \p RootPtr).
   */
  {
  if( RootPtr )
    {
    RootPtr->root   = NULL;
    RootPtr->count  = 0L;
    RootPtr->cmp    = CompFunc;
    RootPtr->flags  = (Flags & ubi_trDUPKEY) ? ubi_trDUPKEY
                                             : (Flags & ubi_trOVERWRITE);
    }                 /* There are only two supported flags, and they are
                       * mutually exclusive.  ubi_trDUPKEY takes precedence
                       * over ubi_trOVERWRITE.
                       */
  return( RootPtr );
  } /* ubi_btInitTree */

ubi_trBool ubi_btInsert( ubi_btRootPtr  RootPtr,
                         ubi_btNodePtr  NewNode,
                         ubi_btItemPtr  ItemPtr,
                         ubi_btNodePtr *OldNode )
  /** Add a new element to the tree.
   *
   * @param   RootPtr   A pointer to the \c #ubi_btRoot structure that
   *                    indicates the root of the tree to which NewNode is to
   *                    be added.
   * @param   NewNode   A pointer to a `ubi_btNode` structure that is NOT
   *                    part of any tree.
   * @param   ItemPtr   A pointer to the sort key that is stored within
   *                    \p NewNode.  \p ItemPtr MUST point to information
   *                    stored in \p NewNode or an EXACT DUPLICATE.  The key
   *                    data indicated by \p ItemPtr is used to place the new
   *                    node into the tree.
   * @param   OldNode   A pointer to a \p #ubi_btNodePtr.  When searching
   *                    the tree, a duplicate node may be found.  If
   *                    duplicates are allowed, then the new node will be
   *                    simply placed into the tree.  If duplicates are
   *                    \b not allowed, however, then one of two things may
   *                    happen.
   *                    1.  If overwritting <b>is not</b> allowed, this
   *                        function will return FALSE (indicating that
   *                        the new node could not be inserted), and
   *                        \p OldNode will point to the duplicate that is
   *                        still in the tree.
   *                    2.  If overwritting \b is allowed, then this
   *                        function will swap \p OldNode for \p NewNode.
   *                        In this case, \p OldNode will point to the node
   *                        that was removed (thus allowing you to free
   *                        the node).
   *                    .
   *                    \b Notes:
   *                      - In overwrite mode, \e always check the return
   *                        value of this parameter.
   *                      - You may pass NULL in this parameter, the
   *                        function knows how to cope.  If you do this,
   *                        however, there will be no way to return a
   *                        pointer to an old (ie. replaced) node (which is
   *                        a problem if you are using overwrite mode).
   *
   * @returns   A boolean value indicating success or failure.
   *            - TRUE if the node was successfully added.
   *            - FALSE if the node could not be added to the tree.
   *              Such failure will only occur if duplicates are not allowed,
   *              nodes cannot be overwritten, AND a duplicate key was found
   *              within the tree.
   */
  {
  ubi_btNodePtr OtherP,
                parent = NULL;
  char          tmp;

  if( NULL == OldNode ) /* If they didn't give us a pointer, supply our own.  */
    OldNode = &OtherP;

  (void)ubi_btInitNode( NewNode );     /* Init the new node's BinTree fields. */

  /* Find a place for the new node. */
  *OldNode = TreeFind(ItemPtr, (RootPtr->root), &parent, &tmp, (RootPtr->cmp));

  /* Now add the node to the tree... */
  if( NULL == (*OldNode) )  /* The easy one: we have a space for a new node.  */
    {
    if( NULL == parent )
      RootPtr->root = NewNode;
    else
      {
      parent->Link[(int)tmp]      = NewNode;
      NewNode->Link[ubi_trPARENT] = parent;
      NewNode->gender             = tmp;
      }
    (RootPtr->count)++;
    return( ubi_trTRUE );
    }

  /* If we reach this point, we know that a duplicate node exists.  This
   * section adds the node to the tree if duplicate keys are allowed.
   */
  if( ubi_trDups_OK(RootPtr) )    /* Key exists, add duplicate */
    {
    ubi_btNodePtr q;

    tmp = ubi_trRIGHT;
    q = (*OldNode);
    *OldNode = NULL;
    while( NULL != q )
      {
      parent = q;
      if( tmp == ubi_trEQUAL )
        tmp = ubi_trRIGHT;
      q = q->Link[(int)tmp];
      if ( q )
        tmp = ubi_trAbNormal( (*(RootPtr->cmp))(ItemPtr, q) );
      }
    parent->Link[(int)tmp]       = NewNode;
    NewNode->Link[ubi_trPARENT]  = parent;
    NewNode->gender              = tmp;
    (RootPtr->count)++;
    return( ubi_trTRUE );
    }

  /* If we get to *this* point, we know that we are not allowed to have
   * duplicate nodes, but our node keys match, so... may we replace the
   * old one?
   */
  if( ubi_trOvwt_OK(RootPtr) )    /* Key exists, we replace */
    {
    if( NULL == parent )
      ReplaceNode( &(RootPtr->root), *OldNode, NewNode );
    else
      ReplaceNode( &(parent->Link[(int)((*OldNode)->gender)]),
                   *OldNode, NewNode );
    return( ubi_trTRUE );
    }

  return( ubi_trFALSE );      /* Failure: could not replace an existing node. */
  } /* ubi_btInsert */

ubi_btNodePtr ubi_btRemove( ubi_btRootPtr RootPtr,
                            ubi_btNodePtr DeadNode )
  /** Remove the indicated node from the tree.
   *
   * @param   RootPtr   A pointer to the header of the tree that contains
   *                    the node to be removed.
   * @param   DeadNode  A pointer to the node that will be removed.
   *
   * @returns   A pointer to the node that was removed from the tree (ie. the
   *            same as \p DeadNode).
   *
   * \b Notes
   *  - The node MUST be in the tree indicated by RootPtr.  If not,
   *    strange and evil things will happen to your trees.
   */
  {
  ubi_btNodePtr p,
               *parentp;
  int           tmp;

  /* If the node has both left and right subtrees, then we have to swap
   * it with another node.  The other node we choose will be the Prev()ious
   * node, which is guaranteed to have no RIGHT child.
   */
  if( (NULL != DeadNode->Link[ubi_trLEFT])
   && (NULL != DeadNode->Link[ubi_trRIGHT]) )
    SwapNodes( RootPtr, DeadNode, ubi_btPrev( DeadNode ) );

  /* The parent of the node to be deleted may be another node, or it may be
   * the root of the tree.  Since we're not sure, it's best just to have
   * a pointer to the parent pointer, whatever it is.
   */
  if( NULL == DeadNode->Link[ubi_trPARENT] )
    parentp = &( RootPtr->root );
  else
    parentp = &((DeadNode->Link[ubi_trPARENT])->Link[(int)(DeadNode->gender)]);

  /* Now link the parent to the only grand-child and patch up the gender. */
  tmp = ((DeadNode->Link[ubi_trLEFT])?ubi_trLEFT:ubi_trRIGHT);

  p = (DeadNode->Link[tmp]);
  if( NULL != p )
    {
    p->Link[ubi_trPARENT] = DeadNode->Link[ubi_trPARENT];
    p->gender       = DeadNode->gender;
    }
  (*parentp) = p;

  /* Finished, reduce the node count and return. */
  (RootPtr->count)--;
  return( DeadNode );
  } /* ubi_btRemove */

ubi_btNodePtr ubi_btLocate( ubi_btRootPtr RootPtr,
                            ubi_btItemPtr FindMe,
                            ubi_trCompOps CompOp )
  /** Locate a node that matches the given search criteria.
   *
   *  The purpose of \c #ubi_btLocate() is to find a node or set of nodes
   *  given a target value and a "comparison operator".  The Locate()
   *  function is more flexible and (in the case of trees that may contain
   *  dupicate keys) more precise than the \c #ubi_btFind() function.  The
   *  latter is faster, but it only searches for exact matches and, if the
   *  tree contains duplicates, Find() may return a pointer to any one of
   *  the duplicate-keyed entries.
   *
   * @param   RootPtr   A pointer to the header of the tree to be searched.
   * @param   FindMe    A \c #ubi_btItemPtr that indicates the key for which
   *                    to search.
   * @param   CompOp    One of the following:
   *
   *      CompOp     Return a pointer to the node with...
   *      --------   --------------------------------------------
   *      ubi_trLT - the last key value that is less than FindMe.
   *      ubi_trLE - the first key matching FindMe, or the last
   *                 key that is less than FindMe.
   *      ubi_trEQ - the first key matching FindMe.
   *      ubi_trGE - the first key matching FindMe, or the first
   *                 key greater than FindMe.
   *      ubi_trGT - the first key greater than FindMe.
   *
   * @returns   A pointer to the node matching the criteria listed above, or
   *            NULL if no node matched the criteria.
   *
   * \b Notes
   *  - In the case of trees with duplicate keys, Locate() will behave as
   *    follows:
   *
   *        Find:  3                       Find: 3
   *        Keys:  1 2 2 2 3 3 3 3 3 4 4   Keys: 1 1 2 2 2 4 4 5 5 5 6
   *                     ^ ^         ^                   ^ ^
   *                    LT EQ        GT                 LE GE
   *
   *    That is:
   *    - When returning a pointer to a node with a key that is LESS
   *      THAN the target key (FindMe), Locate() will return a pointer to the
   *      LAST matching node.
   *    - When returning a pointer to a node with a key that is GREATER
   *      THAN the target key (FindMe), Locate() will return a pointer to the
   *      FIRST matching node.
   *
   * @see #ubi_btFind(), #ubi_btFirstOf(), #ubi_btLastOf()
   */
  {
  register ubi_btNodePtr p;
  ubi_btNodePtr   parent;
  char            whichkid;

  /* Start by searching for a matching node. */
  p = TreeFind( FindMe,
                RootPtr->root,
                &parent,
                &whichkid,
                RootPtr->cmp );

  if( NULL != p )    /* If we have found a match, we can resolve as follows:  */
    {
    switch( CompOp )
      {
      case ubi_trLT:            /* It's just a jump to the left...  */
        p = Border( RootPtr, FindMe, p, ubi_trLEFT );
        return( Neighbor( p, ubi_trLEFT ) );
      case ubi_trGT:            /* ...and then a jump to the right. */
        p = Border( RootPtr, FindMe, p, ubi_trRIGHT );
        return( Neighbor( p, ubi_trRIGHT ) );
      default:
        p = Border( RootPtr, FindMe, p, ubi_trLEFT );
        return( p );
      }
    }

  /* Else, no match. */
  if( ubi_trEQ == CompOp )    /* If we were looking for an exact match... */
    return( NULL );           /* ...forget it.                            */

  /* We can still return a valid result for GT, GE, LE, and LT.
   * <parent> points to a node with a value that is either just before or
   * just after the target value.
   * Remaining possibilities are LT and GT (including LE & GE).
   */
  if( (ubi_trLT == CompOp) || (ubi_trLE == CompOp) )
    return( (ubi_trLEFT == whichkid) ? Neighbor( parent, whichkid ) : parent );
  else
    return( (ubi_trRIGHT == whichkid) ? Neighbor( parent, whichkid ) : parent );
  } /* ubi_btLocate */

ubi_btNodePtr ubi_btFind( ubi_btRootPtr RootPtr,
                          ubi_btItemPtr FindMe )
  /** Search the tree for a node matching the specified key.
   *
   *  Perform a non-recursive search of a tree for any node matching a
   *  specific key.
   *
   * @param   RootPtr   A pointer to the header of the tree to be searched.
   * @param   FindMe    A pointer to the key value for which to search.
   *
   * @returns   A pointer to a node with a key that matches the key indicated
   *            by \p FindMe, or NULL if no such node was found.
   *
   * \b Notes
   *  - In a tree that allows duplicates, the pointer returned <b>might
   *    not</b> point to the (sequentially) first occurance of the desired
   *    key.  In such a tree, it may be more useful to use \c #ubi_btLocate().
   */
  {
  return( qFind( RootPtr->cmp, FindMe, RootPtr->root ) );
  } /* ubi_btFind */

ubi_btNodePtr ubi_btNext( ubi_btNodePtr P )
  /** Return the next node in the tree.
   *
   *  Given the node indicated by \p P, find the (sorted order) Next node in
   *  the tree.
   *
   * @param   P   A pointer to a node that exists in a binary tree.
   *
   * @returns   A pointer to the "next" node in the tree, or NULL if \p P
   *            pointed to the "last" node in the tree (or was NULL).
   */
  {
  return( Neighbor( P, ubi_trRIGHT ) );
  } /* ubi_btNext */

ubi_btNodePtr ubi_btPrev( ubi_btNodePtr P )
  /** Return the previous node in the tree.
   *
   *  Given the node indicated by \p P, find the (sorted order) Previous node
   *  in the tree.
   *
   * @param   P   A pointer to a node that exists in a binary tree.
   *
   * @returns   A pointer to the "previous" node in the tree, or NULL if
   *            \p P pointed to the "first" node in the tree (or was NULL).
   */
  {
  return( Neighbor( P, ubi_trLEFT ) );
  } /* ubi_btPrev */

ubi_btNodePtr ubi_btFirst( ubi_btNodePtr P )
  /** Return the first node in the subtree of which \p P is the root.
   *
   *  Given the node indicated by \p P, find the (sorted order) First node in
   *  the subtree of which \p P is the root.
   *
   * @param   P   A pointer to a node that exists in a binary tree.
   *
   * @returns   A pointer to the "first" node in a subtree that has \p P as
   *            its root.  This function will return NULL only if \p P is
   *            NULL.
   *
   * \b Notes
   *  - In general, you will be passing in the value of the \c root field of
   *    a #ubi_btRoot structure.
   */
  {
  return( SubSlide( P, ubi_trLEFT ) );
  } /* ubi_btFirst */

ubi_btNodePtr ubi_btLast( ubi_btNodePtr P )
  /** Return the last node in the subtree of which \p P is the root.
   *
   *  Given the node indicated by \p P, find the (sorted order) Last node in
   *  the subtree of which \p P is the root.
   *
   * @param   P   A pointer to a node that exists in a binary tree.
   *
   * @returns   A pointer to the "last" node in a subtree that has \p P as
   *            its root.  This function will return NULL only if \p P is
   *            NULL.
   *
   * \b Notes
   *  - In general, you will be passing in the value of the \c root field
   *    of a #ubi_btRoot structure.
   */
  {
  return( SubSlide( P, ubi_trRIGHT ) );
  } /* ubi_btLast */

ubi_btNodePtr ubi_btFirstOf( ubi_btRootPtr RootPtr,
                             ubi_btItemPtr MatchMe,
                             ubi_btNodePtr p )
  /** Return the first of a set of nodes with matching keys.
   *
   *  Given a tree that a allows duplicate keys, and a pointer to a node in
   *  the tree, this function will return a pointer to the first (traversal
   *  order) node with the same key value.
   *
   * @param   RootPtr   A pointer to the root of the tree.
   * @param   MatchMe   A pointer to the key.  This must point to the key
   *                    within node \p p.
   * @param   p         A pointer to a node in the tree.
   *
   * @returns   A pointer to the first node in the set of nodes with keys
   *            matching \p MatchMe, or NULL on error.
   *
   * \b Notes
   *  - NULL (error) is returned only if the key indicated by \p MatchMe
   *    does not match the key included in \p p.
   */
  {
  /* If our starting point is invalid, return NULL. */
  if( (NULL == p)
   || (ubi_trEQUAL != ubi_trAbNormal( (*(RootPtr->cmp))( MatchMe, p ) )) )
    return( NULL );

  return( Border( RootPtr, MatchMe, p, ubi_trLEFT ) );
  } /* ubi_btFirstOf */

ubi_btNodePtr ubi_btLastOf( ubi_btRootPtr RootPtr,
                            ubi_btItemPtr MatchMe,
                            ubi_btNodePtr p )
  /** Return the last of a set of nodes with matching keys.
   *
   *  Given a tree that a allows duplicate keys, and a pointer to a node in
   *  the tree, this function will return a pointer to the last (traversal
   *  order) node with the same key value.
   *
   * @param   RootPtr   A pointer to the root of the tree.
   * @param   MatchMe   A pointer to the key.  This must point to the key
   *                    within node \p p.
   * @param   p         A pointer to a node in the tree.
   *
   * @returns   A pointer to the last node in the set of nodes with keys
   *            matching \p MatchMe, or NULL on error.
   *
   * \b Notes
   *  - NULL (error) is returned only if the key indicated by \p MatchMe
   *    does not match the key included in \p p.
   */
  {
  /* If our starting point is invalid, return NULL. */
  if( (NULL != p)
   || (ubi_trEQUAL != ubi_trAbNormal( (*(RootPtr->cmp))( MatchMe, p ) )) )
    return( NULL );

  return( Border( RootPtr, MatchMe, p, ubi_trRIGHT ) );
  } /* ubi_btLastOf */

unsigned long ubi_btTraverse( ubi_btRootPtr   RootPtr,
                              ubi_btActionRtn EachNode,
                              void           *UserData )
  /** Traverse the tree, calling the given function at each node.
   *
   *  Traverse a tree in sorted order (non-recursively).  At each node, call
   *  \p EachNode(), passing a pointer to the current node and \p UserData
   *  as the second parameter.
   *
   * @param   RootPtr   A pointer to a \p #ubi_btRoot structure that
   *                    indicates the tree to be traversed.
   * @param   EachNode  A pointer to a function to be called at each node
   *                    as the node is visited.
   * @param   UserData  A generic pointer to an unspecified object.
   *
   * @returns   A count of the number of nodes visited.  This will be zero
   *            iff the tree is empty.
   *
   * \b Notes
   *  - It is safe to delete nodes during a traversal using this function.
   *    In older versions, it was assumed that the call to *EachNode() would
   *    not disturb the tree, but there were several e'mails from people who
   *    were trying to delete or re-insert nodes during the traversal.  That
   *    didn't work, because the next node in the sequence was found \b after
   *    the call to the user-supplied function.  Recipe for disaster.\n\n
   *    Traversal now looks ahead to find the next node before it calls the
   *    user-supplied *EachNode() function, which is safer.
   */
  {
  ubi_btNodePtr p = ubi_btFirst( RootPtr->root );
  ubi_btNodePtr q;
  unsigned long count = 0;

  while( NULL != p )
    {
    q = ubi_btNext( p );
    (*EachNode)( p, UserData );
    count++;
    p = q;
    }
  return( count );
  } /* ubi_btTraverse */

unsigned long ubi_btKillTree( ubi_btRootPtr     RootPtr,
                              ubi_btKillNodeRtn FreeNode )
  /** Delete and free all nodes in the given tree.
   *
   *  Delete an entire tree (non-recursively) and reinitialize the
   *  \c #ubi_btRoot structure.  Return a count of the number of nodes
   *  deleted.
   *
   * @param   RootPtr   A pointer to a \c #ubi_btRoot structure that
   *                    indicates the root of the tree to delete.
   * @param   FreeNode  A function that will be called for each node in
   *                    the tree to deallocate the memory used by the node.
   *
   * @returns   The number of nodes removed from the tree.\n
   *            Zero (0) is returned if:
   *            - The tree is empty (actually contains 0 entries).
   *            - \p RootPtr is NULL, in which case the tree is considered
   *              empty.
   *            - \p FreeNode is NULL, in which case entries cannot be
   *              removed, so 0 is returned.
   *            .
   *            In all other cases, the return value should be equal to the
   *            initial value of \c RootPtr->count.
   *
   * \b Notes:
   *  - This function maintains the consistency of the tree as it removes
   *    nodes.  The goal is to prevent memory leakage.
   *  - The function indicated by \p FreeNode is responsible for freeing or
   *    otherwise cleaning up the nodes as they are removed.
   */
  {
  ubi_btNodePtr p, q;
  unsigned long count = 0;

  if( (NULL == RootPtr) || (NULL == FreeNode) )
    return( 0 );

  p = ubi_btFirst( RootPtr->root );
  while( NULL != p )
    {
    q = p;
    while( q->Link[ubi_trRIGHT] )
      q = SubSlide( q->Link[ubi_trRIGHT], ubi_trLEFT );
    p = q->Link[ubi_trPARENT];
    if( NULL != p )
      p->Link[ ((p->Link[ubi_trLEFT] == q)?ubi_trLEFT:ubi_trRIGHT) ] = NULL;
    (*FreeNode)((void *)q);
    count++;
    }

  /* overkill... */
  (void)ubi_btInitTree( RootPtr,
                        RootPtr->cmp,
                        RootPtr->flags );
  return( count );
  } /* ubi_btKillTree */

ubi_btNodePtr ubi_btLeafNode( ubi_btNodePtr leader )
  /** Return a pointer to a leaf node.
   *
   * @param   leader  Pointer to a node at which to start the descent.
   *
   * @returns   A pointer to a leaf node, selected in a somewhat arbitrary
   *            manner but with an effort to go deep.
   *
   *            This function returns NULL iff \p leader is NULL.
   *
   * \b Notes
   *  - This function exists primarily because of the \c ubi_Cache module,
   *    which uses a splay tree to maintain a simple in-memory key->value
   *    cache.  The cache may have a maximum entry count and/or a maximum
   *    memory usage limitation, so there needs to be a way of choosing a
   *    node to sacrifice if the cache becomes full.  In a splay tree, less
   *    recently accessed nodes tend toward the bottom of the tree, meaning
   *    that leaf nodes are very good candidates for removal.
   *
   *    Unfortunately, it's not enough to take a single path to the bottom
   *    of the tree to find a candidate for removal.  Splay trees are
   *    "mostly" balanced, but not completely balanced, and there is a
   *    possibility that the path down the tree will find a leaf node that
   *    is very near the top, even in a full-ish tree.
   *    Not good.
   *
   *    This function mitigates the problem by following multiple paths down
   *    the tree.  Traversing the whole tree would be too slow, so we
   *    traverse a limited number of paths, returning the leaf node at the
   *    bottom of the longest path we find.
   *
   *  - In a simple binary tree, or in an AVL tree, the most recently added
   *    nodes tend to be nearer the bottom.  A cache based on one of those
   *    tree types should probably be trimmed by removing the root node.
   *
   *  - Randomizing the traversal order is probably a good idea.  You can
   *    improve the randomization of leaf node selection by passing in
   *    pointers to non-root nodes.  A pointer to any node in the tree will
   *    do.  Of course, if you pass a pointer to a leaf node you'll get the
   *    same thing back.
   */
  {
  #define MAXPATHS 4  /* Set higher for more maximum paths, lower for fewer.  */
  ubi_trNodePtr p[MAXPATHS];
  ubi_trNodePtr q[MAXPATHS];
  int           whichway;
  int           paths;
  int           i, j;

  /* If the subtree is empty, return NULL.
   */
  if( NULL == leader )
    return( NULL );

  /* Initialize:
   *  The <p[]> array starts with a pointer to the one node we've been given.
   *  Since <p[]> starts with only a single node, the <paths> count is 1.
   *  Start by looking left when going down the tree.  We'll switch back
   *  and forth as we wander downward.
   */
  p[0]     = leader;
  paths    = 1;
  whichway = ubi_trLEFT;

  /* Climb down the tree. */
  while( paths > 0 )
    {
    /* Copy the <p> array into the <q> array. */
    for( i = 0; i < paths; i++ )
      q[i] = p[i];

    /* Now try to fill the <p> array from the children of the <q> nodes.  */
    for( i = j = 0; (i < paths); i++ )
      {
      if( NULL != q[i]->Link[whichway] )
        p[j++] = q[i]->Link[whichway];
      }
    /* Reverse direction and fill in any missing <p> array slots. */
    whichway = ubi_trRevWay( whichway );
    for( i = 0; (i < paths) && (j < (MAXPATHS-1)); i++ )
      {
      if( NULL != q[i]->Link[whichway] )
        p[j++] = q[i]->Link[whichway];
      }
    paths = j;
    }

  return( q[0] );
  } /* ubi_btLeafNode */

int ubi_btModuleID( int size, char *list[] )
  /** Return a set of strings that identify the module.
   *
   * @param   size  The number of elements in the array \p list.
   * @param   list  An array of pointers of type (char *).  The function
   *                will fill in the array with pointers to strings.
   *
   * @returns   The number of elements of \p list that were used.
   *
   * \b Notes
   *  - The indicated strings are stored in static space and must not be
   *    freed or overwritten.
   *  - If there are unused entries available in \p list, the first unused
   *    entry will be set to NULL.
   */
  {
  if( size > 0 )
    {
    list[0] = ModuleID;
    if( size > 1 )
      list[1] = NULL;
    return( 1 );
    }
  return( 0 );
  } /* ubi_btModuleID */


/* ========================================================================== */
