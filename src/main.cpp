#include <stdlib.h>
#include <stdio.h>

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;
typedef float f32;

#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    s32 Width;
    s32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitmap;
    s32 HorzResolution;
    s32 VertResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
};
#pragma pack(pop)


int main()
{

    u32 WIDTH = 1920;
    u32 HEIGHT = 1080;

    u32 OutputPixelSize = sizeof(u32) * WIDTH * HEIGHT;
    u32 *OutputPixels = (u32 *)malloc(OutputPixelSize);

    u32 *Out = OutputPixels;

    for(u32 Y = 0;
            Y < HEIGHT;
            ++Y)
    {
        for(u32 X = 0;
                X < WIDTH;
                ++X)
        {
            *Out++ = 0xFF0000FF;
        }
    }

    bitmap_header Header;
    Header.FileType = 0x4D42;
    Header.Reserved1 = 0;
    Header.Reserved2 = 0;
    Header.BitmapOffset = sizeof(Header) - 14;
    Header.Width = WIDTH;
    Header.Height = HEIGHT;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 0;
    Header.SizeOfBitmap = OutputPixelSize;
    Header.HorzResolution = 0;
    Header.VertResolution = 0;
    Header.ColorsUsed = 0;
    Header.ColorsImportant = 0;

    char *OutputFileName = "test.bmp";
    FILE *OutFile = fopen(OutputFileName, "wb");
    if(OutFile)
    {
        fwrite(&Header, sizeof(Header), 1, OutFile);
        fwrite(OutputPixels, OutputPixelSize, 1, OutFile);
        fclose(OutFile);
    }
    else
    {
        fprintf(stderr, "Error while opening a file!");
    }

    return 0;
}
