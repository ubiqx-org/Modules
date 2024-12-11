#ifndef UBI_SPARSEARRAY_H
#define UBI_SPARSEARRAY_H
/* ========================================================================== **
 *                             ubi_SparseArray.h
 *
 *  Copyright (C) 1999, 2002, 2020 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *  This module implements a binary tree based sparse array.
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
 * $Id: ubi_SparseArray.h; 2024-12-11 08:53:18 -0600; crh$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 1.1  2010-11-08 19:58:15  crh
 * Fixed a spelling error.
 *
 * Revision 1.0  2005/11/13 03:03:42  crh
 * I thought these were in the repository.  Ouch.
 *
 *
 * ========================================================================== **
 *//**
 * @file    ubi_SparseArray.h
 * @author  Christopher R. Hertel
 * @brief   Sparse Array over Binary Trees.
 * @date    29-Aug-2002
 * @version \$Id: ubi_SparseArray.h; 2024-12-11 08:53:18 -0600; crh$
 * @copyright Copyright (C) 1999, 2002, 2020 by Christopher R. Hertel
 *
 * @details
 *  This is a simple implementation of a sparse array using binary trees as
 *  the underlying database engine.  The default is to use ubi_SplayTree,
 *  but this can be changed by modifying the header file.
 *
 *  For those familiar with the workings of the binary tree modules, this
 *  implementation adds a pointer from the #ubi_trNode structure to a child
 *  tree.  A child tree can then be connected at any node in a given binary
 *  tree instance.  The #ubi_trRoot structure also has added pointers to
 *  link back to the parent tree and parent node.
 *
 *  When we talk about a child tree, we are talking about an entire binary
 *  tree, not a subtree.  This adds a new dimension at each tree node.  The
 *  child trees are sometimes refered to as vectors.
 *
 *  Using these new pointers (from node to child tree and from child tree
 *  root back to parent node and parent tree), it is easy to traverse the
 *  sparse array.  Traversal within a vector is handled by the binary tree
 *  functions.
 *
 *  This is neither the fastest or most efficient implementation.  A B+Tree
 *  would probably provide a better underlying database.  It'd be
 *  interesting to see what could be done with a sparse array built on top
 *  of \c memcached, or similar.
 *
 *  I would be remiss if I did not mention that my first exposure to this
 *  sort of database came from working with a language called MUMPS.  It
 *  was a very early interpreted language, but it is still in use.  The
 *  Sparse Array at the core of the language is one of the reasons for its
 *  longevity.
 *
 * @see https://en.wikipedia.org/wiki/MUMPS
 */

#include "ubi_SplayTree.h"


/* -------------------------------------------------------------------------- **
 * Typedefs...
 */

/* Forward Reference. */
struct ubi_arrNodeStruct;

/**
 * @struct  ubi_arrRootStruct
 * @brief   The root of a single vector within the sparse array.
 * @note    The `%ubi_arrRootStruct` and `%ubi_arrNodeStruct` names are used
 *          only as a forward references.
 *          * `%ubi_arrRoot` is a typedef for `struct %ubi_arrRootStruct`.
 *          * `%ubi_arrNode` is a typedef for `struct %ubi_arrNodeStruct`.
 * @var     ubi_arrRootStruct::root
 *          Tree Header for the sub-array.
 * @var     ubi_arrRootStruct::parentArray
 *          Pointer to the parent array, if any.
 * @var     ubi_arrRootStruct::parentNode
 *          Pointer to the parent node, if any.
 */
struct ubi_arrRootStruct
  {
  ubi_trRoot                root;
  struct ubi_arrRootStruct *parentArray;
  struct ubi_arrNodeStruct *parentNode;
  };

/**
 * @typedef ubi_arrRoot
 * @brief   This is the short (typedef'd) name for a `struct ubi_arrRootStruct`.
 */
 typedef struct ubi_arrRootStruct ubi_arrRoot;

/**
 * @typedef ubi_arrRootPtr
 * @brief   Pointer to a `ubi_arrRoot`.
 */
typedef ubi_arrRoot *ubi_arrRootPtr;

/**
 * @struct  ubi_arrNodeStruct
 * @brief   A node which can be stored within a vector.
 */
struct ubi_arrNodeStruct
  {
  ubi_trNode     node;        /**< Standard tree node structure.  */
  ubi_arrRootPtr subArray;    /**< Link to potential child array. */
  };

/**
 * @typedef ubi_arrNode
 * @brief   This is the short (typedef'd) name for a `struct ubi_arrNodeStruct`.
 */
typedef struct ubi_arrNodeStruct ubi_arrNode;

/**
 * @typedef ubi_arrNodePtr
 * @brief   Pointer to a `ubi_arrNode`.
 */
typedef ubi_arrNode *ubi_arrNodePtr;


/* -------------------------------------------------------------------------- **
 * Function Prototypes...
 */

ubi_arrRootPtr ubi_arrInitRoot( ubi_arrRootPtr RootPtr,
                                ubi_trCompFunc CompFunc,
                                char           Flags );

ubi_arrNodePtr ubi_arrInitNode( ubi_arrNodePtr NodePtr );

ubi_arrRootPtr ubi_arrDown( ubi_arrNodePtr NodePtr );

ubi_arrRootPtr ubi_arrUp( ubi_arrRootPtr RootPtr, ubi_arrNodePtr *parentp );

ubi_arrRootPtr ubi_arrTop( ubi_arrRootPtr RootPtr );

ubi_arrRootPtr ubi_arrAddSubArray( ubi_arrRootPtr NewRootPtr,
                                   ubi_arrRootPtr ParentRootPtr,
                                   ubi_arrNodePtr ParentNodePtr );

ubi_arrRootPtr ubi_arrRemSubArray( ubi_arrNodePtr NodePtr );

/* ================================ The End ================================= */
#endif /* UBI_SPARSEARRAY_H */
