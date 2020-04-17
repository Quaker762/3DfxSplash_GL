/**
 * 3DF file format structures (from glide)
 */
#pragma once

#include <cstdint>
#include <vector>

typedef unsigned char   FxU8;
typedef unsigned short  FxU16;
typedef short           FxI16;
typedef unsigned int    FxU32;
typedef int             FxI32;
typedef int             FxBool;

typedef FxI32 GrAspectRatio_t;
enum class GrAspectRatio : FxI32
{
    GR_TEXFMT_8BIT                  = 0,               
    GR_TEXFMT_RGB_332               = GR_TEXFMT_8BIT,
    GR_TEXFMT_YIQ_422               = 0x1,
    GR_TEXFMT_ALPHA_8               = 0x2, /* (0..0xFF) alpha     */
    GR_TEXFMT_INTENSITY_8           = 0x3, /* (0..0xFF) intensity */
    GR_TEXFMT_ALPHA_INTENSITY_44    = 0x4,
    GR_TEXFMT_P_8                   = 0x5,
    GR_TEXFMT_RSVD1                 = 0x6,
    GR_TEXFMT_RSVD2                 = 0x7,
    GR_TEXFMT_16BIT                 = 0x8,
    GR_TEXFMT_ARGB_8332             = GR_TEXFMT_16BIT,
    GR_TEXFMT_AYIQ_8422             = 0x9,
    GR_TEXFMT_RGB_565               = 0xa,
    GR_TEXFMT_ARGB_1555             = 0xb,
    GR_TEXFMT_ARGB_4444             = 0xc,
    GR_TEXFMT_ALPHA_INTENSITY_88    = 0xd,
    GR_TEXFMT_AP_88                 = 0xe,
    GR_TEXFMT_RSVD4                 = 0xf
};

typedef FxU32 GrTextureFormat_t;
enum class GrTextureFormat : FxU32
{
    GR_LOD_256      = 0x0,
    GR_LOD_128      = 0x1,
    GR_LOD_64       = 0x2,
    GR_LOD_32       = 0x3,
    GR_LOD_16       = 0x4,
    GR_LOD_8        = 0x5,
    GR_LOD_4        = 0x6,
    GR_LOD_2        = 0x7,
    GR_LOD_1        = 0x8
};

typedef FxI32 GrLOD_t;

/*
** 3DF texture file structs
*/
typedef struct
{
  FxU32               width, height;
  int                 small_lod, large_lod;
  GrAspectRatio_t     aspect_ratio;
  GrTextureFormat_t   format;
} Gu3dfHeader;

typedef struct
{
  FxU8  yRGB[16];
  FxI16 iRGB[4][3];
  FxI16 qRGB[4][3];
  FxU32 packed_data[12];
} GuNccTable;

typedef struct {
    FxU32 data[256];
} GuTexPalette;

typedef union {
    GuNccTable   nccTable;
    GuTexPalette palette;
} GuTexTable;

typedef struct
{
  Gu3dfHeader  header;
  GuTexTable   table;
  void        *data;
  FxU32        mem_required;    /* memory required for mip map in bytes. */
} Gu3dfInfo;


void yiq422_to_rgb888(const GuNccTable* ncc_table, const uint8_t* index_data, std::vector<uint32_t>& data, int num_bytes);
void pal256_to_rgb88(const GuTexPalette* pal, const uint8_t* index_data, std::vector<uint8_t>& data, int num_bytes);