/*
 * Copyright something idk
 */

/*
 * Pixar Image Format Library
 * something
 */
/*
**
**  FORMAT
**	PIXAR picture file
**
**  AKA
** 	pic, picio, pixar
**
**  FORMAT REFERENCES
**	- PIXAR Image Computer Programmer's Manual, PIXAR.
**	- PIXAR Image Computer ChapLibraries User's Guide, PIXAR.
**	- The RenderMan Companion, Steve Upstill, PIXAR.
**
**
**  CODE CREDITS
**	Custom development, Jim McLeod, San Diego Supercomputer Center, 1992.
**
**  DESCRIPTION
**
**  	The Pixar picture format accomodates several formats:
**	multiple channels; different number of bits;
**	encoded and dumped format; and arbitrary picture size.
**
**  	Large pictures can be handled by breaking the picture into
**  	smaller uniform rectangular pieces called " TILES ."
**
**  	Multiple byte data is stored with the LEAST SIGNIFICANT 8 bits
**	in the first byte ( ie: the first 4 bytes of each file are
**					0x80, 0xe8, 0x00, 0x00	   )
**
**
**
**  PIXAR PICTURE IMAGE STORAGE
**	Header:
**		byte number	#bytes		name
**		  000		  4	  magic number = 0x00, 0x00, 0xe8, 0x80
**		  004		  2	  version number = 0
**		  006		 246	  label
**		  252		  4	  labelptr - for continuation
**					  gap between data
**		  416		  2	  picture height (+ num)
**		  418		  2	  picture width (+ num)
**		  420		  2	  tile height (+ num, <= pic h)
**		  422		  2	  tile width (+ num, <= pic w)
**		  424		  2	  picture format
**		  426		  2	  picture storage
**		  428		  2	  blocking factor
**		  430		  2	  alpha mode (mat-to-black=0,unassoc.=1)
**		  432		  2	  x offset
**		  434		  2	  y offset
**					  space between data
**		  448		  4	  unused
**		  452		 28	  unused
**		  512		 8*n	  tile pointer table (n = # of tiles)
**
**	Label:	Ascii description - labels can be arbitrarily long as the
**		label pointer in the header points to any continuation
**		( allocated in chunks the size of the blocking factor, with
**		last four bytes of this	chunk reserved for further blocks )
**
**	Pic Format:	Any subset of RGBA channels. Single channel "R pictures"
**		are recovered as grey scale pictures. RGBA channels correspond
**		to bits 3210; so that, as an example:
**				RGB selection has 1110 binary.
**
**	Pic Storage:	Four modes:	(0)	8-bit "encoded"
**					(1)	12-bit "encoded"
**					(2)	8-bit dumped
**					(3)	12-bit dumped
**			(note: 12 bits stored as two bytes)
**
**	Encoded Tiles:	Pixel information is broken into "packets."
**		NO packet may span multiple scanlines.
**		NO packet spans multiple disk blocks.
**		HOWEVER, each scanline may have ANY COMBINATION of the
**			four types of packets.
**
**		flag	count	    RGBA	   RGB	 	  R
**		 1	 c	RGBARGBARGBA.	RGBRGBRGB..	RRRR..
**		 2	 c	lRGBAlRGBA...	lRGBlRGB... 	lRlR..
**		 3	 c	ARGBRGBRGB... 	   n/a		 n/a
**		 4	 c	AlRGBlRGB....	   n/a		 n/a
**
**		 0				signifies end of block
**
**		flag and count are packed into 16 bits as follows:
**			1st byte:	count<0:7>
**			2nd byte:	flag<0:3>	count<8:11>
**
**		if flag = 1 or 3: c = 1 less than the number of dummped pixels
**		if flag = 2 or 4: c = 1 less than the number of run lengths
**				    and l = number of repetitions
**				   (ie: l=0 indicates 1 instance 0 repetitions)
**		Zeros fill out a block.
**
**	Dummped Tiles:	No excess bytes are used.
**			(ex: RGB format: RGBRGBRGBRGB.....)
**
**	Blocking Factor:  Optimum disk transfer chunk ( normally 8192 bytes )
**
**	Alpha mode: Has something to do with background of image but does not
**			influence the current implementation of this im utility.
**
**	Picture Offsets: For proper restoration of image on Pixar buffer window.
**
**	Tiles:  Each tile has a four byte pointer and four byte length.
**		Tiles are numbered across from
**			0 to (num_x_tiles*num_y_tiles -1 )
**			where num_x_tiles is 1+(pic_width-1)/tile_width
**			and   num_y_tiles is 1+(pic_height-1)/tile_height.
**
**		Tile 0 is in the UPPER LEFT CORNER:  pic_pixel(0,0)=tile_0(0,0).
**		Tiles can extend down or to the right beyond the picture
**		boundaries, but the pixels outside the picture and inside the
**		tile are undefined with regard to the picture.
**			 ( note: they are still properly encoded.)
**		A tile pointer of 0 indicates null tile, a positive
**		pointer and a count of -1 indicates an incomplete tile.
**
**		( IMPLEMENTATION NOTE:  Tiles have NOT been handled by the
**		  initial revision of the READ routine.  It is assumed that
**		  all PIC files will only have ONE tile which is of the same
**		  dimension of the picture.
**		  For the WRITE routine, the PIC file created will only have
**		  one tile which is the entire image.  )
**
**
*/

#ifndef PXR_H
#define PXR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

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
    // struct packet_s* tile_ptr; // location of tile
    uint32_t tile_ptr; // location of tile
    uint32_t tile_len; // length of tile
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
    size_t _length;
    tile_ptr_t *tile_ptr_table;
    uint8_t* _data;
} pxr_image_t;

pxr_image_t* pxr_open_file(const char* path);
void pxr_close(pxr_image_t* img);

int pxr_read_dump8_fmt(pxr_image_t* img);
int pxr_read_dump12_fmt(pxr_image_t* img);
int pxr_read_encode8_fmt(pxr_image_t* img);
int pxr_read_encode12_fmt(pxr_image_t* img);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* PXR_H */