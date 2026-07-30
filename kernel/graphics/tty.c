#include <kernel/framebuffer.h>
#include <kernel/tty.h>

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "ssfn.h"

extern const unsigned char _binary_console_sfn_start[];

static const framebuffer_t *terminal_framebuffer;

uint32_t terminal_fg;
uint32_t terminal_bg;

void ssfn_setup(uint32_t start_x, uint32_t start_y)
{
	ssfn_src = (ssfn_font_t *)_binary_console_sfn_start;
	ssfn_dst.ptr = terminal_framebuffer->fb_base;
	ssfn_dst.p = terminal_framebuffer->fb_pitch;
	ssfn_dst.w = terminal_framebuffer->fb_width;
	ssfn_dst.h = terminal_framebuffer->fb_height;
	ssfn_dst.fg = terminal_fg;
	ssfn_dst.bg = terminal_bg;
	ssfn_dst.x = start_x;
	ssfn_dst.y = start_y;
}

void terminal_clear(uint32_t start_y)
{
	uint32_t *ptr = (uint32_t *)(terminal_framebuffer->fb_base + start_y * terminal_framebuffer->fb_pitch);
	uint32_t count = (terminal_framebuffer->fb_pitch * (terminal_framebuffer->fb_height - start_y)) / 4;

	while (count--) {
		*(ptr++) = terminal_bg;
	}
}

static void terminal_shift(uint32_t font_height)
{
	uint32_t max_row = terminal_framebuffer->fb_height - (terminal_framebuffer->fb_height % font_height) - font_height;
	uint32_t total_pixels = (max_row * terminal_framebuffer->fb_pitch) / 4;

	uint32_t *dst_addr = (uint32_t *)terminal_framebuffer->fb_base;
	uint32_t *src_addr = (uint32_t *)(terminal_framebuffer->fb_base + font_height * terminal_framebuffer->fb_pitch);

	for (uint32_t i = 0; i < total_pixels; i++) {
		*(dst_addr++) = *(src_addr++);
	}

	terminal_clear(max_row);
}

void terminal_initialize(uint32_t fg, uint32_t bg)
{
	terminal_framebuffer = get_framebuffer_info();
	terminal_fg = fg;
	terminal_bg = bg;
	ssfn_setup(0, 0);
	terminal_clear(0);
}

void terminal_putchar(char c)
{
	if (c == '\n') {
		ssfn_dst.x = 0;
		if ((uint32_t)ssfn_dst.y + 2 * ssfn_src->height > terminal_framebuffer->fb_height) {
			terminal_shift(ssfn_src->height);
		} else {
			ssfn_dst.y += ssfn_src->height;
		}
		return;
	}

	if (c == '\r') {
		uint32_t *ptr;

		for (uint32_t y = (uint32_t)ssfn_dst.y; y < (uint32_t)(ssfn_dst.y + ssfn_src->height); y++) {
			ptr = (uint32_t *)(terminal_framebuffer->fb_base + y * terminal_framebuffer->fb_pitch);
			for (uint32_t x = 0; x < (uint32_t)ssfn_dst.x; x++) {
				*(ptr++) = terminal_bg;
			}
		}

		ssfn_dst.x = 0;
		return;
	}

	ssfn_putc(c);

	if ((uint32_t)ssfn_dst.x + ssfn_src->width > terminal_framebuffer->fb_width) {
		ssfn_dst.x = 0;
		ssfn_dst.y += ssfn_src->height;
	}

	if ((uint32_t)ssfn_dst.y + ssfn_src->height > terminal_framebuffer->fb_height) {
		ssfn_dst.x = 0;
		terminal_shift(ssfn_src->height);
		ssfn_dst.y -= ssfn_src->height;
	}
}

void terminal_writestring(const char *s)
{
	while (*s) terminal_putchar(*s++);
}