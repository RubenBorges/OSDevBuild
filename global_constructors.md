# If you are using GNU Make, you can do it easily in your makefile assuming $(CC) is your cross-compiler and $(CFLAGS) is the flags you would normally pass it:
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)


# Usage:

OBJS:=foo.o bar.o

CRTI_OBJ=crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=crtn.o

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

myos.kernel: $(OBJ_LINK_LIST)
	$(CC) -o myos.kernel $(OBJ_LINK_LIST) -nostdlib -lgcc

clean:
	rm -f myos.kernel $(INTERNAL_OBJS)

# It is important to remember that the objects must be linked in this exact order, or you will experience strange bugs.

# Your kernel will then have an _init and a _fini function linked in, which can be called from your boot.o (or what your kernel entry point object is called) before passing control to kernel_main (or what your kernel main routine is called). Please note that the kernel may not be initialized, at all, at this point in time and you can only do trivial things from your global constructors. 

# Using crti.o, crtbegin.o, crtend.o, and crtn.o in User-Space:



