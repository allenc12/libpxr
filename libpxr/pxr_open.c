/*
 * Copyright something idk
 */

/*
 * Pixar Image Format Library
 * something
 */

#include "pxr.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PXR_SUCCESS 0
#define PXR_FAILURE 0x1
#define PXR_READ_OPEN_FAILURE 0x40
#define PXR_READ_BAD_HEADER 0x80

#define NumTiles(hdr)                                                          \
  ((1 + ((hdr).pic_width - 1) / (hdr).tile_width) *                            \
       (1 + ((hdr).pic_height - 1) / (hdr).tile_height) -                      \
   1)

int pxr_errno;

pxr_image_t* pxr_open_file(const char* path)
{
    pxr_errno = 0;
    FILE* file = NULL;
    pxr_image_t* img = NULL;

    if (path == NULL) {
        goto err;
    }
    if ((file = fopen(path, "r")) == NULL) {
        goto err;
    }
    if ((img = calloc(1, sizeof(*img))) == NULL) {
        goto err;
    }
    ssize_t read_objs = fread(&img->header, sizeof(img->header), 1, file);
    if (read_objs != 1 || img->header.magic != PXR_HEADER_MAGIC) {
        pxr_errno = PXR_READ_BAD_HEADER;
        goto err;
    }
    int ntiles = NumTiles(img->header);
#ifdef PXR_DEBUG_LOGGING
    if (img->header.label[0] != '\0') {
        fprintf(stdout, "Label: \"%s\"\n", img->header.label);
    }
#endif
    if (ntiles > 1) {
        puts("TODO: multiple tiles");
    }
    switch (img->header.pic_storage) {
    case Dump8:
        pxr_read_dump8_fmt(img);
        break;
    case Encode8:
        pxr_read_encode8_fmt(img);
        break;
    case Dump12:
        pxr_read_dump12_fmt(img);
        break;
    case Encode12:
        pxr_read_encode12_fmt(img);
        break;
    }

err:
    if (file != NULL) {
        if (fclose(file) == EOF) {
            fputs("fclose() returned EOF, something is very wrong.", stderr);
        }
    }
    if (pxr_errno != PXR_SUCCESS) {
        free(img);
        img = NULL;
    }
    return img;
}

int pxr_read_tiles(pxr_image_t* img)
{
    int ret = PXR_SUCCESS;
    if (img == NULL) {
        ret = PXR_FAILURE;
        goto err;
    }

err:
    return ret;
}

int pxr_read_dump8_fmt(pxr_image_t* img)
{
    int ret = PXR_SUCCESS;
    if (img == NULL) {
        ret = PXR_FAILURE;
        goto err;
    }

err:
    return ret;
}

int pxr_read_encode8_fmt(pxr_image_t* img)
{
    int ret = PXR_SUCCESS;
    if (img == NULL) {
        ret = PXR_FAILURE;
        goto err;
    }

err:
    return ret;
}

int pxr_read_dump12_fmt(pxr_image_t* img)
{
    int ret = PXR_SUCCESS;
    if (img == NULL) {
        ret = PXR_FAILURE;
        goto err;
    }

err:
    return ret;
}

int pxr_read_encode12_fmt(pxr_image_t* img)
{
    int ret = PXR_SUCCESS;
    if (img == NULL) {
        ret = PXR_FAILURE;
        goto err;
    }

err:
    return ret;
}