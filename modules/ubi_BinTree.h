#ifndef UBI_BINTREE_H
#define UBI_BINTREE_H
/* ========================================================================== **
 *                              ubi_BinTree.h
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
 * $Id: ubi_BinTree.h; 2020-09-23 19:59:08 -0500; crh$
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
 * Revision 4.7  1998/10/21 06:15:07  crh
 * Fixed bugs in FirstOf() and LastOf() reported by Massimo Campostrini.
 * See function comments.
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
 * Revision 4.1  1998/03/31 06:13:47  crh
 * Thomas Aglassinger sent E'mail pointing out errors in the
 * dereferencing of function pointers, and a missing typecast.
 * Thanks, Thomas!
 *
 * Revision 4.0  1998/03/10 03:16:04  crh
 * Added the AVL field 'balance' to the ubi_btNode structure.  This means
 * that all BinTree modules now use the same basic node structure, which
 * greatly simplifies the AVL module.
 * Decided that this was a big enough change to justify a new major revision
 * number.  3.0 was an error, so we're at 4.0.
 *
 * Revision 2.6  1998/01/24 06:27:30  crh
 * Added ubi_trCount() macro.
 *
 * Revision 2.5  1997/12/23 03:59:21  crh
 * In this version, all constants & macros defined in the header file have
 * the ubi_tr prefix.  Also cleaned up anything that gcc complained about
 * when run with '-pedantic -fsyntax-only -Wall'.
 *
 * Revision 2.4  1997/07/26 04:11:14  crh
 * + Just to be annoying I changed ubi_TRUE and ubi_FALSE to ubi_trTRUE
 *   and ubi_trFALSE.
 * + There is now a type ubi_trBool to go with ubi_trTRUE and ubi_trFALSE.
 * + There used to be something called "ubi_TypeDefs.h".  I got rid of it.
 * + Added function ubi_btLeafNode().
 *
 * Revision 2.3  1997/06/03 05:15:27  crh
 * Changed TRUE and FALSE to ubi_TRUE and ubi_FALSE to avoid conflicts.
 * Also changed the interface to function InitTree().  See the comments
 * for this function for more information.
 *
 * Revision 2.2  1995/10/03 22:00:40  CRH
 * Ubisized!
 *
 * Revision 2.1  95/03/09  23:43:46  CRH
 * Added the ModuleID static string and function.  These modules are now
 * self-identifying.
 *
 * Revision 2.0  95/02/27  22:00:33  CRH
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
 * Revision 1.0  93/10/15  22:55:04  CRH
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
 *//**
 * @file    ubi_BinTree.h
 * @author  Christopher R. Hertel
 * @brief   Basic Binary Tree implementation.
 * @date    June 1991
 * @version \$Id: ubi_BinTree.h; 2020-09-23 19:59:08 -0500; crh$
 * @copyright Copyright (C) 1991-1998, 2014, 2020 by Christopher R. Hertel
 *
 * @details
 *  This module implements a simple unbalanced binary tree.
 *
 *  Binary trees are an in-memory data structure that can be used to
 *  maintain a sorted-order collection of key->value pairs.  Key-based
 *  lookup of individual entries is relatively fast, particularly if the
 *  tree is balanced.
 *
 *  This implementation avoids recursion by using a "parent pointer" to
 *  point to the parent node in the tree.  This is in addition to the left
 *  and right child pointers.
 */

#include "sys_include.h"  /* Global include file, used to adapt the ubiqx
                           * modules to the host environment and the project
                           * with which the modules will be used.  See
                           * sys_include.h for more info.
                           */

