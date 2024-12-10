#ifndef UBI_DLINKLIST_H
#define UBI_DLINKLIST_H
/* ========================================================================== **
 *                              ubi_dLinkList.h
 *
 *  Copyright (C) 1997-1998, 2020 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *  This module implements simple doubly-linked lists.
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
 * $Id: ubi_dLinkList.h; 2024-12-09 21:48:17 -0600; crh$
 * https://github.com/ubiqx-org/Modules
 *
 * Change logs are now in git.
 *
 * Old CVS Logs:
 *
 * Revision 0.11  1999/06/19 16:58:06  crh
 * Renamed the ubi_slRemove() function in ubi_sLinkList to
 * ubi_slRemoveNext().  I was bothered by the fact that it didn't
 * match the functionality of the ubi_dlRemove() function in
 * ubi_dLinkList.  The new name is more 'correct'.
 *
 * Revision 0.10  1998/07/24 07:30:20  crh
 * Added the ubi_dlNewList() macro.
 *
 * Revision 0.9  1998/06/04 21:29:27  crh
 * Upper-cased defined constants (eg UBI_BINTREE_H) in some header files.
 * This is more "standard", and is what people expect.  Weird, eh?
 *
 * Revision 0.8  1998/06/03 18:06:03  crh
 * Further fiddling with sys_include.h, which has been moved from the .c file
 * to the .h file.
 *
 * Revision 0.7  1998/06/02 01:38:47  crh
 * Changed include file name from ubi_null.h to sys_include.h to make it
 * more generic.
 *
 * Revision 0.6  1998/05/20 04:38:05  crh
 * The C file now includes ubi_null.h.  See ubi_null.h for more info.
 *
 * Revision 0.5  1998/03/10 02:54:04  crh
 * Simplified the code and added macros for stack & queue manipulations.
 *
 * Revision 0.4  1998/01/03 01:53:44  crh
 * Added ubi_dlCount() macro.
 *
 * Revision 0.3  1997/10/15 03:04:31  crh
 * Added some handy type casting to the macros.  Added AddHere and RemThis
 * macros.
 *
 * Revision 0.2  1997/10/08 03:08:16  crh
 * Fixed a few forgotten link-ups in Insert(), and fixed the AddHead()
 * macro, which was passing the wrong value for <After> to Insert().
 *
 * Revision 0.1  1997/10/07 04:34:38  crh
 * Initial Revision.
 * ========================================================================== **
 *//**
 * @file    ubi_dLinkList.h
 * @author  Christopher R. Hertel
 * @brief   Doubly linked list implementation.
 * @date    Octoberly 1997
 * @version \$Id: ubi_dLinkList.h; 2024-12-09 21:48:17 -0600; crh$
 * @copyright Copyright (C) 1997-1998, 2020 by Christopher R. Hertel
 *
 * @details
 *  This module is similar to the \c ubi_sLinkList module, but it is neither
 *  a descendant type nor an easy drop-in replacement for the latter.  One
 *  key difference is that the \c #ubi_dlRemove() function removes the
 *  indicated node, while the \c #ubi_slRemoveNext() function (in
 *  \c ubi_sLinkList) removes the node \b following the indicated node...an
 *  important semantic difference.
 *
 *  Singly and doubly linked lists have different semantics.  That's kinda
 *  basic CSci stuff, and the reason that both are so commonly used.
 */

#include "sys_include.h"    /* System-specific includes. */


/* ========================================================================== **
 * Typedefs...
 */

/**
 * @struct  ubi_dlListNode
 * @brief   Doubly linked list node structure.
 * @details The `ubi_dlListNode` name is given only as a forward reference.
 *          Use the typedef'd name, \c #ubi_dlNode, instead.
 * @var     ubi_dlListNode::Next
 *          Pointer to the next node in the linked list.
 * @var     ubi_dlListNode::Prev
 *          Pointer to the previous node in the linked list.
 *
 * @struct  ubi_dlNode
 * @brief   This is the short (typedef'd) name for a `struct ubi_dlListNode`.
 */
typedef struct ubi_dlListNode
  {
  struct ubi_dlListNode *Next;
  struct ubi_dlListNode *Prev;
  } ubi_dlNode;

/**
 * @struct  ubi_dlNodePtr
 * @brief   Pointer to a \c #ubi_dlNode.
 */
typedef ubi_dlNode *ubi_dlNodePtr;

/**
 * @struct    ubi_dlList
 * @brief     The list header structure.
 * @var       ubi_dlList::Head
 *            Pointer to the first node in the linked list, if any.
 * @var       ubi_dlList::Tail
 *            Pointer to the last node in the linked list, if any.
 * @var       ubi_dlList::count
 *            The number of entries in the linked list.
 */
typedef struct
  {
  ubi_dlNodePtr Head;
  ubi_dlNodePtr Tail;
  unsigned long count;
  } ubi_dlList;

/**
 * @struct  ubi_dlListPtr
 * @brief   Pointer to a List (i.e., a \c #ubi_dlList structure).
 */
typedef ubi_dlList *ubi_dlListPtr;


/* ========================================================================== **
 * Macros...
 *
 *  Note that all of these provide type casting of the parameters.  The
 *  Add and Rem macros are nothing more than nice front-ends to the
 *  Insert and Remove operations.
 *
 *  Also note that the First, Next and Last macros do no parameter checking!
 */

