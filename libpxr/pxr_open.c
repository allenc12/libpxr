/*
 * Copyright something idk
 */

/*
 * Pixar Image Format Library
 * something
 */

#include "pxr.h"
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define PXR_SUCCESS 0
#define PXR_FAILURE 0x1
#define PXR_MALLOC_FAILURE 0x2
#define PXR_READ_OPEN_FAILURE 0x40
#define PXR_READ_BAD_HEADER 0x80

#define NumTiles(hdr)                                                          \
  ((1 + ((hdr).pic_width - 1) / (hdr).tile_width) *                            \
       (1 + ((hdr).pic_height - 1) / (hdr).tile_height) - 1)

int pxr_errno;
#define pxr_log_message(msg, ...)  (_pxr_log_message(__FILE_NAME__, __FUNCTION__, __LINE__, msg, __VA_ARGS__))
int _pxr_log_message(const char* fname, const char* func, int line, const char* fmt, ...) {
    va_list lst;
    int nchrs;

    va_start(lst, fmt);
    nchrs = fprintf(stderr, "%s:%s:%d: ", fname, func, line);
    nchrs += vfprintf(stderr, fmt, lst);
    va_end(lst);
    return nchrs;
}

uint16_t popcount(uint16_t v) {
    v = v - ((v >> 1) & (uint16_t)~(uint16_t)0/3);                           // temp
    v = (v & (uint16_t)~(uint16_t)0/15*3) + ((v >> 2) & (uint16_t)~(uint16_t)0/15*3);      // temp
    v = (v + (v >> 4)) & (uint16_t)~(uint16_t)0/255*15;                      // temp
    return (uint16_t)(v * ((uint16_t)~(uint16_t)0/255)) >> (sizeof(uint16_t) - 1) * CHAR_BIT; // count
}

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
    img->_buf = file;
    int ntiles = NumTiles(img->header);
#ifdef PXR_DEBUG_LOGGING
    if (img->header.label[0] != '\0') {
        fprintf(stdout, "Label: \"%s\"\n", img->header.label);
    }
#endif
    if (ntiles > 1) {
        puts("TODO: multiple tiles");
    }
    tile_ptr_t *tile_ptrs = (tile_ptr_t*)calloc(ntiles, sizeof(*tile_ptrs));
    read_objs = fread(tile_ptrs, sizeof(*tile_ptrs), ntiles, img->_buf);
    if (read_objs != ntiles) {
        pxr_log_message("ntiles != read_objs: %d != %zd\n", ntiles, read_objs);
    }
    for (int i=0; i < ntiles; i++) {
        if ((fseek(file, tile_ptrs[i].tile_ptr, SEEK_SET)) == -1) {
            pxr_errno = PXR_FAILURE;
            goto err;
        }
        // todo: read scanlines
    }
    int pix_size = popcount(img->header.pic_format);
    // 12-bit color channels stored as two bytes
    if (img->header.pic_storage == Encode12 || img->header.pic_storage == Dump12) {
        pix_size = pix_size * 2;
    } else if (img->header.pic_storage == Encode8 || img->header.pic_storage == Dump8) {
        ;
    } else {
        pxr_log_message("WARNING: Unknown pic_storage format: %hu\n", img->header.pic_storage);
    }
    img->_length = img->header.pic_height * img->header.pic_width * pix_size;
    img->_data = (uint8_t*)calloc(1, img->_length);
    if (img->_data == NULL) {
        pxr_log_message("FATAL: calloc failed: %s\n", strerror(errno));
        pxr_errno = PXR_FAILURE | PXR_MALLOC_FAILURE;
        goto err;
    }
    read_objs = fread(img->_data, sizeof(*img->_data), img->_length, file);
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
            perror("fclose() returned EOF, something is very wrong.");
        }
    }
    if (pxr_errno != PXR_SUCCESS) {
        if (img != NULL) {
            if (img->_data != NULL) {
                free(img->_data);
                img->_data = NULL;
            }
            free(img);
            img = NULL;
        }
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
    uint8_t *buf;
    uint16_t width = img->header.pic_width;
    uint16_t height = img->header.pic_height;


    int ret = PXR_SUCCESS;
    if (img == NULL) {
        ret = PXR_FAILURE;
        goto err;
    }

    buf = (uint8_t*)malloc(1);

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