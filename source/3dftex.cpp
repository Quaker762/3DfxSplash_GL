/**
 *
 */
#include "3dftex.h"
#include <cstdio>

void yiq422_to_rgb888(const GuNccTable* ncc_table, const uint8_t* index_data, std::vector<uint32_t>& data, int num_bytes)
{
    // First, we create a pallette based on the YIQ colours
    // https://github.com/sezero/glide/blob/46226e7d142940f86d4960327e2932e0d21624fe/swlibs/texus/lib/texuslib.c#L296

    for(int i = 0; i < num_bytes; i++)
    {
        int iy, ia, ib, r, g, b;
        
        //std::printf("0x%x\n", index_data[i]);
        iy = (index_data[i] >> 4) & 0xF;
        ia = (index_data[i] >> 2) & 0x3;
        ib = (index_data[i] >> 0) & 0x3;

        r = ncc_table->yRGB[iy] + ncc_table->iRGB[ia][0] + ncc_table->qRGB[ib][0];
        g = ncc_table->yRGB[iy] + ncc_table->iRGB[ia][1] + ncc_table->qRGB[ib][1];
        b = ncc_table->yRGB[iy] + ncc_table->iRGB[ia][2] + ncc_table->qRGB[ib][2];
        
        if (r < 0) r = 0; 
        if (r > 255) r = 255;
        if (g < 0) g = 0; 
        if (g > 255) g = 255;
        if (b < 0) b = 0; 
        if (b > 255) b = 255;

        uint32_t pixel = (r | (g << 8) | (b << 16) | 0xff000000);
        data.push_back(pixel);
                        //OGL_LOAD_CREATE_TEXTURE( 4, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_temp );
    }

    FILE* pFile = fopen("out.raw", "wb");
    fwrite(data.data(), 1, data.size(), pFile);
    fclose(pFile);
}

void pal256_to_rgb88(const GuTexPalette* pal, const uint8_t* index_data, std::vector<uint8_t>& data, int num_bytes)
{
    for(int i = 0; i < num_bytes; i++)
    {
        uint32_t pixel = pal->data[index_data[i]];

        uint8_t r = (pixel >> 16) & 0xff;
        uint8_t g = (pixel >> 8) & 0xff;
        uint8_t b = pixel & 0xff;

        data.push_back(b);
        data.push_back(g);
        data.push_back(r);
        data.push_back(0xff);
    }
}