# X86 

/* x86 crti.s */
.section .init
.global _init
.type _init, @function
_init:
	push %ebp
	movl %esp, %ebp
	/* gcc will nicely put the contents of crtbegin.o's .init section here. */

.section .fini
.global _fini
.type _fini, @function
_fini:
	push %ebp
	movl %esp, %ebp
	/* gcc will nicely put the contents of crtbegin.o's .fini section here. */

	/* x86 crtn.s */
.section .init
	/* gcc will nicely put the contents of crtend.o's .init section here. */
	popl %ebp
	ret

.section .fini
	/* gcc will nicely put the contents of crtend.o's .fini section here. */
	popl %ebp
	ret

# X64

/* x86_64 crti.s */
.section .init
.global _init
.type _init, @function
_init:
	push %rbp
	movq %rsp, %rbp
	/* gcc will nicely put the contents of crtbegin.o's .init section here. */

.section .fini
.global _fini
.type _fini, @function
_fini:
	push %rbp
	movq %rsp, %rbp
	/* gcc will nicely put the contents of crtbegin.o's .fini section here. */

/* x86_64 crtn.s */
.section .init
	/* gcc will nicely put the contents of crtend.o's .init section here. */
	popq %rbp
	ret

.section .fini
	/* gcc will nicely put the contents of crtend.o's .fini section here. */
	popq %rbp
	ret


# ARM (BPABI)

/* crti.c for ARM - BPABI - use -std=c99 */
typedef void (*func_ptr)(void);

extern func_ptr _init_array_start[0], _init_array_end[0];
extern func_ptr _fini_array_start[0], _fini_array_end[0];

void _init(void)
{
	for ( func_ptr* func = _init_array_start; func != _init_array_end; func++ )
		(*func)();
}

void _fini(void)
{
	for ( func_ptr* func = _fini_array_start; func != _fini_array_end; func++ )
		(*func)();
}

func_ptr _init_array_start[0] __attribute__ ((used, section(".init_array"), aligned(sizeof(func_ptr)))) = { };
func_ptr _fini_array_start[0] __attribute__ ((used, section(".fini_array"), aligned(sizeof(func_ptr)))) = { };

/* Include the list of initialization functions sorted. */
.init_array :
{
    crti.o(.init_array)
    KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .init_array.*)))
    KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .init_array))
    crtn.o(.init_array)
}

/* Include the list of termination functions sorted. */
.fini_array :
{
    crti.o(.fini_array)
    KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .fini_array.*)))
    KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .fini_array))
    crtn.o(.fini_array)
}

# CTOR/DTOR: Using Constructors and Destructors Manually

# Another way to execute the global constructors / destructors is to execute the .ctors / .dtors symbols manually (assuming you have your own ELF loader, see ELF_Tutorial). Once you have 
# loaded each ELF file into memory, and all of the symbols have been resolved and relocated you can use .ctors / .dtors to execute the global constructors / destructors manually 
# (apparently, # the same applies to .init_array and .fini_array). To do this, you must first locate the .ctors / .dtors section headers:

for (i = 0; i < ef->ehdr->e_shnum; i++)
{
    char name[250];
    struct elf_shdr *shdr;

    ret = elf_section_header(ef, i, &shdr);
    if (ret != ELF_SUCCESS)
        return ret;

    ret = elf_section_name_string(ef, shdr, &name);
    if (ret != BFELF_SUCCESS)
        return ret;

    if (strcmp(name, ".ctors") == 0)
    {
        ef->ctors = shdr;
        continue;
    }

    if (strcmp(name, ".dtors") == 0)
    {
        ef->dtors = shdr;
        continue;
    }
}

# Now that you have the .ctors / .dtors section headers, you can resolve each constructor using the following. Note that .ctors / .dtors is a table of pointers (32bit for ELF32 and 64bit for ELF64). Each pointer is a function that must be executed.

typedef void(*ctor_func)(void);

for(i = 0; i < ef->ctors->sh_size / sizeof(void *); i++)
{
    ctor_func func;
    elf64_addr sym = 0;

    sym = ((elf64_addr *)(ef->file + ef->ctors->sh_offset))[i];
    func = ef->exec + sym;
    func();

    /* elf->file is the char * that stores the ELF file that your working with. Could be binary or shared library */
    /* elf->exec is the char * that stores the location in memory that the ELF file have been loaded to, and and reloacted */
}

# Stability Issues
# If you don't call the constructors / destructors that GCC provides, GCC will generate code that will segfault under certain conditions with x86_64. Take this for example:

class A
{
    public: 
        A() {}
};

A g_a;

void foo(void)
{
    A *p_a = &g_a;
    p_a->anything();     // <---- segfault
}
