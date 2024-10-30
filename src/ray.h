typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;
typedef float f32;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array[0])))

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

struct image_u32
{
    u32 Width;
    u32 Height;
    u32 *Pixels;
};

struct material
{
    f32 Scatter; // 0 is pure diffuse, 1 is pure specular
    v3 EmitColor;
    v3 RefColor;
};

struct plane
{
    v3 N;
    f32 d;
    u32 MatIndex;
};

struct sphere
{
    v3 P;
    f32 r;
    u32 MatIndex;
};

struct world
{
    u32 MaterialCount;
    material *Materials;

    u32 PlaneCount;
    plane *Planes;

    u32 SphereCount;
    sphere* Spheres;
};

