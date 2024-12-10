#ifndef UBI_SLINKLIST_H
#define UBI_SLINKLIST_H
/* ========================================================================== **
 *                              ubi_sLinkList.h
 *
 *  Copyright (C) 1997, 1998, 2014, 2020 by Christopher R. Hertel
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
 * $Id: ubi_sLinkList.h; 2024-12-09 21:48:17 -0600; crh$
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
 * Revision 0.4  1998/03/10 02:22:39  crh
 * Combined ubi_StackQueue and ubi_sLinkList into one module.  Redesigned
 * the functions and macros.  Not a complete rewrite but close to it.
 *
 * Revision 0.3  1998/01/03 02:00:02  crh
 * Added ubi_slCount() macro.
 *
 * Revision 0.2  1997/10/21 03:36:14  crh
 * Added parameter <After> in function Insert().  Made necessary changes
 * to macro AddHead() and added macro AddHere().
 *
 * Revision 0.1  1997/10/16 02:54:08  crh
 * Initial Revision.
 *//**
 * @file    ubi_sLinkList.h
 * @author  Christopher R. Hertel
 * @brief   Singly linked lists.
 * @date    October 1997
 * @version \$Id: ubi_sLinkList.h; 2024-12-09 21:48:17 -0600; crh$
 * @copyright Copyright (C) 1997, 1998, 2014, 2020 by Christopher R. Hertel
 *
 * @details
 *  This module implements a singly-linked list, which may also be used as a
 *  queue or a stack.  For a queue, entries are added at the tail and removed
 *  from the head of the list.  For a stack, the entries are added to and
 *  removed from the head of the list.  A traversal will always start at the
 *  head of the list and proceed toward the tail.
 *
 *  This is all mind-numbingly simple, but I'm surprised by the number of
 *  programs out there which re-implement linked lists a dozen or so times.
 *
 * \b Notes
 *  - When the list header is initialized, the Tail pointer is set to point
 *    to the Head pointer.  This simplifies things a great deal, except that
 *    you can't initialize a stack or queue by simply zeroing it out.  One
 *    sure way to initialize the header is to call ubi_slInit().
 *  - This module is similar to the \c ubi_dLinkList module, with three key
 *    differences:
 *    - This is a singly-linked list, the other is a doubly-linked list.
 *      (Kinda fundamental, eh?)
 *    - In this module, if the list is empty, the tail pointer will point
 *      point back to the head of the list as described above.  This is
 *      \e not done in \c ubi_dLinkList.
 *    - The \c #ubi_slRemoveNext() function, by necessity, removes the
 *      'next' node.  In \c ubi_dLinkList, the #ubi_dlRemove() function
 *      removes the specified node.
 */

#include "sys_include.h"    /* System-specific includes. */

/* ========================================================================== **
 * Typedefs...
 */

/**
 * @struct  ubi_slListNode
 * @brief   Basic singly-linked node structure.
 * @details
    Defined as follows:
 *  \code{.c}
 *  typedef struct ubi_slListNode
 *    {
 *    struct ubi_slListNode *Next;
 *    } ubi_slNode;
 *  \endcode
 *  The `ubi_slListNode` name is given only as a forward reference.  Use the
 *  typedef'd name, \c #ubi_slNode, instead.
 * @var ubi_slListNode::Next
 *      Pointer to the next node in the singly linked list.
 *
 * @struct  ubi_slNode
 * @brief   This is the short (typedef'd) name for a `struct ubi_slListNode`.
 */
typedef struct ubi_slListNode
  {
  struct ubi_slListNode *Next;
  } ubi_slNode;

/**
 * @struct  ubi_slNodePtr
 * @brief   Pointer to a \c #ubi_slNode.
 */
typedef ubi_slNode *ubi_slNodePtr;

/**
 * @struct  ubi_slList
 * @brief   List Header structure.
 * @var     ubi_slList::Head
 *          Pointer to the first node in the linked list.
 *          This will be NULL if the list is empty.
 * @var     ubi_slList::Tail
 *          Pointer to the last node in the linked list.
 *          In an empty list, this will point to back to the list header.
 *          This trick (which I learned from AmigaOS) simplifies several
 *          aspects of list handling.
 * @var     ubi_slList::count
 *          A count of the number of nodes currently in the list.
 */
typedef struct
  {
  ubi_slNodePtr Head;
  ubi_slNodePtr Tail;
  unsigned long count;
  } ubi_slList;

/**
 * @struct  ubi_slListPtr
 * @brief   Pointer to a #ubi_slList.
 */
typedef ubi_slList *ubi_slListPtr;


/* ========================================================================== **
 * Macros...
 *
 *  Notes
 *  - All of these provide type casting of the parameters.  The Add and Rem
 *    macros are nothing more than nice front-ends to the Insert and Remove
 *    functions.
 *  - The First, Next and Last macros do no parameter checking.
 */

