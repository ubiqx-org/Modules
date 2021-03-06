/* ========================================================================== **
 *                              ubi_sLinkList.c
 *
 *  Copyright (C) 1997, 1998, 2014 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *  This module implements a simple singly-linked list.
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
 * $Id: ubi_sLinkList.c; 2020-08-04 20:00:04 -0500; Christopher R. Hertel$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 0.11  2014/11/20 crh
 * Updated some internal comments to remind readers to think very carefully
 * about how the ubi_slRemoveNext() code works.  Updated the copyright date.
 *
 * Revision 0.10  1999/06/19 16:58:06  crh
 * Renamed the ubi_slRemove() function in ubi_sLinkList to
 * ubi_slRemoveNext().  I was bothered by the fact that it didn't
 * match the functionality of the ubi_dlRemove() function in
 * ubi_dLinkList.  The new name is more 'correct'.
 *
 * Revision 0.9  1998/07/24 07:30:20  crh
 * Added the ubi_slNewList() macro.
 *
 * Revision 0.8  1998/06/04 21:29:27  crh
 * Upper-cased defined constants (eg UBI_BINTREE_H) in some header files.
 * This is more "standard", and is what people expect.  Weird, eh?
 *
 * Revision 0.7  1998/06/03 18:06:03  crh
 * Further fiddling with sys_include.h, which has been moved from the .c file
 * to the .h file.
 *
 * Revision 0.6  1998/06/02 01:38:47  crh
 * Changed include file name from ubi_null.h to sys_include.h to make it
 * more generic.
 *
 * Revision 0.5  1998/05/20 04:38:05  crh
 * The C file now includes ubi_null.h.  See ubi_null.h for more info.
 *
 * Revision 0.4  1998/03/10 02:23:20  crh
 * Combined ubi_StackQueue and ubi_sLinkList into one module.  Redesigned
 * the functions and macros.  Not a complete rewrite but close to it.
 *
 * Revision 0.3  1998/01/03 01:59:52  crh
 * Added ubi_slCount() macro.
 *
 * Revision 0.2  1997/10/21 03:35:18  crh
 * Added parameter <After> in function Insert().  Made necessary changes
 * to macro AddHead() and added macro AddHere().
 *
 * Revision 0.1  1997/10/16 02:53:45  crh
 * Initial Revision.
 *
 * ========================================================================== **
 */

#include "ubi_sLinkList.h"  /* Header for *this* module. */

/* ========================================================================== **
 * Functions...
 */

ubi_slListPtr ubi_slInitList( ubi_slListPtr ListPtr )
  /** Initialize a singly-linked list header.
   *
   * @param   ListPtr   A pointer to the list structure that is to be
   *                    initialized for use.
   *
   * @returns A pointer to the initialized list header (i.e., same as
   *          \p ListPtr).
   */
  {
  ListPtr->Head  = NULL;
  ListPtr->Tail  = (ubi_slNodePtr)ListPtr;
  ListPtr->count = 0;
  return( ListPtr );
  } /* ubi_slInitList */

ubi_slNodePtr ubi_slInsert( ubi_slListPtr ListPtr,
                            ubi_slNodePtr New,
                            ubi_slNodePtr After )
  /** Add a node to the list.
   *
   * @param   ListPtr   A pointer to the list into which the node is to
   *                    be inserted.
   * @param   New       Pointer to the node that is to be added to the list.
   * @param   After     Pointer to a list in a node after which the new node
   *                    will be inserted.  If NULL, then the new node will
   *                    be added at the head of the list.
   *
   * @returns   A pointer to the node that was inserted into the list (i.e.,
   *            the same as \p New).
   */
  {
  After = After ? After : (ubi_slNodePtr)ListPtr;
  New->Next   = After->Next;
  After->Next = New;
  if( !(New->Next) )
    ListPtr->Tail = New;
  (ListPtr->count)++;
  return( New );
  } /* ubi_slInsert */

ubi_slNodePtr ubi_slRemoveNext( ubi_slListPtr ListPtr, ubi_slNodePtr AfterMe )
  /** Remove a node from the list.
   *
   * Remove the node followng \p AfterMe.  If \p AfterMe is NULL, remove from
   * the head of the list.
   *
   * @param   ListPtr   A pointer to the list from which the node is to be
   *                    removed.
   * @param   AfterMe   Pointer to the node \e preceeding the node to be
   *                    removed.
   *
   * @returns A pointer to the node that was removed, or NULL.  NULL is
   *          returned if the list is already empty, or if there is no node
   *          following \p AfterMe.
   */
  {
  ubi_slNodePtr DelNode;

  /* This next blurp of code is quirky, but it works.
   * If <AfterMe> is given as NULL, we set it to point to the list header,
   * casting the list header to be a node header.
   */
  if( NULL == AfterMe )
    AfterMe = (ubi_slNodePtr)ListPtr;
  DelNode = AfterMe->Next;

  /* If there is a node to delete, delete it. */
  if( DelNode )
    {
    if( !(DelNode->Next) )
      ListPtr->Tail = AfterMe;
    AfterMe->Next = DelNode->Next;
    (ListPtr->count)--;
    }
  return( DelNode );
  } /* ubi_slRemoveNext */

/* ================================ The End ================================= */
