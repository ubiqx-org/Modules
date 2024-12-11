/* ========================================================================== **
 *                             ubi_SparseArray.c
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
 * $Id: ubi_SparseArray.c; 2024-12-11 08:53:18 -0600; crh$
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
 */

#include "ubi_SparseArray.h"  /* Header for *this* module. */


/* -------------------------------------------------------------------------- **
 * Functions...
 */

ubi_arrRootPtr ubi_arrInitRoot( ubi_arrRootPtr RootPtr,
                                ubi_trCompFunc CompFunc,
                                char           Flags )
  /** Initialize a vector root.
   *
   * @param   RootPtr   Pointer to the ubi_arrRoot to be initialized.
   * @param   CompFunc  Comparison function to be used to sort the entries
   *                    in this vector.
   * @param   Flags     Available flags are listed in \c ubi_BinTree.h.
   *
   * @returns A pointer to the initialized root structure (that is, the same
   *          as \p RootPtr).
   *
   * \b Notes
   *  - This is a descendent of the binary tree root initialization.  The
   *    vector root has additional pointers to the parent tree and node
   *    that are also initialized.
   *
   * @see #ubi_arrAddSubArray()
   */
  {
  (void)ubi_trInitTree( (ubi_trRootPtr)RootPtr, CompFunc, Flags );
  RootPtr->parentArray = NULL;
  RootPtr->parentNode  = NULL;
  return( RootPtr );
  } /* ubi_arrInitRoot */

ubi_arrNodePtr ubi_arrInitNode( ubi_arrNodePtr NodePtr )
  /** Initialize a sparse array node structure.
   *
   * @param   NodePtr   Pointer to the ubi_arrNode to be initialized.
   *
   * @returns A pointer to the initialized node (that is, the same as
   *          \p NodePtr).
   */
  {
  (void)ubi_trInitNode( (ubi_trNodePtr)NodePtr );
  NodePtr->subArray = NULL;
  return( NodePtr );
  } /* ubi_arrInitNode */

ubi_arrRootPtr ubi_arrDown( ubi_arrNodePtr NodePtr )
  /** Return the subvector of the current node, if any.
   *
   * @param   NodePtr   pointer to the link node.
   *
   * @returns A pointer to the subarray attached at \p NodePtr, or NULL if
   *          \p NodePtr does not point to a subarray.
   */
  {
  return( (ubi_arrRootPtr)(NodePtr->subArray) );
  } /* ubi_arrDown */

ubi_arrRootPtr ubi_arrUp( ubi_arrRootPtr RootPtr, ubi_arrNodePtr *parentp )
  /** Logically move up a level in the given sparse array.
   *
   * Given a pointer to a vector root, return a pointer to the vector that
   * contains the parent node (if there is one).
   *
   * @param   RootPtr   A pointer to a vector root.
   * @param   parentp   A pointer to a #ubi_arrNodePtr.  If NULL, this value
   *                    will be ignored.  If non-NULL, the indicated pointer
   *                    will be set to point to the node which is the
   *                    parent of \p RootPtr.
   *
   * @returns A pointer to the root of the parent vector.  If RootPtr points
   *          to a top-level vector, then the return value will be NULL (and,
   *          if <tt>parentp != NULL</tt>, then \p parentp will also be set
   *          to NULL).
   */
  {
  if( NULL != parentp )
    *parentp = RootPtr->parentNode;
  return( RootPtr->parentArray );
  } /* ubi_arrUp */

ubi_arrRootPtr ubi_arrTop( ubi_arrRootPtr RootPtr )
  /** Return a pointer to the top-most tree in the sparse array.
   *
   * @param   RootPtr   Pointer to the root of a tree (vector) within the
   *                    sparse array.
   *
   * @returns A pointer to the top-most root in the sparse array.  If
   *          \p RootPtr is NULL, then the array is considered to be the
   *          'empty' array and NULL will be returned.
   */
  {
  if( NULL != RootPtr )
    {
    while( NULL != RootPtr->parentArray )
      RootPtr = RootPtr->parentArray;
    }
  return( RootPtr );
  } /* ubi_arrTop */

ubi_arrRootPtr ubi_arrAddSubArray( ubi_arrRootPtr NewRootPtr,
                                   ubi_arrRootPtr ParentRootPtr,
                                   ubi_arrNodePtr ParentNodePtr )
  /** Add a vector header (tree root) at a given node.
   *
   * @param   NewRootPtr      Pointer to the root of the vector (tree) that
   *                          is to be added as a child array at the node
   *                          indicated by \p ParentNodePtr.
   * @param   ParentRootPtr   Pointer to the root of the vector (tree) that
   *                          contains \p ParentNodePtr.
   * @param   ParentNodePtr   The node at which the new vector will be
   *                          attached.
   *
   * @returns Pointer to the root of the newly added vector (that is, the
   *          same as \p NewRootPtr).
   */
  {
  ParentNodePtr->subArray = NewRootPtr;
  NewRootPtr->parentArray = ParentRootPtr;
  NewRootPtr->parentNode  = ParentNodePtr;
  return( NewRootPtr );
  } /* ubi_arrAddSubArray */

ubi_arrRootPtr ubi_arrRemSubArray( ubi_arrNodePtr NodePtr )
  /** Remove a vector from a node.
   *
   * @param   NodePtr   Pointer to the node at which the vector to be removed
   *                    is connected.
   *
   * @returns A pointer to the root of the vector that was disconnected.
   *          If no vector was found, the function will return NULL.
   */
  {
  ubi_arrRootPtr tmp = NodePtr->subArray;

  if( NULL != tmp )
    {
    tmp->parentArray  = NULL;
    tmp->parentNode   = NULL;
    NodePtr->subArray = NULL;
    }
  return( tmp );
  } /* ubi_arrRemSubArray */

/* ================================ The End ================================= */
