#====================================================================
#
# Makefile for ubiqx
#
#====================================================================

#######################################
# CC generic configuration            #
#######################################
#CC	= cc
#CFLAGS	= -O2
#RM	= rm -f
#AR	= ar
#RANLIB	= ranlib

#KDB_XDBM	= DBimps/kdb_xdbm.o

#LIB_UBIQX	= libubiqx.a
#ALL_CFLAGS	= -I library $(CFLAGS)

#######################################
# SAS/c (Amiga) configuration         #
#######################################
#CC	= ucc
#CFLAGS	=
#RM	= delete quiet
#AR	= # unused
#RANLIB	= # unused

#KDB_XDBM	= # unused

#LIB_UBIQX	= ubiqx.lib
#ALL_CFLAGS	= -I library $(CFLAGS)

#######################################
# Gcc configuration with no gdbm      #
# support.                            #
#######################################
CC     = gcc
CFLAGS = -O2
RM     = rm -f
AR     = ar
RANLIB = ranlib

KDB_XDBM 	= # unused

LIB_UBIQX 	= libubiqx.a
ALL_CFLAGS 	= -I library $(CFLAGS) 

#######################################
# SGI IRIX 6 configuration            #
#######################################
#CC	= cc
#CFLAGS	= -n32 -O2
#RM	= rm -f
#AR	= ar
#RANLIB	= echo created # Irix doesn't have ranlib

#KDB_XDBM	= DBimps/kdb_xdbm.o

#LIB_UBIQX	= libubiqx.a
#ALL_CFLAGS	= -I library $(CFLAGS)

#######################################
# CygWin32 configuration              #
# Thanks to Pete Nelson in Saint Paul #
#######################################
#CC = gcc
#CFLAGS = -O2
#RM = rm -f
#AR = ar
#RANLIB = echo created

#KDB_XDBM	= # unused

#LIB_UBIQX = libubiqx.a
#ALL_CFLAGS = -I library $(CFLAGS) 

#====================================================================

# ------------ #
# main library #
# ------------ #
OBJ_UBIQX = \
	library/ubi_AVLtree.o \
	library/ubi_BinTree.o \
	library/ubi_SplayTree.o \
	library/ubi_Cache.o \
	library/ubi_dLinkList.o \
	library/ubi_sLinkList.o \
	library/ubi_SparseArray.o

# ------------- #
# Test programs #
# ------------- #

BIN_TEST_TOYS = \
	test-toys/avl-test \
	test-toys/cache-test \
	test-toys/dll-test \
	test-toys/sll-test \
	test-toys/tree-sample \

TEST_DATA = test-toys/test_data.tmp

#
# all: Compile all objects and create all executables
#
all : $(OBJ_UBIQX) $(OBJ_TEST_TOYS) $(BIN_TEST_TOYS)

#
# modules : compiles all ubiqx modules, but creates only
#           the *.o files
#
modules : $(OBJ_UBIQX)

#
# Create the static link library
#
libubiqx.a : $(OBJ_UBIQX)
	$(AR) cr $@ $(OBJ_UBIQX)
	$(RANLIB) $@

ubiqx.lib : $(OBJ_UBIQX)
	oml -s $@ r $(OBJ_UBIQX)

lib: $(LIB_UBIQX)

#
# Cleanup
#
clean :
	-$(RM) $(OBJ_UBIQX) $(LIB_UBIQX) \
	       $(OBJ_TEST_TOYS) $(BIN_TEST_TOYS) $(TEST_DATA)

#
# Rebuild everything from scratch
#
rebuild : clean all

#
# Compile executables from test-toys/
#
test-toys/avl-test : test-toys/avl-test.c $(OBJ_UBIQX)
	$(CC) $(ALL_CFLAGS) $(OBJ_UBIQX) test-toys/avl-test.c -o $@

test-toys/cache-test : test-toys/cache-test.c $(OBJ_UBIQX)
	$(CC) $(ALL_CFLAGS) $(OBJ_UBIQX) test-toys/cache-test.c -o $@

test-toys/dll-test : test-toys/dll-test.c $(OBJ_UBIQX)
	$(CC) $(ALL_CFLAGS) $(OBJ_UBIQX) test-toys/dll-test.c -o $@

test-toys/sll-test : test-toys/sll-test.c $(OBJ_UBIQX)
	$(CC) $(ALL_CFLAGS) $(OBJ_UBIQX) test-toys/sll-test.c -o $@

test-toys/tree-sample : test-toys/tree-sample.c $(OBJ_UBIQX)
	$(CC) $(ALL_CFLAGS) $(OBJ_UBIQX) test-toys/tree-sample.c -o $@

#######################################
# Private rules                       #
#######################################

#
# Update dependencies
#
depend :
	MkDepend -i library/ \
        library/#?.c test-toys/#?.c

#
# Pattern rule to compile C-library into object file
#
.c.o :
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# --- DO NOT MODIFY THIS LINE -- AUTO-DEPENDS FOLLOW ---
library/ubi_AVLtree.o : library/ubi_AVLtree.h library/ubi_BinTree.h \
    library/sys_include.h

library/ubi_BinTree.o : library/ubi_BinTree.h library/sys_include.h

library/ubi_Cache.o : library/ubi_Cache.h library/ubi_SplayTree.h \
    library/ubi_BinTree.h library/sys_include.h

library/ubi_SplayTree.o : library/ubi_SplayTree.h library/ubi_BinTree.h \
    library/sys_include.h

library/ubi_dLinkList.o : library/ubi_dLinkList.h library/sys_include.h

library/ubi_sLinkList.o : library/ubi_sLinkList.h library/sys_include.h

# --- DO NOT MODIFY THIS LINE -- AUTO-DEPENDS PRECEDE ---
# DO NOT DELETE
