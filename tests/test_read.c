#include <assert.h>
#include <stdio.h>

#include "../libpxr/pxr.h"

static const char* test_images[] = {
    "tests/pic.andre1.pxr",
    "tests/beebox.pxr",
    "tests/beebox1.pxr",
    "tests/andre.con3.193.pxr",
    "tests/paperclip.pxr",
    "tests/comp.0420.pxr",
    "tests/test_read.c", // invalid Pixar Image File
};

static const int num_tests = sizeof(test_images) / sizeof(*test_images);

void print_pxr_header(pxr_header_t* head)
{
    printf("pxr_header_t {"
           "  .magic = 0x%08x;\n"
           "  .version = %hu;\n"
           "  .label = \"%s\";\n"
           "  .labelptr = 0x%08x;\n"
           "  ._gap = \"%s\";\n"
           "  .pic_height = %hu;\n"
           "  .pic_width = %hu;\n"
           "  .tile_height = %hu;\n"
           "  .tile_width = %hu;\n"
           "  .pic_format = %hu;\n"
           "  .pic_storage = %hu;\n"
           "  .block = %hu;\n"
           "  .alpha = %hu;\n"
           "  .x_offset = %u;\n"
           "  .y_offset = %u;\n"
           "  ._space = \"%s\";\n"
           "  .intspace = 0x%08x;\n"
           "  ._unused = \"%s\";\n"
           "};\n",
        head->magic, head->version, head->label, head->labelptr, head->_gap,
        head->pic_height, head->pic_width, head->tile_height, head->tile_width,
        head->pic_format, head->pic_storage, head->block, head->alpha,
        head->x_offset, head->y_offset, head->_space, head->intspace,
        head->_unused);
}

void print_pxr_image(pxr_image_t* img)
{
    if (img != NULL) {
        print_pxr_header(&img->header);
    }
}

int main(int argc, const char* argv[], const char* envp[])
{
    if (argc > 1) {
        // todo
        (void)argv;
    }
    if (envp != NULL && *envp != NULL) {
        // todo
        (void)envp;
    }
    pxr_image_t* img;

    for (int i=0; i < num_tests; i++) {
        printf("IMAGE_FILE: %s\n", test_images[i]);
        img = pxr_open_file(test_images[i]);
        print_pxr_image(img);
        puts("");
    }
}