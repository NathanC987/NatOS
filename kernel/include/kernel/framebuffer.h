#ifndef KERNEL_FRAMEBUFFER_H
#define KERNEL_FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

typedef struct framebuffer_t
{
    uint8_t *fb_base;
    uint32_t fb_pitch;
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t  fb_bpp;
} framebuffer_t;

void framebuffer_init(void *multiboot_info);
const framebuffer_t* get_framebuffer_info(void);

#endif
