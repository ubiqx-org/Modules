#ifndef UBI_AVLTREE_H
#define UBI_AVLTREE_H
/* ========================================================================== **
 *                              ubi_AVLtree.h
 *
 *  Copyright (C) 1990-1998, 2020 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *
 *  This module provides an implementation of AVL height balanced binary
 *  trees.  (Adelson-Velsky, Landis 1962)
 *
 *  This header file contains the basic AVL structure and pointer typedefs
 *  as well as the prototypes needed to access the functions in the AVL
 *  module ubi_AVLtree.  The .c file implements the low-level height balancing
 *  routines that manage the AVL tree, plus all of the basic primops for
 *  adding, searching for, and deleting nodes.
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
 * $Id: ubi_AVLtree.h; 2020-09-23 19:59:08 -0500; crh$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 4.5  2000/01/31 05:21:17  crh
 * Made checks for NULL more explicit in a few places.
 *
 * Revision 4.4  1998/06/04 21:29:27  crh
 * Upper-cased defined constants (eg UBI_BINTREE_H) in some header files.
 * This is more "standard", and is what people expect.  Weird, eh?
 *
 * Revision 4.3  1998/06/03 17:41:46  crh
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
 * Revision 4.2  1998/06/02 01:26:53  crh
 * Changed ubi_null.h to sys_include.h to make it more generic.
 *
 * Revision 4.1  1998/05/20 04:35:50  crh
 * The C file now includes ubi_null.h.  See ubi_null.h for more info.
 *
 * Revision 4.0  1998/03/10 03:34:45  crh
 * Major changes.
 * By adding the AVL balance field to the base ubi_btNode structure, I no
 * longer need AVL-specific ReplaceNode(), SwapNodes(), and InitNode()
 * functions.  The Remove() function is also simplified.  It's all much
 * cleaner.
 * This is rev. 4.0.  The 3.x series was dropped.
 *
 * Revision 2.5  1997/12/23 04:00:15  crh
 * In this version, all constants & macros defined in the header file have
 * the ubi_tr prefix.  Also cleaned up anything that gcc complained about
 * when run with '-pedantic -fsyntax-only -Wall'.
 *
 * Revision 2.4  1997/07/26 04:36:23  crh
 * Andrew Leppard, aka "Grazgur", discovered that I still had my brains tied
 * on backwards with respect to node deletion.  I did some more digging and
 * discovered that I was not changing the balance values correctly in the
 * single rotation functions.  Double rotation was working correctly because
 * the formula for changing the balance values is the same for insertion or
 * deletion.  Not so for single rotation.
 *
 * I have tested the fix by loading the tree with over 44 thousand names,
 * deleting 2,629 of them (all those in which the second character is 'u')
 * and then walking the tree recursively to verify that the balance factor of
 * each node is correct.  Passed.
 *
 * Thanks Andrew!
 *
 * Also:
 * + Changed ubi_TRUE and ubi_FALSE to ubi_trTRUE and ubi_trFALSE.
 * + Rewrote the ubi_tr<func> macros because they weren't doing what I'd
 *   hoped they would do (see the bottom of the header file).  They work now.
 *
 * Revision 2.3  1997/06/03 05:22:07  crh
 * Changed TRUE and FALSE to ubi_TRUE and ubi_FALSE to avoid causing
 * problems.
 *
 * Revision 2.2  1995/10/03 22:15:47  CRH
 * Ubisized!
 *
 * Revision 2.1  95/03/09  23:46:44  CRH
 * Added the ModuleID static string and function.  These modules are now
 * self-identifying.
 *
 * Revision 2.0  95/03/05  14:11:22  CRH
 * This revision of ubi_AVLtree coincides with revision 2.0 of ubi_BinTree,
 * and so includes all of the changes to that module.  In addition, a bug in
 * the node deletion process has been fixed.
 *
 * After rewriting the Locate() function in ubi_BinTree, I decided that it was
 * time to overhaul this module.  In the process, I discovered a bug related
 * to node deletion.  To fix the bug, I wrote function Debalance().  A quick
 * glance will show that it is very similar to the Rebalance() function.  In
 * previous versions of this module, I tried to include the functionality of
 * Debalance() within Rebalance(), with poor results.
 *
 * Revision 1.0  93/10/15  22:58:48  CRH
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
 *  V0.0 - May, 1990   -  Written by Christopher R. Hertel (CRH).
 *
 *  ========================================================================= **
 *//**
 * @file    ubi_AVLtree.h
 * @author  Christopher R. Hertel
 * @brief   AVL Height Balanced Tree implementation.
 * @date    May 1990
 * @version \$Id: ubi_AVLtree.h; 2020-09-23 19:59:08 -0500; crh$
 * @copyright Copyright (C) 1990-1998, 2014, 2020 by Christopher R. Hertel
 *
 * @details
 *  This AVL Tree implementation is descendent from the ubi_BinTree
 *  implementation of simple (unbalanced) binary trees.  Including the
 *  \c "ubi_AVLtree.h" header provides access to the AVL balancing
 *  functionality.
 *
 * @see https://en.wikipedia.org/wiki/AVL_tree
 */

#include "ubi_BinTree.h"   /* Base binary tree support. */


/* -------------------------------------------------------------------------- **
 *  Function prototypes.
 * -------------------------------------------------------------------------- **
 */

ubi_trBool ubi_avlInsert( ubi_btRootPtr  RootPtr,
                          ubi_btNodePtr  NewNode,
                          ubi_btItemPtr  ItemPtr,
                          ubi_btNodePtr *OldNode );

ubi_btNodePtr ubi_avlRemove( ubi_btRootPtr RootPtr,
                             ubi_btNodePtr DeadNode );

int ubi_avlModuleID( int size, char *list[] );


/* -------------------------------------------------------------------------- **
 * Masquarade...
 *
 * This set of defines allows you to write programs that will use any of the
 * implemented binary tree modules (currently BinTree, AVLtree, and SplayTree).
 * Instead of using ubi_avl... or ubi_bt, use ubi_tr... and select the tree
 * type by including the appropriate module header.
 *//**
 * @def   ubi_trInsert
 * @brief Alias for #ubi_avlInsert()
 *
 * @def   ubi_trRemove
 * @brief Alias for #ubi_avlRemove()
 *
 * @def   ubi_trModuleID
 * @brief Alias for #ubi_avlModuleID()
 */

#undef ubi_trInsert
#define ubi_trInsert( Rp, Nn, Ip, On ) \
        ubi_avlInsert( (ubi_btRootPtr)(Rp), (ubi_btNodePtr)(Nn), \
                       (ubi_btItemPtr)(Ip), (ubi_btNodePtr *)(On) )

#undef ubi_trRemove
#define ubi_trRemove( Rp, Dn ) \
        ubi_avlRemove( (ubi_btRootPtr)(Rp), (ubi_btNodePtr)(Dn) )

#undef ubi_trModuleID
#define ubi_trModuleID( s, l ) ubi_avlModuleID( s, l )


/* =========================== End  ubi_AVLtree.h =========================== */
#endif /* UBI_AVLTREE_H */