/* -------------------------------------------------------------------------- **
 * Defined Constants.
 *//**
 * @def     ubi_trTRUE
 * @brief   Boolean TRUE.
 * @details The \c stdbool.h header was not standard when these modules were
 *          originally written, so it was useful to create our own boolean
 *          values.
 *
 * @def     ubi_trFALSE
 * @brief   Boolean FALSE.
 * @details The \c stdbool.h header was not standard when these modules were
 *          originally written, so it was useful to create our own boolean
 *          values.
 *
 * @def     ubi_trOVERWRITE
 * @brief   If set, allow overwrite of an existing entry with the same key.
 * @details If this flag is set in the tree header, an existing node may be
 *          overwritten by a new node with a matching key.
 *
 * @def     ubi_trDUPKEY
 * @brief   If set, allow duplicate keys in the tree.
 * @details If this flag is set in the tree header, the tree will allow
 *          duplicate keys.  The \c #ubi_trOVERWRITE flag is ignored if
 *          duplicate keys are permitted.
 *
 * @def     ubi_trLEFT
 * @brief   Left child pointer.
 * @details Each node has three pointers: left child, parent, and right
 *          child.  These are kept in an array, and these constants help
 *          to make it clear which pointer is being accessed.
 *
 * @def     ubi_trPARENT
 * @brief   Parent pointer.
 * @details Each node has three pointers: left child, parent, and right
 *          child.  These are kept in an array, and these constants help
 *          to make it clear which pointer is being accessed.
 *
 * @def     ubi_trRIGHT
 * @brief   Right child pointer.
 * @details Each node has three pointers: left child, parent, and right
 *          child.  These are kept in an array, and these constants help
 *          to make it clear which pointer is being accessed.
 *
 * @def     ubi_trEQUAL
 * @brief   Synonym for \c #ubi_trPARENT.
 *
 * @enum  ubi_trCompOps
 * @brief This set of values is used to specify a search type in
 *        \c #ubi_trLocate().
 * @var   ubi_trLT
 *        Request the first instance of the greatest key less than the search
 *        search key.
 * @var   ubi_trLE
 *        Request the first instance of the greatest key less than or equal
 *        to the search key.
 * @var   ubi_trEQ
 *        Request the first instance of the search key.
 * @var   ubi_trGE
 *        Request the first instance of the first key that is greater than
 *        or equal to the search key.
 * @var   ubi_trGT
 *        Request the first instance of the first key that is greater than
 *        the search key.
 */

/* Boolean values.  */
#define ubi_trTRUE  0xFF
#define ubi_trFALSE 0x00

/* Flags for managing duplicate keys within a tree. */
#define ubi_trOVERWRITE 0x01
#define ubi_trDUPKEY    0x02

/* Pointer array index constants. */
#define ubi_trLEFT   0x00
#define ubi_trPARENT 0x01
#define ubi_trRIGHT  0x02
#define ubi_trEQUAL  ubi_trPARENT

/* Used for specifying the search type in ubi_trLocate(). */
typedef enum
  {
  ubi_trLT = 1,
  ubi_trLE,
  ubi_trEQ,
  ubi_trGE,
  ubi_trGT
  } ubi_trCompOps;


/* -------------------------------------------------------------------------- **
 * Macros.
 */

/**
 * These three macros allow simple manipulation of pointer index values (LEFT,
 * RIGHT, and PARENT).
 */

/**
 * @def     ubi_trNormalize( W )
 * @param   W   One of {\c #ubi_trLEFT, \c #ubi_trPARENT, \c #ubi_trRIGHT}.
 *              The \p W stands for "way" (as in which \b way to go).
 * @returns One of {-1, 0, 1}.
 * @brief   Converts {LEFT, PARENT, RIGHT} into {-1, 0 ,1}.
 * @details Many standard C functions use {-1, 0, 1} to indicate less than,
 *          equal to, and greater than.  Arrays, however, are typically
 *          indexed using unsigned integers.  Yes, you can fudge this but
 *          it's kinda ugly and this normalization does effectively the
 *          same thing.
 * @hideinitializer
 */
#define ubi_trNormalize(W) ((char)( (W) - ubi_trEQUAL ))


/**
 * @def     ubi_trAbNormal( W )
 * @param   W   A signed integer value.
 * @returns One of {\c #ubi_trLEFT, \c #ubi_trPARENT, \c #ubi_trRIGHT}.
 * @brief   Returns a directional value to indicate the sign of the input.
 * @details This macro converts the signed integer to a Left, Parent, or
 *          Right value so that the correct node pointer can be accessed.
 *          - Negative input returns \c #ubi_trLEFT.
 *          - Positive input returns \c #ubi_trRIGHT.
 *          - Zero (0) returns \c #ubi_trPARENT.
 *          .
 *          This is the opposite of \c #ubi_trNormalize().
 * @see #ubi_trNormalize()
 * @see #ubi_btSgn()
 * @hideinitializer
 */
#define ubi_trAbNormal(W)  ((char)( ((char)ubi_btSgn( (long)(W) )) \
                                         + ubi_trEQUAL ))

