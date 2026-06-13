/*Build With:
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*Check if the compiler thinks you are targeting the wrong system.*/
#if defined (__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/*This tutorial will only work for the 32-bit ix86 targets */
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif 

/*Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

// Inline assembly to write to x86 I/O ports (needed for hardware cursor)
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory" );
}

#define VGA_WIDTH 80
#define VGA_HEIGHT 25 
#define VGA_MEMORY (uint16_t*) 0xB8000
static size_t terminal_row;
static size_t terminal_column;
static size_t totalTerminalChars = VGA_WIDTH * VGA_HEIGHT;
static uint8_t terminal_color;  
static uint16_t* terminal_buffer = (uint16_t*) VGA_MEMORY;

// Updates the blinking hardware cursor on the monitor
void terminal_update_cursor(void) {
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_initialized(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_update_cursor();
} 

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
} 

void terminal_scroll(void) {
    for (size_t i = 0; i < totalTerminalChars - VGA_WIDTH; i++) {
        terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
    }
    for (size_t i = totalTerminalChars - VGA_WIDTH; i < totalTerminalChars; i++) {
        terminal_buffer[i] = vga_entry(' ', terminal_color);
    }
}

#define TAB_WIDTH 4
void terminal_putchar(char c) {
    // 1. Handle Backspace
    if (c == '\b') {
        if (terminal_column > 0) {
            --terminal_column;
        } else if (terminal_row > 0) {
            --terminal_row;
            terminal_column = VGA_WIDTH - 1;
        }
        // Clear the character at the new cursor position
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        return; 
    }

    // 2. Handle Newline
    if (c == '\n') {
        terminal_column = 0;
        ++terminal_row;
    } 
    // 3. Handle Tab
    else if (c == '\t') {
        // Calculate spaces needed to reach the next tab stop
        size_t spaces_to_add = TAB_WIDTH - (terminal_column % TAB_WIDTH);
        
        for (size_t i = 0; i < spaces_to_add; i++) {
            // If tab forces a line wrap, handle it gracefully
            if (terminal_column == VGA_WIDTH) {
                terminal_column = 0;
                ++terminal_row;
                if (terminal_row == VGA_HEIGHT) {
                    terminal_scroll();
                    terminal_row = VGA_HEIGHT - 1;
                }
            }
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
            ++terminal_column;
        }
    }
    // 4. Handle Regular Characters
    else {
        if (terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            ++terminal_row;
        }

        if (terminal_row == VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }

        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        ++terminal_column; 
    }

    // Final scroll check for newline or tab row overflows
    if (terminal_row == VGA_HEIGHT) {
        terminal_scroll();
        terminal_row = VGA_HEIGHT - 1;
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
    terminal_update_cursor(); // Update cursor once per string write block for performance
}   

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}   

void kernel_main(void) {
    /*Initialize Terminal Interface*/
    terminal_initialized();
    terminal_writestring("Hello, kernel World!\n");
    terminal_writestring("Testing scrolling mechanics...\n");

    terminal_writestring("Testing Tabs:\n");
    terminal_writestring("1\t2\t3\t4\n");
    terminal_writestring("100\t200\t300\t400\n\n");
    
    terminal_writestring("Testing Backspace:\n");
    terminal_writestring("Delete this X\b"); // The 'X' will disappear
};