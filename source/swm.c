#include <stdio.h>

#include <xcb/xcb.h>

static struct xcb_screen_t const* screen = NULL;

static struct xcb_screen_t const* swm_get_screen_info(struct xcb_connection_t* xcb_connection)
{
    struct xcb_setup_t const*    xcb_setup = xcb_get_setup(xcb_connection);
    struct xcb_screen_iterator_t iterator  = xcb_setup_roots_iterator(xcb_setup);
    struct xcb_screen_t*         result    = iterator.data;
    return result;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    struct xcb_connection_t* xcb_connection = xcb_connect(NULL, NULL);

    screen = swm_get_screen_info(xcb_connection);

    printf("\n");
    printf("Informations of screen %ud:\n", screen->root);
    printf("  width.........: %d\n", screen->width_in_pixels);
    printf("  height........: %d\n", screen->height_in_pixels);
    printf("  white pixel...: 0x%X\n", screen->white_pixel);
    printf("  black pixel...: 0x%X\n", screen->black_pixel);
    printf("\n");

    xcb_disconnect(xcb_connection);
    return 0;
}