/**
 * @def     ubi_trRevWay( W )
 * @param   W   One of {\c #ubi_trLEFT, \c #ubi_trPARENT, \c #ubi_trRIGHT}.
 * @returns One of {\c #ubi_trRIGHT, \c #ubi_trPARENT, \c #ubi_trLEFT}.
 * @brief   Reverse the input direction.
 * @details Converts \c #ubi_trLEFT into \c #ubi_trRIGHT and vice versa.
 * @hideinitializer
 */
#define ubi_trRevWay(W)    ((char)( ubi_trEQUAL - ((W) - ubi_trEQUAL) ))

/* -------------------------------------------------------------------------- **
 * These macros allow us to quickly read the values of the OVERWRITE and
 * DUPlicate KEY bits of the tree root flags field.
 * -------------------------------------------------------------------------- **
 */

/**
 * @def     ubi_trDups_OK( Tr )
 * @param   Tr  Pointer to a tree root (a #ubi_btRoot structure).
 * @returns Boolean (\c #ubi_trTRUE or \c #ubi_trFALSE).
 * @brief   Tests whether duplicate entries are permitted in the given tree.
 * @details \p Tr must point to a previously initialized #ubi_btRoot
 *          structure.  This macro tests the #ubi_trDUPKEY bit of the
 *          \c flags field, returning True if the flag is set.
 * @hideinitializer
 */
#define ubi_trDups_OK(Tr) \
        ((ubi_trDUPKEY & ((Tr)->flags))?(ubi_trTRUE):(ubi_trFALSE))

/**
 * @def     ubi_trOvwt_OK( Tr )
 * @param   Tr  Pointer to a tree root (a #ubi_btRoot structure).
 * @returns Boolean (\c #ubi_trTRUE or \c #ubi_trFALSE).
 * @brief   Tests whether insertions may overwrite existing nodes.
 * @details \p Tr must point to a previously initialized #ubi_btRoot
 *          structure.  This macro tests teh #ubi_trOVERWRITE bit of the
 *          \c flags field, returning True if the flag is set.
 * @hideinitializer
 */
#define ubi_trOvwt_OK(Tr) \
        ((ubi_trOVERWRITE & ((Tr)->flags))?(ubi_trTRUE):(ubi_trFALSE))

/* -------------------------------------------------------------------------- **
 * Additional Utility Macros.
 * -------------------------------------------------------------------------- **
 */

/**
 * @def     ubi_trCount( Tr )
 * @param   Tr  Pointer to a tree root (a #ubi_btRoot structure).
 * @returns The number of nodes currently in the tree.
 * @brief   Shortcut macro; reads the current node count.
 * @details Reads the current node count from the tree indicated by \p Tr.
 * @hideinitializer
 */
#define ubi_trCount( Tr ) (((ubi_trRootPtr)(Tr))->count)

/**
 * @def     ubi_trNewTree( N, C, F )
 * @param   N   New tree name.
 * @param   C   Comparison function to be used.
 * @param   F   Flags.
 * @brief   Shortcut macro; declares and initializes a new binary tree.
 * @details Creates a new binary tree in one line of code.  The line:
 *          @code
 *          static ubi_trNewTree( MyTree, cmpfn, ubi_trDUPKEY );
 *          @endcode
 *          is equivalent to:
 *          @code
 *          static ubi_trRoot MyTree[1] = {{ NULL, cmpfn, 0, ubi_trDUPKEY }};
 *          @endcode
 *          The new node is allocated from the stack.
 * @hideinitializer
 */
#define ubi_trNewTree( N, C, F ) ubi_trRoot (N)[1] = {{ NULL, (C), 0, (F) }}

/* -------------------------------------------------------------------------- **
 * Typedefs...
 * -------------------------------------------------------------------------- **
 */

/** Pre-standard boolean type.
 *
 * Before \c <stdbool.h> was standardized, we had to make our own booleans
 * from scratch.
 * @see #ubi_trTRUE, #ubi_trFALSE
 */
typedef unsigned char ubi_trBool;

/** Generic key pointer.
 *
 * The \c ubi_btItemPtr is a generic pointer type, used in these modules to
 * indicate a search key when searching within the tree.
 * @see #ubi_btFind(), #ubi_btLocate(), #ubi_btInsert()
 */
typedef void *ubi_btItemPtr;          /* A pointer to key data within a node. */

/*  ------------------------------------------------------------------------- **
 *  Binary Tree Node Structure:  This structure defines the basic elements of
 *       the tree nodes.  In general you should not directly access these
 *       fields.  Let the module methods handle the dirty work.
 *  ------------------------------------------------------------------------- **
 */

