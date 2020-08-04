/* ========================================================================== **
 *                              ubi_SplayTree.c
 *
 *  Copyright (C) 1993-1998 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *
 *  This module implements "splay" trees.  Splay trees are binary trees
 *  that are rearranged (splayed) whenever a node is accessed.  The
 *  splaying process *tends* to make the tree bushier (improves balance),
 *  and the nodes that are accessed most frequently *tend* to be closer to
 *  the top.
 *
 *  References: "Self-Adjusting Binary Search Trees", by Daniel Sleator and
 *              Robert Tarjan.  Journal of the Association for Computing
 *              Machinery Vol 32, No. 3, July 1985 pp. 652-686
 *
 *    See also: http://www.cs.cmu.edu/~sleator/
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
 * $Id: ubi_SplayTree.c; 2020-08-04 12:52:18 -0500; Christopher R. Hertel$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 4.5  2000/01/08 23:26:49  crh
 * Added ubi_trSplay() macro, which does a type cast for us.
 *
 * Revision 4.4  1998/06/04 21:29:27  crh
 * Upper-cased defined constants (eg UBI_BINTREE_H) in some header files.
 * This is more "standard", and is what people expect.  Weird, eh?
 *
 * Revision 4.3  1998/06/03 17:45:05  crh
 * Further fiddling with sys_include.h.  It's now in ubi_BinTree.h which is
 * included by all of the binary tree files.
 *
 * Also fixed some warnings produced by lint on Irix 6.2, which doesn't seem
 * to like syntax like this:
 *
 *   if( (a = b) )
 *
 * The fix was to change lines like the above to:
 *
 *   if( 0 != (a=b) )
 *
 * Which means the same thing.
 *
 * Reminder: Some of the ubi_tr* macros in ubi_BinTree.h are redefined in
 *           ubi_AVLtree.h and ubi_SplayTree.h.  This allows easy swapping
 *           of tree types by simply changing a header.  Unfortunately, the
 *           macro redefinitions in ubi_AVLtree.h and ubi_SplayTree.h will
 *           conflict if used together.  You must either choose a single tree
 *           type, or use the underlying function calls directly.  Compare
 *           the two header files for more information.
 *
 * Revision 4.2  1998/06/02 01:29:14  crh
 * Changed ubi_null.h to sys_include.h to make it more generic.
 *
 * Revision 4.1  1998/05/20 04:37:54  crh
 * The C file now includes ubi_null.h.  See ubi_null.h for more info.
 *
 * Revision 4.0  1998/03/10 03:41:33  crh
 * Minor comment changes.  The revision number is now 4.0 to match the
 * BinTree and AVLtree modules.
 *
 * Revision 2.7  1998/01/24 06:37:08  crh
 * Added a URL for more information.
 *
 * Revision 2.6  1997/12/23 04:01:12  crh
 * In this version, all constants & macros defined in the header file have
 * the ubi_tr prefix.  Also cleaned up anything that gcc complained about
 * when run with '-pedantic -fsyntax-only -Wall'.
 *
 * Revision 2.5  1997/07/26 04:15:42  crh
 * + Cleaned up a few minor syntax annoyances that gcc discovered for me.
 * + Changed ubi_TRUE and ubi_FALSE to ubi_trTRUE and ubi_trFALSE.
 *
 * Revision 2.4  1997/06/03 04:42:21  crh
 * Changed TRUE and FALSE to ubi_TRUE and ubi_FALSE to avoid causing
 * problems.
 *
 * Revision 2.3  1995/10/03 22:19:07  CRH
 * Ubisized!
 * Also, added the function ubi_sptSplay().
 *
 * Revision 2.1  95/03/09  23:54:42  CRH
 * Added the ModuleID static string and function.  These modules are now
 * self-identifying.
 *
 * Revision 2.0  95/02/27  22:34:46  CRH
 * This module was updated to match the interface changes made to the
 * ubi_BinTree module.  In particular, the interface to the Locate() function
 * has changed.  See ubi_BinTree for more information on changes and new
 * functions.
 *
 * The revision number was also upped to match ubi_BinTree.
 *
 * Revision 1.1  93/10/18  20:35:16  CRH
 * I removed the hard-coded logical device names from the include file
 * specifications.  CRH
 *
 * Revision 1.0  93/10/15  23:00:15  CRH
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
 * Revision 0.1  93/04/25  22:03:32  CRH
 * Simply changed the <exec/types.h> #include reference the .c file to
 * use <stdlib.h> instead.  The latter is portable, the former is not.
 *
 * Revision 0.0  93/04/21  23:05:52  CRH
 * Initial version, written by Christopher R. Hertel.
 * This module implements Splay Trees using the ubi_BinTree module as a basis.
 *
 * ========================================================================== **
 */

