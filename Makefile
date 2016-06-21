#====================================================================
#
# Makefile for the ubiqx modules
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

#LIB_UBIQX	= libubiqx.a
#ALL_CFLAGS	= -I modules $(CFLAGS)

#######################################
# SAS/c (Amiga) configuration         #
#######################################
#CC	= ucc
#CFLAGS	=
#RM = delete quiet
#AR	= # unused
#RANLIB	= # unused

#LIB_UBIQX	= ubiqx.lib
#ALL_CFLAGS	= -I modules $(CFLAGS)

#######################################
# Gcc configuration.                  #
#######################################
CC	= gcc
CFLAGS	= -O2
RM	= rm -f
AR	= ar -s
RANLIB	= echo created

LIB_UBIQX	= libubiqx.a
ALL_CFLAGS	= -I modules $(CFLAGS)

#######################################
# SGI IRIX 6 configuration            #
#######################################
#CC	= cc
#CFLAGS	= -n32 -O2
#RM	= rm -f
#AR	= ar
#RANLIB	= echo created # Irix doesn't have ranlib

#LIB_UBIQX	= libubiqx.a
#ALL_CFLAGS	= -I modules $(CFLAGS)

#######################################
# CygWin32 configuration              #
# Thanks to Pete Nelson in Saint Paul #
#######################################
#CC	= gcc
#CFLAGS	= -O2
#RM	= rm -f
#AR	= ar
#RANLIB	= echo created

#LIB_UBIQX	= libubiqx.a
#ALL_CFLAGS	= -I modules $(CFLAGS)

#====================================================================

# ------------ #
# main library #
# ------------ #
OBJ_UBIQX = \
	modules/ubi_AVLtree.o \
	modules/ubi_BinTree.o \
	modules/ubi_SplayTree.o \
	modules/ubi_Cache.o \
	modules/ubi_dLinkList.o \
	modules/ubi_sLinkList.o \
	modules/ubi_SparseArray.o

# ------------- #
# Test programs #
# ------------- #

BIN_TEST_TOYS = \
	test-toys/avl-test \
	test-toys/cache-test \
	test-toys/dll-test \
	test-toys/sll-test \
	test-toys/tree-sample

#
# all: Compile all objects and create all executables
#
all : $(OBJ_UBIQX) $(BIN_TEST_TOYS)

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
	-$(RM) $(OBJ_UBIQX) $(LIB_UBIQX) $(BIN_TEST_TOYS)

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

#
# Perform a little selftest
#
check : $(BIN_TEST_TOYS)
	test-toys/randstrings  >$(TEST_DATA) 20 5 10
	test-toys/avl-test <$(TEST_DATA)

#######################################
# Private rules                       #
#######################################

#
# Update dependencies
#
depend :
	MkDepend -i modules/ modules/#?.c test-toys/#?.c

#
# Pattern rule to compile C-library into object file
#
.c.o :
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# --- DO NOT MODIFY THIS LINE -- AUTO-DEPENDS FOLLOW ---
modules/ubi_AVLtree.o : modules/ubi_AVLtree.h modules/ubi_BinTree.h \
    modules/sys_include.h

modules/ubi_BinTree.o : modules/ubi_BinTree.h modules/sys_include.h

modules/ubi_Cache.o : modules/ubi_Cache.h modules/ubi_SplayTree.h \
    modules/ubi_BinTree.h modules/sys_include.h

modules/ubi_SplayTree.o : modules/ubi_SplayTree.h modules/ubi_BinTree.h \
    modules/sys_include.h

modules/ubi_dLinkList.o : modules/ubi_dLinkList.h modules/sys_include.h

modules/ubi_sLinkList.o : modules/ubi_sLinkList.h modules/sys_include.h

# --- DO NOT MODIFY THIS LINE -- AUTO-DEPENDS PRECEDE ---
# DO NOT DELETE