/**
 * @struct  ubi_btNode
 * @brief   Binary Tree Node structure.
 * @details There are three pointers, used to link the node to its Left and
 *          Right children (if any) and to its Parent node.  Only the root
 *          node of the tree has a NULL parent pointer.  By keeping track of
 *          the parent pointer, we avoid the need for recursive routines or
 *          hand-tooled stacks to keep track of our path back to the root.
 *
 *          In addition, the node structure keeps track of the "gender" and
 *          (for AVL trees) "balance" of the node.
 *
 * @var ubi_btNode::Link[]
 *      A set of three pointers to, respectively, the left child, parent,
 *      and right child nodes.
 * @var ubi_btNode::gender
 *      A one-byte field indicating whether the node is the RIGHT or LEFT
 *      child of its parent.  If the node is the root of the tree, gender
 *      will be PARENT.
 * @var ubi_btNode::balance
 *      Only used by the AVL tree module.  This field indicates the height
 *      balance at a given node.  Other types of balanced or semi-balanced
 *      trees (e.g. red-black trees), if implemented, might also make use
 *      of this field.
 *      @see ubi_AVLtree.h.
 */
typedef struct ubi_btNodeStruct
  {
  struct ubi_btNodeStruct *Link[ 3 ];
  char                     gender;
  char                     balance;
  } ubi_btNode;

/** Pointer to a #ubi_btNode structure.
 */
typedef ubi_btNode *ubi_btNodePtr;

/*  ------------------------------------------------------------------------- **
 * The next three typedefs define standard function types used by the binary
 * tree management routines.
 */

/**
 * @typedef ubi_btCompFunc
 * @brief   A a pointer to a comparison function.
 * @details The sorting and searching of a given tree are based upon a
 *          comparison function that is associated with the tree when it is
 *          instantiated.  The comparison function is passed two values:
 *          a #ubi_btItemPtr and a #ubi_btNodePtr.
 * @param   #ubi_btItemPtr  A pointer to the search key.
 * @param   #ubi_btNodePtr  A pointer to a node within the tree.  The node's
 *          key will be compared against the search key.
 * @returns The return value of the comparison function must be an unsigned
 *          integer that is less than, equal to, or greater than zero (0) to
 *          indicate that the Item is (respectively) less than, equal to, or
 *          greater than the Item contained within the node.
 *
 * @see #ubi_btInitTree()
 */
typedef  int (*ubi_btCompFunc)( ubi_btItemPtr, ubi_btNodePtr );

/**
 * @typedef ubi_btActionRtn
 * @brief   A pointer to a function to be called for each node visited when
 *          performing a tree traversal.
 * @details The function will be passed two parameters: a #ubi_btNodePtr and
 *          a (<tt>void *</tt>) pointer.
 * @param   #ubi_btNodePtr  A pointer to a node in the tree.  This is the
 *                          current node in the traversal.
 * @param   (void*)         A generic pointer to user data.
 * @returns No return value (<tt>void</tt>).
 * @see     #ubi_btTraverse()
 */
typedef void (*ubi_btActionRtn)( ubi_btNodePtr, void * );

/**
 * @typedef ubi_btKillNodeRtn
 * @brief   A pointer to a function that will deallocate the memory in use
 *          by a node.
 * @details This toolkit leaves memory management to the caller.
 * @param   #ubi_btNodePtr  A pointer to a node that has just been removed
 *          from the tree.
 * @returns No return value (<tt>void</tt>).
 * @see #ubi_btKillTree()
 */
typedef void (*ubi_btKillNodeRtn)( ubi_btNodePtr );

/**
 * @struct  ubi_btRoot
 * @brief   Tree header structure.
 * @details
 *  The \c ubi_btRoot structure provides a header for a binary tree.  A new
 *  binary tree is instantiated by allocating and initializing a
 *  \c ubi_btRoot.
 *
 * @var ubi_btRoot::root
 *      A pointer to the root node of the tree.
 * @var ubi_btRoot::count
 *      A count of the number of nodes stored in the tree.
 * @var ubi_btRoot::cmp
 *      A pointer to the comparison function to be used when inserting or
 *      searching for nodes in the tree.
 * @var ubi_btRoot::flags
 *      A set of bit flags.  Two flags are currently defined:
 *      - #ubi_trOVERWRITE
 *      - #ubi_trDUPKEY
 *      .
 *      \c #ubi_trDUPKEY takes priority over \c #ubi_trOVERWRITE.
 *
 * @see #ubi_trInitTree().
 */