#include "ubi_SplayTree.h"  /* Header for THIS module.   */

/* ========================================================================== **
 * Static data.
 */

static char ModuleID[] =
  "$Id: ubi_SplayTree.c; 2020-08-04 12:52:18 -0500; Christopher R. Hertel$\n";


/* ========================================================================== **
 * Private functions...
 */

static void Rotate( ubi_btNodePtr p )
  /* ------------------------------------------------------------------------ **
   * This function performs a single rotation, moving node *p up one level
   * in the tree.
   *
   *  Input:    p - a pointer to an ubi_btNode in a tree.
   *
   *  Output:   None.
   *
   *  Notes:    This implements a single rotation in either direction (left
   *            or right).  This is the basic building block of all splay
   *            tree rotations.
   * ------------------------------------------------------------------------ **
   */
  {
  ubi_btNodePtr parentp;
  ubi_btNodePtr tmp;
  char          way;
  char          revway;

  parentp = p->Link[ubi_trPARENT];    /* Find parent. */

  if( parentp )                 /* If no parent, then we're already the root. */
    {
    way    = p->gender;
    revway = ubi_trRevWay(way);
    tmp    = p->Link[(int)revway];

    parentp->Link[(int)way] = tmp;
    if( tmp )
      {
      tmp->Link[ubi_trPARENT] = parentp;
      tmp->gender             = way;
      }

    tmp                   = parentp->Link[ubi_trPARENT];
    p->Link[ubi_trPARENT] = tmp;
    p->gender             = parentp->gender;
    if( tmp )
      tmp->Link[(int)(p->gender)] = p;

    parentp->Link[ubi_trPARENT] = p;
    parentp->gender             = revway;
    p->Link[(int)revway]        = parentp;
    }
  } /* Rotate */

static ubi_btNodePtr Splay( ubi_btNodePtr SplayWithMe )
  /* ------------------------------------------------------------------------ **
   * Move the node indicated by SplayWithMe to the root of the tree by
   * splaying the tree.
   *
   *  Input:  SplayWithMe - A pointer to an ubi_btNode within a tree.
   *
   *  Output: A pointer to the root of the splay tree (i.e., the same as
   *          SplayWithMe).
   * ------------------------------------------------------------------------ **
   */
  {
  ubi_btNodePtr parent;

  while( NULL != (parent = SplayWithMe->Link[ubi_trPARENT]) )
    {
    if( parent->gender == SplayWithMe->gender )       /* Zig-Zig */
      Rotate( parent );
    else
      {
      if( ubi_trEQUAL != parent->gender )             /* Zig-Zag */
        Rotate( SplayWithMe );
      }
    Rotate( SplayWithMe );                            /* Zig */
    } /* while */
  return( SplayWithMe );
  } /* Splay */

/* ========================================================================== **
 * Exported utilities.
 */

ubi_trBool ubi_sptInsert( ubi_btRootPtr  RootPtr,
                          ubi_btNodePtr  NewNode,
                          ubi_btItemPtr  ItemPtr,
                          ubi_btNodePtr *OldNode )
  /**
   * @copydoc ubi_BinTree.h::ubi_btInsert()
   * @details After the node is added, the tree is splay-rebalanced from
   *          the newly added node.
   * @see #ubi_sptSplay()
   */
  {
  ubi_btNodePtr OtherP;

  if( !(OldNode) )
    OldNode = &OtherP;

  if( ubi_btInsert( RootPtr, NewNode, ItemPtr, OldNode ) )
    {
    RootPtr->root = Splay( NewNode );
    return( ubi_trTRUE );
    }

  /* Splay the unreplacable, duplicate keyed, unique, old node. */
  RootPtr->root = Splay( (*OldNode) );
  return( ubi_trFALSE );
  } /* ubi_sptInsert */