/** Macro used to declare and initialize a new list in one swell foop.
 *
 *  This macro allocates a variable of type \c #ubi_dlList.  For example:
 * @code
 *    static ubi_dlNewList( gerbil );
 * @endcode
 *  would translate to
 * @code
 *    static ubi_dlList gerbil[1] = {{ NULL, NULL, 0 }};
 * @endcode
 */
#define ubi_dlNewList( L ) ubi_dlList (L)[1] = {{ NULL, NULL, 0 }}

/** Return the number of entries currently in the list. */
#define ubi_dlCount( L ) (((ubi_dlListPtr)(L))->count)

/** Add a new node at the head of the list.
 * @param   L The list into which the node is to be added.
 * @param   N The node to be added to the list.
 * @returns A pointer to the newly inserted node (same as \p N).
 * @see     #ubi_dlInsert()
 */
#define ubi_dlAddHead( L, N ) \
        ubi_dlInsert( (ubi_dlListPtr)(L), (ubi_dlNodePtr)(N), NULL )

/** Add a node following the given node.
 * @param   L The list into which the node is to be added.
 * @param   N The node to be added to the list.
 * @param   A The node after which the new node will be inserted.
 * @returns A pointer to the newly inserted node (same as \p N).
 * @see     #ubi_dlInsert()
 */
#define ubi_dlAddNext( L, N, A ) \
        ubi_dlInsert( (ubi_dlListPtr)(L), \
                      (ubi_dlNodePtr)(N), \
                      (ubi_dlNodePtr)(A) )

/** Add a new node at the tail of the list.
 * @param   L The list into which the node is to be added.
 * @param   N The node to be added to the list.
 * @returns A pointer to the newly inserted node (same as \p N).
 * \b Note: #ubi_dlAddTail() evaluates the \p L parameter twice.
 * @see     #ubi_dlInsert()
 */
#define ubi_dlAddTail( L, N ) \
        ubi_dlInsert( (ubi_dlListPtr)(L), \
                      (ubi_dlNodePtr)(N), \
                    (((ubi_dlListPtr)(L))->Tail) )

/** Remove the node at the head of the list, if any.
 * @param   L Pointer to the list from which the node will be removed.
 * @returns A pointer to the node that was removed, or NULL if the list
 *          was empty.
 * @see     #ubi_dlRemove()
 */
#define ubi_dlRemHead( L ) ubi_dlRemove( (ubi_dlListPtr)(L), \
                                         (((ubi_dlListPtr)(L))->Head) )

/** Remove the given node from the list.
 * @param   L Pointer to the list from which the node will be removed.
 * @param   N Pointer to the node to be removed.
 * @returns A pointer to the node that was removed, or NULL if no node
 *          was removed.
 * @see     #ubi_dlRemove()
 */
#define ubi_dlRemThis( L, N ) ubi_dlRemove( (ubi_dlListPtr)(L), \
                                            (ubi_dlNodePtr)(N) )

/** Remove the node at the tail of the list, if any.
 * @param   L Pointer to the list from which the node will be removed.
 * @returns A pointer to the node that was removed, or NULL if the list
 *          was empty.
 * @see     #ubi_dlRemove()
 * \b Note: This macro evaluates the \p L parameter twice.
 */
#define ubi_dlRemTail( L ) ubi_dlRemove( (ubi_dlListPtr)(L), \
                                         (((ubi_dlListPtr)(L))->Tail) )

/** Return a pointer to the first node in the list, if any. */
#define ubi_dlFirst( L ) (((ubi_dlListPtr)(L))->Head)

/** Return a pointer to the last node in the list, if any.  */
#define ubi_dlLast( L )  (((ubi_dlListPtr)(L))->Tail)

/** Given a node, return a pointer to the next node in the list.  */
#define ubi_dlNext( N )  (((ubi_dlNodePtr)(N))->Next)

/** Given a node, return a pointer to the previous node in the list.  */
#define ubi_dlPrev( N )  (((ubi_dlNodePtr)(N))->Prev)

/** Add a node at the head of the list; synonym of \c #ubi_dlAddHead().  */
#define ubi_dlPush    ubi_dlAddHead

/** Remove a node at the head of the list; synonym of \c #ubi_dlRemHead(). */
#define ubi_dlPop     ubi_dlRemHead

/** Add a node at the tail of the list; synonym of \c #ubi_dlAddTail().  */
#define ubi_dlEnqueue ubi_dlAddTail

/** Remove a node at the head of the list; another synonym of
 *  \c #ubi_dlRemHead().
 */
#define ubi_dlDequeue ubi_dlRemHead


/* ========================================================================== **
 * Function prototypes...
 */

ubi_dlListPtr ubi_dlInitList( ubi_dlListPtr ListPtr );

ubi_dlNodePtr ubi_dlInsert( ubi_dlListPtr ListPtr,
                            ubi_dlNodePtr New,
                            ubi_dlNodePtr After );

ubi_dlNodePtr ubi_dlRemove( ubi_dlListPtr ListPtr, ubi_dlNodePtr Old );

/* ================================ The End ================================= */
#endif /* UBI_DLINKLIST_H */