typedef struct
  {
  ubi_btNodePtr  root;     /* A pointer to the root node of the tree       */
  ubi_btCompFunc cmp;      /* A pointer to the tree's comparison function  */
  unsigned long  count;    /* A count of the number of nodes in the tree   */
  char           flags;    /* Overwrite Y|N, Duplicate keys Y|N...         */
  } ubi_btRoot;

/** Pointer to an ubi_btRoot structure.
 */
typedef ubi_btRoot *ubi_btRootPtr;


/* -------------------------------------------------------------------------- **
 * Function Prototypes.
 */

long ubi_btSgn( register long x );

ubi_btNodePtr ubi_btInitNode( ubi_btNodePtr NodePtr );

ubi_btRootPtr  ubi_btInitTree( ubi_btRootPtr   RootPtr,
                               ubi_btCompFunc  CompFunc,
                               char            Flags );

ubi_trBool ubi_btInsert( ubi_btRootPtr  RootPtr,
                         ubi_btNodePtr  NewNode,
                         ubi_btItemPtr  ItemPtr,
                         ubi_btNodePtr *OldNode );

ubi_btNodePtr ubi_btRemove( ubi_btRootPtr RootPtr,
                            ubi_btNodePtr DeadNode );

ubi_btNodePtr ubi_btLocate( ubi_btRootPtr RootPtr,
                            ubi_btItemPtr FindMe,
                            ubi_trCompOps CompOp );

ubi_btNodePtr ubi_btFind( ubi_btRootPtr RootPtr,
                          ubi_btItemPtr FindMe );

ubi_btNodePtr ubi_btNext( ubi_btNodePtr P );

ubi_btNodePtr ubi_btPrev( ubi_btNodePtr P );

ubi_btNodePtr ubi_btFirst( ubi_btNodePtr P );

ubi_btNodePtr ubi_btLast( ubi_btNodePtr P );

ubi_btNodePtr ubi_btFirstOf( ubi_btRootPtr RootPtr,
                             ubi_btItemPtr MatchMe,
                             ubi_btNodePtr p );

ubi_btNodePtr ubi_btLastOf( ubi_btRootPtr RootPtr,
                            ubi_btItemPtr MatchMe,
                            ubi_btNodePtr p );

unsigned long ubi_btTraverse( ubi_btRootPtr   RootPtr,
                              ubi_btActionRtn EachNode,
                              void           *UserData );

unsigned long ubi_btKillTree( ubi_btRootPtr     RootPtr,
                              ubi_btKillNodeRtn FreeNode );

ubi_btNodePtr ubi_btLeafNode( ubi_btNodePtr leader );

int ubi_btModuleID( int size, char *list[] );


/* -------------------------------------------------------------------------- **
 * Masquarade...
 *
 * This set of defines allows you to write programs that will use any of the
 * implemented binary tree modules (currently BinTree, AVLtree, and SplayTree).
 * Instead of using ubi_bt..., use ubi_tr..., and select the tree type by
 * including the appropriate module header.
 *//**
 * @def   ubi_trItemPtr
 * @brief Alias for #ubi_btItemPtr
 *
 * @def   ubi_trNode
 * @brief Alias for #ubi_btNode
 *
 * @def   ubi_trNodePtr
 * @brief Alias for #ubi_btNodePtr
 *
 * @def   ubi_trRoot
 * @brief Alias for #ubi_btRoot
 *
 * @def   ubi_trRootPtr
 * @brief Alias for #ubi_btRootPtr
 *
 * @def   ubi_trCompFunc
 * @brief Alias for #ubi_btCompFunc
 *
 * @def   ubi_trActionRtn
 * @brief Alias for #ubi_btActionRtn
 *
 * @def   ubi_trKillNodeRtn
 * @brief Alias for #ubi_btKillNodeRtn
 *
 * @def   ubi_trSgn
 * @brief Alias for #ubi_btSgn
 *
 * @def   ubi_trInitNode
 * @brief Alias for #ubi_btInitNode
 *
 * @def   ubi_trInitTree
 * @brief Alias for #ubi_btInitTree
 *
 * @def   ubi_trInsert
 * @brief Alias for #ubi_btInsert
 *
 * @def   ubi_trRemove
 * @brief Alias for #ubi_btRemove
 *
 * @def   ubi_trLocate
 * @brief Alias for #ubi_btLocate
 *
 * @def   ubi_trFind
 * @brief Alias for #ubi_btFind
 *
 * @def   ubi_trNext
 * @brief Alias for #ubi_btNext
 *
 * @def   ubi_trPrev
 * @brief Alias for #ubi_btPrev
 *
 * @def   ubi_trFirst
 * @brief Alias for #ubi_btFirst
 *
 * @def   ubi_trLast
 * @brief Alias for #ubi_btLast
 *
 * @def   ubi_trFirstOf
 * @brief Alias for #ubi_btFirstOf
 *
 * @def   ubi_trLastOf
 * @brief Alias for #ubi_btLastOf
 *
 * @def   ubi_trTraverse
 * @brief Alias for #ubi_btTraverse
 *
 * @def   ubi_trKillTree
 * @brief Alias for #ubi_btKillTree
 *
 * @def   ubi_trLeafNode
 * @brief Alias for #ubi_btLeafNode
 *
 * @def   ubi_trModuleID
 * @brief Alias for #ubi_btModuleID
 */