ubi_btNodePtr ubi_sptRemove( ubi_btRootPtr RootPtr, ubi_btNodePtr DeadNode )
  /**
   * @copydoc ubi_BinTree.h::ubi_btRemove()
   * @details After the node is removed, the tree is splay-rebalanced.
   * @see #ubi_sptSplay()
   */
  {
  ubi_btNodePtr p;

  (void)Splay( DeadNode );                  /* Move dead node to root.        */
  if( NULL != (p = DeadNode->Link[ubi_trLEFT]) )
    {                                       /* If left subtree exists...      */
    ubi_btNodePtr q = DeadNode->Link[ubi_trRIGHT];

    p->Link[ubi_trPARENT] = NULL;           /* Left subtree node becomes root.*/
    p->gender             = ubi_trPARENT;
    p                     = ubi_btLast( p );  /* Find rightmost left node...  */
    p->Link[ubi_trRIGHT]  = q;                /* ...attach right tree.        */
    if( q )
      q->Link[ubi_trPARENT] = p;
    RootPtr->root   = Splay( p );           /* Resplay at p.                  */
    }
  else
    {
    if( NULL != (p = DeadNode->Link[ubi_trRIGHT]) )
      {                               /* No left, but right subtree exists... */
      p->Link[ubi_trPARENT] = NULL;         /* Right subtree root becomes...  */
      p->gender       = ubi_trPARENT;       /* ...overall tree root.          */
      RootPtr->root   = p;
      }
    else
      RootPtr->root = NULL;                 /* No subtrees => empty tree.     */
    }

  (RootPtr->count)--;                       /* Decrement node count.          */
  return( DeadNode );                       /* Return pointer to pruned node. */
  } /* ubi_sptRemove */

ubi_btNodePtr ubi_sptLocate( ubi_btRootPtr RootPtr,
                             ubi_btItemPtr FindMe,
                             ubi_trCompOps CompOp )
  /**
   * @copydoc ubi_BinTree.h::ubi_btLocate()
   * @details If the node is located, the tree is splay-rebalanced from the
   *          located node.
   * @see #ubi_sptSplay()
   */
  {
  ubi_btNodePtr p;

  p = ubi_btLocate( RootPtr, FindMe, CompOp );
  if( p )
    RootPtr->root = Splay( p );
  return( p );
  } /* ubi_sptLocate */

ubi_btNodePtr ubi_sptFind( ubi_btRootPtr RootPtr,
                           ubi_btItemPtr FindMe )
  /**
   * @copydoc ubi_BinTree.h::ubi_btFind()
   * @details If the node is found, the tree is splay-rebalanced from the
   *          node that was found.
   * @see #ubi_sptSplay()
   */
  {
  ubi_btNodePtr p;

  p = ubi_btFind( RootPtr, FindMe );
  if( p )
    RootPtr->root = Splay( p );
  return( p );
  } /* ubi_sptFind */

void ubi_sptSplay( ubi_btRootPtr RootPtr,
                   ubi_btNodePtr SplayMe )
  /** Splay the tree from the given node.
   *
   *  This function allows you to splay the tree at a given node, thus
   *  moving the node to the top of the tree and moving it toward balance
   *  in the process.
   *
   * @param   RootPtr   A pointer to the header of the tree to be splayed.
   * @param   SplayMe   A pointer to a node within the tree.  This will
   *                    become the new root node.
   * @returns None (<tt>void</tt>).
   *
   * \b Notes
   *  - This is an uncharacteristic function for this group of modules in
   *    in that it provides access to the internal balancing routines,
   *    which would normally be hidden. \n
   *    Splaying the tree will not damage it (assuming that I've done
   *    \b my job), but there is overhead involved.
   *  - This function should only be used directly if you understand the
   *    underlying Splay Tree principles involved.
   */
  {
  RootPtr->root = Splay( SplayMe );
  } /* ubi_sptSplay */

int ubi_sptModuleID( int size, char *list[] )
  /**
   * @copydoc ubi_BinTree.h::ubi_btModuleID()
   */
  {
  if( size > 0 )
    {
    list[0] = ModuleID;
    if( size > 1 )
      return( 1 + ubi_btModuleID( --size, &(list[1]) ) );
    return( 1 );
    }
  return( 0 );
  } /* ubi_sptModuleID */

/* ================================ The End ================================= */
