#include <kernel/framebuffer.h>

static framebuffer_t terminal_framebuffer = {0};

static inline uint32_t mbi_u32(const void *mbi, size_t off)
{
    return *(const uint32_t *)((const uint8_t *)mbi + off);
}

static inline uint64_t mbi_u64(const void *mbi, size_t off)
{
    return *(const uint64_t *)((const uint8_t *)mbi + off);
}

static inline uint8_t mbi_u8(const void *mbi, size_t off)
{
    return *(const uint8_t *)((const uint8_t *)mbi + off);
}

void framebuffer_init(void *multiboot_info)
{
    uint32_t flags = mbi_u32(multiboot_info, 0);
    // Bit 12 means framebuffer info is present
    if ((flags & (1u << 12)) == 0)
    {
        __asm__ volatile("cli; 1: hlt; jmp 1b;");
    }

    uint8_t fb_type = mbi_u8(multiboot_info, 109);
    uint8_t fb_bpp = mbi_u8(multiboot_info, 108);
    // Start only with direct RGB framebuffers
    if (fb_type != 1 || fb_bpp != 32)
    {
        __asm__ volatile("cli; 1: hlt; jmp 1b;");
    }

    terminal_framebuffer.fb_base   = (uint8_t *)(uintptr_t)mbi_u64(multiboot_info, 88);
    terminal_framebuffer.fb_pitch  = mbi_u32(multiboot_info, 96);
    terminal_framebuffer.fb_width  = mbi_u32(multiboot_info, 100);
    terminal_framebuffer.fb_height = mbi_u32(multiboot_info, 104);
    terminal_framebuffer.fb_bpp    = fb_bpp;
}

const framebuffer_t* get_framebuffer_info(void)
{
    return &terminal_framebuffer;
}
