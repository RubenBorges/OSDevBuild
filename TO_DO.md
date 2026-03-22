# *****DONE********
# Implementing Terminal Scrolling
--------------------------------------
In case the terminal is filled up, it will just go back to the top of the screen. This is unacceptable for normal use. Instead, it should move all rows up one row and discard the upper most, and leave a blank row at the bottom ready to be filled up with characters. Implement this.

# Rendering Colorful ASCII Art
---------------------------------------
Use the existing terminal driver to render some pretty stuff in all the glorious 16 colors you have available. Note that only 8 colors may be available for the background color, as the uppermost bit in the entries by default means something other than background color. You'll need a real VGA driver to fix this. ?

# Calling Global Constructors
---------------------------------------
    Main article: Calling Global Constructors
This tutorial showed a small example of how to create a minimal environment for C and C++ kernels. Unfortunately, you don't have everything set up yet. For instance, C++ with global objects will not have their constructors called because you never do it. The compiler uses a special mechanism for performing tasks at program initialization time through the crt*.o objects, which may be valuable even for C programmers. If you combine the crt*.o files correctly, you will create an _init function that invokes all the program initialization tasks. Your boot.o object file can then invoke _init before calling kernel_main.

# Meaty Skeleton
----------------------------------------------
    Main article: Meaty Skeleton
This tutorial is meant as a minimal example to give impatient beginners a quick hello world operating system. It is deliberately minimal and doesn't show the best practices on how to organize your operating system. The Meaty Skeleton tutorial shows an example of how to organize a minimal operating system with a kernel, room for a standard library to grow, and prepared for a user-space to appear.

# Going Further
-------------------------------------------
    Main article: Going Further on x86
This guide is meant as an overview of what to do, so you have a kernel ready for more features, without actually redesigning it radically when adding them.
Bare Bones II

Make your operating system self-hosting and then complete Bare Bones under your own operating system while following all the instructions. This is a five star exercise and you may need a couple of years to solve it. 