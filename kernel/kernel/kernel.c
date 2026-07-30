#include <stdio.h>

#include <kernel/framebuffer.h>
#include <kernel/tty.h>
#include <kernel/balls.h>

void kernel_main(uint32_t magic, void *multiboot_info)
{
    if (magic != 0x2BADB002)
    {
        for (;;);
    }

    framebuffer_init(multiboot_info);
	terminal_initialize(0x00FFFFFF, 0x00000000);
	// printf("Hello, kernel World!\n\nThis is NatOS!\n\n");
    animate_balls();
}
