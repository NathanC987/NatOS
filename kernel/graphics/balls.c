#include <kernel/balls.h>
#include <kernel/framebuffer.h>

__attribute__((aligned(16)))
static uint8_t backbuffer[MAX_FB_SIZE];

static const framebuffer_t *screenbuffer;

typedef struct Circle
{
    double x;
    double y;
    uint32_t radius;
    uint32_t color;
    double speed_x;
    double speed_y;
} Circle;

static Circle circle1 = {850, 250, 150, 0x00FF0000, -3, 0};
static double circle1_acc = 0.08;

static Circle circle2 = {100, 350, 80, 0x000000FF, 2, 0};
static double circle2_acc = 0.16;

static inline void draw_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x >= screenbuffer->fb_width || y >= screenbuffer->fb_height)
    {
        return;
    }

    uint8_t *row = backbuffer + y * screenbuffer->fb_pitch;
    uint32_t *px = (uint32_t *)(row + x * (screenbuffer->fb_bpp / 8));
    *px |= color;
}

static void buffer_clear(uint32_t bg)
{
    uint32_t *ptr = (uint32_t *)backbuffer;
    uint32_t count = (screenbuffer->fb_pitch * screenbuffer->fb_height) / 4;
    while (count--)
    {
        *(ptr++) = bg;
    }
}

static void draw_horizontal_line(uint32_t x1, uint32_t x2, uint32_t y, uint32_t color)
{
    for (uint32_t x = x1; x <= x2; x++)
    {
        draw_pixel(x, y, color);
    }
}

static void draw_circle(Circle *circle)
{
    uint32_t x0 = circle->x, y0 = circle->y, radius = circle->radius, color = circle->color;

    uint32_t x = 0;
    uint32_t y = radius;
    int32_t d = 3 - (2 * radius);

    while (y >= x)
    {
        draw_horizontal_line(x0 - x, x0 + x, y0 + y, color);
        draw_horizontal_line(x0 - x, x0 + x, y0 - y, color);
        draw_horizontal_line(x0 - y, x0 + y, y0 + x, color);
        draw_horizontal_line(x0 - y, x0 + y, y0 - x, color);

        if (d < 0) {
            d = d + (4 * x) + 6;
        } else {
            d = d + (4 * (x - y)) + 10;
            y--;
        }
        x++;
    }
}

static void update_circle(Circle *circle, double acc)
{
    circle->x += circle->speed_x;
    if (circle->speed_x > 0 && circle->x >= (screenbuffer->fb_width - circle->radius))
    {
        circle->x = screenbuffer->fb_width - circle->radius;
        circle->speed_x = -(circle->speed_x);
    }
    else if (circle->speed_x < 0 && circle->x <= circle->radius)
    {
        circle->x = circle->radius;
        circle->speed_x = -(circle->speed_x);
    }

    circle->speed_y += acc;
    circle-> y += circle->speed_y;
    if (circle->y >= (screenbuffer->fb_height - circle->radius))
    {
        circle->y = screenbuffer->fb_height - circle->radius;
        circle->speed_y = -(circle->speed_y);
    }
}

void animate_balls()
{
    screenbuffer = get_framebuffer_info();

    uint32_t fb_size = screenbuffer->fb_pitch * screenbuffer->fb_height;
    if (fb_size > MAX_FB_SIZE)
    {
        for (;;);
    }

    for (;;)
    {
        buffer_clear(0x00000000);

        draw_circle(&circle2);
        update_circle(&circle2, circle2_acc);

        draw_circle(&circle1);
        update_circle(&circle1, circle1_acc);

        for (uint32_t i = 0; i < fb_size; i++)
        {
            screenbuffer->fb_base[i] = backbuffer[i];
        }
    }
}
