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

struct header_t {
  uint32_t magic;       // 0x00_00_E8_80
  uint16_t version;     // zero for current release
  uint8_t label[246];   // ascii description of image
  uint32_t labelptr;    // ptr to label continuation
  uint8_t gap[160];     // unused space in header info
  uint16_t pic_height;  // pixel height of full image
  uint16_t pic_width;   // pixel width of full image
  uint16_t tile_height; // pixel height of each tile
  uint16_t tile_width;  // pixel width of each tile
  uint16_t pic_format;  // four bits designating RGBA
  uint16_t pic_storage; // encoding and number of bits
  uint16_t block;       // optimum disk transfer chunk
  uint16_t alpha;       // matted-to-black:0 unassoc:1
  uint16_t x_offset;    // horizontal offset of picture
  uint16_t y_offset;    // vertical offset of picture
  uint8_t space[12];    // mystery void in header
  uint32_t intspace;    // unused int space
  uint8_t unused[60];   // unused expansion space
};

enum PacketFlag : uint8_t {
  End = 0,
  FullDmp = 1,
  FullRun = 2,
  PartDmp = 3,
  PartRun = 4
};

typedef struct pix_rgb pix_rgb;
typedef struct pix_rgba pix_rgba;
typedef struct pix_partrun pix_partrun;
typedef struct pix_fullrun pix_fullrun;
typedef struct tile_ptr_t tile_ptr_t;
typedef struct tile_ptr_table_t tile_ptr_table_t
;
enum PicStorage : uint16_t { Encode8 = 0, Encode12 = 1, Dump8 = 2, Dump12 = 3 };

struct pix_rgb {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

struct pix_rgba {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
};

struct pix_partrun {
  uint8_t lambda;
  struct pix_rgb c;
};

struct pix_fullrun {
  uint8_t lambda;
  struct pix_rgba c;
};

struct packet_t {
  uint16_t count : 12;
  uint16_t flag : 4;
};

 struct tile_ptr_t {
  struct packet_t *tile_ptr; // location of tile
  uint32_t tile_len;         // length of tile
};

struct tile_ptr_table_t {
  tile_ptr_t table[4];
};

union pix_data {
  packet_t type;
	pix_rgb rgb;
  pix_rgba rgba;
  pix_partrun prun;
  pix_fullrun frun;
};

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* PXR_H */