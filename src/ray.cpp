#include <stdlib.h>
#include <stdio.h>
#include "ray.h"


static u32
GetTotalPixelSize(image_u32 Image)
{
    u32 Result = Image.Width*Image.Height*sizeof(u32);
    return Result;
}

static image_u32
AllocateImage(u32 Width, u32 Height)
{   
    image_u32 Image = {};
    Image.Width = Width;
    Image.Height = Height;

    u32 OutputPixelSize = GetTotalPixelSize(Image);
    Image.Pixels = (u32 *)malloc(OutputPixelSize);
    
    return Image;
}

static void
WriteImage(image_u32 Image, char *FileName)
{
    u32 OutputPixelSize = GetTotalPixelSize(Image);

    bitmap_header Header = {};
    Header.FileType = 0x4D42;
    Header.FileSize = sizeof(Header) + OutputPixelSize;
    Header.Reserved1 = 0;
    Header.Reserved2 = 0;
    Header.BitmapOffset = sizeof(Header);
    Header.Size = sizeof(Header) - 14;
    Header.Width = Image.Width;
    Header.Height = Image.Height;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 0;
    Header.SizeOfBitmap = OutputPixelSize;
    Header.HorzResolution = 0;
    Header.VertResolution = 0;
    Header.ColorsUsed = 0;
    Header.ColorsImportant = 0;

    FILE *OutFile = fopen(FileName, "wb");
    if(OutFile)
    {
        fwrite(&Header, sizeof(Header), 1, OutFile);
        fwrite(Image.Pixels, OutputPixelSize, 1, OutFile);
        fclose(OutFile);
    }
    else
    {
        fprintf(stderr, "[ERROR] Unable to write output file %s.\n",FileName);
    }


}

int main()
{

    image_u32 Image = AllocateImage(1920, 1080);
    u32 *Out = Image.Pixels;
    for(u32 Y = 0;
            Y < Image.Height;
            ++Y)
    {
        for(u32 X = 0;
                X < Image.Width;
                ++X)
        {
            *Out++ = (Y < 32) ? 0xFFFF0000 : 0xFF0000FF;
        }
    }

    WriteImage(Image, "test.bmp");

    return 0;
}