/** Declare and initialize a list header in one step.
 * @param   L The name to be given to the new list.
 * @details   This macro is used to allocate a new list header from the stack.
 */
#define ubi_slNewList( L ) ubi_slList (L)[1] = {{ NULL, (ubi_slNodePtr)(L), 0 }}

/** Returns the current number of entries in the list.
 * @param   L Pointer to the list to be queried.
 * @returns An unsigned long integer; the number of entries in the list.
 */
#define ubi_slCount( L ) (((ubi_slListPtr)(L))->count)

/** Add a new node at the head of the list.
 * @param   L Pointer to the list to which the node will be added.
 * @param   N Pointer to the node to add to the list.
 * @returns A pointer to the inserted node (same as \p N).
 * @see     #ubi_slInsert()
 */
#define ubi_slAddHead( L, N ) \
        ubi_slInsert( (ubi_slListPtr)(L), (ubi_slNodePtr)(N), NULL )

/** Add a new node following the indicated node.
 * @param   L Pointer to the list to which the node will be added.
 * @param   N Pointer to the node to add to the list.
 * @param   A Pointer to a node already in the list, after which the new
 *          node should be added.
 * @returns A pointer to the inserted node (same as \p N).
 * @see     #ubi_slInsert()
 */
#define ubi_slAddNext( L, N, A ) \
        ubi_slInsert( (ubi_slListPtr)(L), \
                      (ubi_slNodePtr)(N), \
                      (ubi_slNodePtr)(A) )

/** Add a new node to the tail of the list.
 * @param   L Pointer to the list to which the node will be added.
 * @param   N Pointer to the node to add to the list.
 * @returns A pointer to the inserted node (same as \p N).
 * \b Note: AddTail evaluates the L parameter twice.
 * @see     #ubi_slInsert()
 */
#define ubi_slAddTail( L, N ) \
        ubi_slInsert( (ubi_slListPtr)(L), \
                      (ubi_slNodePtr)(N), \
                     ((ubi_slListPtr)(L))->Tail )

/** Remove the node at the head of the list, if any.
 * @param   L Pointer to the list from which the node will be removed.
 * @returns A pointer to the node that was removed, or NULL if the list
 *          was empty.
 * @see     #ubi_slRemoveNext()
 */
#define ubi_slRemHead( L ) ubi_slRemoveNext( (ubi_slListPtr)(L), NULL )

/** Remove the node following the given node.
 * @param   L Pointer to the list from which the node will be removed.
 * @param   N Pointer to the node \e preceeding the node to be removed.
 * @returns A pointer to the node that was removed, or NULL if no node
 *          was removed.
 * @see     #ubi_slRemoveNext()
 */
#define ubi_slRemNext( L, N ) \
        ubi_slRemoveNext( (ubi_slListPtr)(L), (ubi_slNodePtr)(N) )

/** Return a pointer to the first node in the list, if any.
 * @param   L Pointer to the list.
 * @returns A pointer to the first element in the list, or NULL if the
 *          list is empty.
 */
#define ubi_slFirst( L ) (((ubi_slListPtr)(L))->Head)

/** Given a node, return a pointer to the next node.
 * @param   N Pointer to a node in a list.
 * @returns A pointer to the next node in the list, or NULL if \p N
 *          is the last node in the list.
 */
#define ubi_slNext( N )  (((ubi_slNodePtr)(N))->Next)

/** Return a pointer to the last node in the list, if any.
 * @param   L Pointer to the list.
 * @returns A pointer to the last element in the list, or NULL if the
 *          list is empty.
 */
#define ubi_slLast( L )  (((ubi_slListPtr)(L))->Tail)

/** Add a node at the head of the list; synonym of \c #ubi_slAddHead()). */
#define ubi_slPush    ubi_slAddHead

/** Remove a node at the head of the list; synonym of \c #ubi_slRemHead(). */
#define ubi_slPop     ubi_slRemHead

/** Add a node at the tail of the list; synonym of \c #ubi_slAddTail(). */
#define ubi_slEnqueue ubi_slAddTail

/** Remove a node at the head of the list; yet another synonym of
 *  \c #ubi_slRemHead().
 */
#define ubi_slDequeue ubi_slRemHead


/* ========================================================================== **
 * Function prototypes...
 */

ubi_slListPtr ubi_slInitList( ubi_slListPtr ListPtr );

ubi_slNodePtr ubi_slInsert( ubi_slListPtr ListPtr,
                            ubi_slNodePtr New,
                            ubi_slNodePtr After );

ubi_slNodePtr ubi_slRemoveNext( ubi_slListPtr ListPtr, ubi_slNodePtr AfterMe );

/* ================================ The End ================================= */
#endif /* UBI_SLINKLIST_H */
