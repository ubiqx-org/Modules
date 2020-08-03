#ifndef SYS_INCLUDE_H
#define SYS_INCLUDE_H
/* ========================================================================== **
 *                               sys_include.h
 *
 *  Copyright (C) 1998, 2020 by Christopher R. Hertel
 *
 * -------------------------------------------------------------------------- **
 *  This header provides system declarations and data types used internally
 *  by the ubiqx modules.
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
 * Notes:
 *   The only thing this header currently provides is a definition of NULL.
 *   Any other system-specific variations might also be handled by adapting
 *   this header.  You might also choose to replace this file with your own
 *   system specific header, or you may find that this default header has
 *   everything you need.  I expect the latter to be the case most of the
 *   time.
 *
 *   The ubi_* modules were written to be as clean as possible.  On purpose.
 *   There are limits, though.  Variations in compilers and environments
 *   have made it difficult to write code that just compiles.  In
 *   particular, the location of a definition of NULL seems to be less
 *   than consistent.  See the notes below.
 *
 *   This header makes a good attempt to find NULL.  If you find that you
 *   need something more on your system make sure that you keep a copy of
 *   your version so that it won't be overwritten by updates of the ubiqx
 *   code.
 *
 * -------------------------------------------------------------------------- **
 *
 * $Id: sys_include.h; 2020-08-03 14:19:41 -0500; Christopher R. Hertel$
 * https://github.com/ubiqx-org/Modules
 *
 * Revision 0.0  1998/06/02 02:20:49  crh
 * Initial Revision.
 *
 * ========================================================================== **
 *//**
 * @file      sys_include.h
 * @author    Christopher R. Hertel
 * @version   $Id: sys_include.h; 2020-08-03 14:19:41 -0500; Christopher R. Hertel$
 * @copyright Copyright (C) 1998, 2020 by Christopher R. Hertel
 *
 * @brief     Ensure a correct definition of NULL.
 *
 * @details   Looking for NULL.
 *
 *  The core ubiqx modules (all those beginning with 'ubi_') rely on very
 *  little from the outside world.  One exception is that we need a proper
 *  defintion for NULL.  This has turned out to be something of a problem,
 *  as NULL is NOT always defined in the same place on different systems.
 *
 *  K&R 2nd Ed. (pg 102) says NULL should be in \c <stdio.h>.  Rumor has it
 *  that some systems define it in \c <locale.h>.  POSIX says that it must be
 *  defined in \c <stddef.h> \e and also in \c <stdlib.h>.  POSIX is our best
 *  bet, of course, but there are older compilers still out there thay may
 *  not be compliant.
 *
 *  Also, NULL is supposed to be <tt>((void *)0)</tt>, but I have been
 *  informed of at least one exception existing in the wild.  Weird, eh?
 *
 *  crh may 1998, updated august 2020.
 */

/* Just make sure we have a valid NULL, eh? */
#ifndef NULL
#include <stddef.h>
#endif
#ifndef NULL
#include <stdlib.h>
#endif
#ifndef NULL
#include <stdio.h>
#endif
#ifndef NULL
#include <locale.h>
#endif
#ifndef NULL
/**
 * @def   NULL
 * @brief If all else fails, define NULL as <tt>((void *)0)</tt>.
 * @hideinitializer
 */
#define NULL ((void *)0)
#endif

/* ================================ The End ================================= */
#endif /* SYS_INCLUDE_H */
