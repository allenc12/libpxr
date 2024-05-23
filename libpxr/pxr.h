/*
 * Copyright something idk
 */

/*
 * Pixar Image Format Library
 * something
 */

#ifndef PXR_H
#define PXR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PXR_HEADER_MAGIC 0x0000E880u
#define PXR_LABEL_SIZE 246
#define _GAP 160
#define _PADDING 12
#define _UNUSED 60

extern int pxr_errno;

typedef struct pix_rgbs pix_rgb;
typedef struct pix_rgba_s pix_rgba;
typedef struct pix_partrun_s pix_partrun;
typedef struct pix_fullrun_s pix_fullrun;
typedef struct tile_ptr_s tile_ptr_t;
typedef struct tile_ptr_table_s tile_ptr_table_t;
typedef struct pxr_header_s pxr_header_t;
typedef struct packet_s packet_t;

struct pxr_header_s {
    uint32_t magic; // 0x00_00_E8_80
    uint16_t version; // zero for current release
    uint8_t label[PXR_LABEL_SIZE]; // ascii description of image
    uint32_t labelptr; // ptr to label continuation
    uint8_t _gap[_GAP]; // unused space in header info
    uint16_t pic_height; // pixel height of full image
    uint16_t pic_width; // pixel width of full image
    uint16_t tile_height; // pixel height of each tile
    uint16_t tile_width; // pixel width of each tile
    uint16_t pic_format; // four bits designating RGBA
    uint16_t pic_storage; // encoding and number of bits
    uint16_t block; // optimum disk transfer chunk
    uint16_t alpha; // matted-to-black:0 unassoc:1
    uint16_t x_offset; // horizontal offset of picture
    uint16_t y_offset; // vertical offset of picture
    uint8_t _space[_PADDING]; // mystery void in header
    uint32_t intspace; // unused int space
    uint8_t _unused[_UNUSED]; // unused expansion space
    uint8_t *tile_ptr_table;
};

enum PacketFlag : uint8_t {
    End = 0,
    FullDmp = 1,
    FullRun = 2,
    PartDmp = 3,
    PartRun = 4
};

enum PicStorage : uint16_t { Encode8 = 0,
    Encode12 = 1,
    Dump8 = 2,
    Dump12 = 3 };

struct pix_rgbs {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct pix_rgba_s {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct pix_partrun_s {
    uint8_t lambda;
    struct pix_rgbs c;
};

struct pix_fullrun_s {
    uint8_t lambda;
    struct pix_rgba_s c;
};

struct packet_s {
    uint16_t count : 12;
    uint16_t flag : 4;
};

struct tile_ptr_s {
    struct packet_s* tile_ptr; // location of tile
    uint32_t tile_len; // length of tile
};

struct tile_ptr_table_s {
    tile_ptr_t table[4];
};

struct pix_data {
    packet_t type;
    union {
        pix_rgb rgb;
        pix_rgba rgba;
        pix_partrun prun;
        pix_fullrun frun;
    };
};

typedef struct __sFILE FILE;

typedef struct pxr_image_s {
    FILE* _buf;
    pxr_header_t header;
    uint8_t* _data;
} pxr_image_t;

pxr_image_t* pxr_open_file(const char* path);
void pxr_close(pxr_image_t* img);

int pxr_read_dump8_fmt(pxr_image_t* img);
int pxr_read_encode8_fmt(pxr_image_t* img);
int pxr_read_dump12_fmt(pxr_image_t* img);
int pxr_read_encode12_fmt(pxr_image_t* img);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* PXR_H */