#define ubi_trItemPtr ubi_btItemPtr

#define ubi_trNode    ubi_btNode
#define ubi_trNodePtr ubi_btNodePtr

#define ubi_trRoot    ubi_btRoot
#define ubi_trRootPtr ubi_btRootPtr

#define ubi_trCompFunc    ubi_btCompFunc
#define ubi_trActionRtn   ubi_btActionRtn
#define ubi_trKillNodeRtn ubi_btKillNodeRtn

#define ubi_trSgn( x ) ubi_btSgn( x )

#define ubi_trInitNode( Np ) ubi_btInitNode( (ubi_btNodePtr)(Np) )

#define ubi_trInitTree( Rp, Cf, Fl ) \
        ubi_btInitTree( (ubi_btRootPtr)(Rp), (ubi_btCompFunc)(Cf), (Fl) )

#define ubi_trInsert( Rp, Nn, Ip, On ) \
        ubi_btInsert( (ubi_btRootPtr)(Rp), (ubi_btNodePtr)(Nn), \
                      (ubi_btItemPtr)(Ip), (ubi_btNodePtr *)(On) )

#define ubi_trRemove( Rp, Dn ) \
        ubi_btRemove( (ubi_btRootPtr)(Rp), (ubi_btNodePtr)(Dn) )

#define ubi_trLocate( Rp, Ip, Op ) \
        ubi_btLocate( (ubi_btRootPtr)(Rp), \
                      (ubi_btItemPtr)(Ip), \
                      (ubi_trCompOps)(Op) )

#define ubi_trFind( Rp, Ip ) \
        ubi_btFind( (ubi_btRootPtr)(Rp), (ubi_btItemPtr)(Ip) )

#define ubi_trNext( P ) ubi_btNext( (ubi_btNodePtr)(P) )

#define ubi_trPrev( P ) ubi_btPrev( (ubi_btNodePtr)(P) )

#define ubi_trFirst( P ) ubi_btFirst( (ubi_btNodePtr)(P) )

#define ubi_trLast( P ) ubi_btLast( (ubi_btNodePtr)(P) )

#define ubi_trFirstOf( Rp, Ip, P ) \
        ubi_btFirstOf( (ubi_btRootPtr)(Rp), \
                       (ubi_btItemPtr)(Ip), \
                       (ubi_btNodePtr)(P) )

#define ubi_trLastOf( Rp, Ip, P ) \
        ubi_btLastOf( (ubi_btRootPtr)(Rp), \
                      (ubi_btItemPtr)(Ip), \
                      (ubi_btNodePtr)(P) )

#define ubi_trTraverse( Rp, En, Ud ) \
        ubi_btTraverse((ubi_btRootPtr)(Rp), (ubi_btActionRtn)(En), (void *)(Ud))

#define ubi_trKillTree( Rp, Fn ) \
        ubi_btKillTree( (ubi_btRootPtr)(Rp), (ubi_btKillNodeRtn)(Fn) )

#define ubi_trLeafNode( Nd ) \
        ubi_btLeafNode( (ubi_btNodePtr)(Nd) )

#define ubi_trModuleID( s, l ) ubi_btModuleID( s, l )

/* ========================================================================== */
#endif /* UBI_BINTREE_H */
