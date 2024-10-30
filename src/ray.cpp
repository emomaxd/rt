#include <stdlib.h>
#include <stdio.h>
#include <limits>

#include "math.h"
#include "ray.h"

/* Color unpacking macros */
#define RED(color)   ((color >> 16) & 0xFF)
#define GREEN(color) ((color >> 8) & 0xFF)
#define BLUE(color)  (color & 0xFF)
#define ALPHA(color) ((color >> 24) & 0xFF)

/* Pack color from ARGB components */
static u32
PackColor(int a, int r, int g, int b) {
    return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

static u32
PackColor(v3 v) {
    int r = static_cast<int>(v.x * 255.0f) & 0xFF;
    int g = static_cast<int>(v.y * 255.0f) & 0xFF;
    int b = static_cast<int>(v.z * 255.0f) & 0xFF;
    return ((0xFF) << 24) | (r << 16) | (g << 8) | b;
}

static u32
BGRAPackColor4x8(v4 C)
{
    int r = static_cast<int>(C.x) & 0xFF;
    int g = static_cast<int>(C.y) & 0xFF;
    int b = static_cast<int>(C.z) & 0xFF;
    int a = static_cast<int>(C.w) & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | b;
}
static f32
ExactLinearTosRGB(f32 L)
{
    if (L < 0.0f)
    {
        L = 0.0f;
    }

    if (L > 1.0f)
    {
        L = 1.0f;
    }

    f32 S = L * 12.92f;
    if (L > 0.0031308f)
    {
        S = 1.055f * std::pow(L, 1.0f / 2.4f) - 0.055f;
    }

    return S;
}

static v4
Linear1ToSRGB255(v4 C)
{
    v4 Result;

    f32 One255 = 255.0f;

    Result.x = One255 * std::sqrt(C.x);
    Result.y = One255 * std::sqrt(C.y);
    Result.z = One255 * std::sqrt(C.z);
    Result.w = One255 * std::sqrt(C.w);

    return Result;
}

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

static f32
RandomUnilateral(void)
{
    f32 Result = (f32)rand() / (f32)RAND_MAX;
    return Result;
}

static f32
RandomBilateral(void)
{
    f32 Result = -1.0f - 2.0f * RandomUnilateral();
    return Result;
}

static v3
RayCast(world* World, v3 RayOrigin, v3 RayDirection)
{
    f32 MinHitDistance = 0.001f;

    f32 Tolerance = 0.0001f;

    v3 Result = {};
    v3 Attenuation = v3(1, 1, 1);
    for(u32 RayCount = 0;
            RayCount < 8;
            ++RayCount)
    {
        f32 HitDistance = std::numeric_limits<float>::max();
        
        v3 Color;
        u32 HitMatIndex = 0;
        v3 NextOrigin;
        v3 NextNormal;
    
        for(u32 PlaneIndex = 0;
                PlaneIndex < World->PlaneCount;
                ++PlaneIndex)
        {
            plane Plane = World->Planes[PlaneIndex];
            
            f32 Denom = Plane.N.dot(RayDirection);
            if (std::abs(Denom) > Tolerance)
            {
                f32 t = (- Plane.d - Plane.N.dot(RayOrigin)) / Denom;
                //f32 ThisDistance = RayIntersectsPlane(RayOrigin, RayDirection, Plane.N, Plane.d);
                if ((t > MinHitDistance) && (t < HitDistance))
                {
                    HitDistance = t;
                    HitMatIndex = Plane.MatIndex;

                    //NextOrigin = RayOrigin + t * RayDirection;
                    NextNormal = Plane.N;
                }
            }
        }

        for(u32 SphereIndex = 0;
                SphereIndex < World->SphereCount;
                ++SphereIndex)
        {
            sphere Sphere = World->Spheres[SphereIndex];
            
            v3 SphereRelativeRayOrigin = RayOrigin - Sphere.P;
            f32 a = RayDirection.dot(RayDirection);
            f32 b = 2.0f * RayDirection.dot(SphereRelativeRayOrigin);
            f32 c = SphereRelativeRayOrigin.dot(SphereRelativeRayOrigin) - (Sphere.r * Sphere.r);

            f32 Denom = 2.0f * a;
            f32 RootTerm = std::sqrt(b * b - 4.0f * a * c);
            if (RootTerm > Tolerance)
            {
                f32 tp = (-b + RootTerm) / Denom;
                f32 tn = (-b - RootTerm) / Denom;

                f32 t = tp;
                if ((tn > MinHitDistance) && (tn < tp))
                {
                    t = tn;
                }

                if ((t > MinHitDistance) && (t < HitDistance))
                {
                    HitDistance = t;
                    HitMatIndex = Sphere.MatIndex;

                    //NextOrigin = RayOrigin + t * RayDirection;
                    NextNormal = (t * RayDirection + SphereRelativeRayOrigin).normalize();
                }
            }
        }

        if(HitMatIndex)
        {
            material Mat = World->Materials[HitMatIndex];
            
            Result += Attenuation.hadamard(Mat.EmitColor);
            f32 CosAtten = CosAtten = (RayDirection * -1.0f).dot(NextNormal);
            if (CosAtten < 0)
            {
                CosAtten = 0;
            }
            Attenuation = Attenuation.hadamard(CosAtten * Mat.RefColor);
            
            RayOrigin += HitDistance * RayDirection;

            v3 PureBounce = RayDirection - 2.0f * RayDirection.dot(NextNormal) * NextNormal;
            v3 RandomBounce = (NextNormal + v3(RandomBilateral(), RandomBilateral(), RandomBilateral())).normalize();
            RayDirection = RandomBounce.lerp(PureBounce, Mat.Scatter).normalize();
            //RayDirection = PureBounce.normalize();
        }
        else
        {
            material Mat = World->Materials[HitMatIndex];
            Result += Attenuation.hadamard(Mat.EmitColor);

            break;
        }
    }

    return Result;
}



int main()
{
    image_u32 Image = AllocateImage(1920, 1080);
    
    material Materials[7] = {};
    Materials[0].EmitColor = v3(0.3f, 0.4f, 0.5f);
    
    Materials[1].RefColor = v3(0.5f, 0.5f, 0.5f);

    Materials[2].RefColor = v3(0.7f, 0.5f, 0.3f);
    
    Materials[3].EmitColor = v3(4.0f, 0.0f, 0.0f);

    Materials[4].RefColor = v3(0.2f, 0.8f, 0.2f);
    Materials[4].Scatter = 0.7f;

    Materials[5].RefColor = v3(0.4f, 0.8f, 0.9f);
    Materials[5].Scatter = 0.85f;

    Materials[6].RefColor = v3(0.95f, 0.95f, 0.95f);
    Materials[6].Scatter = 0.1f;

    plane Planes[1] = {};
    Planes[0].N = v3(0, 0, 1);
    Planes[0].d = 0;
    Planes[0].MatIndex = 1;
    
    sphere Spheres[5] {};
    Spheres[0].P = v3(0, 0, 0); 
    Spheres[0].r = 1.0f;
    Spheres[0].MatIndex = 2;

    Spheres[1].P = v3(3, -2, 0); 
    Spheres[1].r = 1.0f;
    Spheres[1].MatIndex = 3;

    Spheres[2].P = v3(-2, -1, 2); 
    Spheres[2].r = 1.0f;
    Spheres[2].MatIndex = 4;

    Spheres[3].P = v3(1, -1, 3); 
    Spheres[3].r = 1.0f;
    Spheres[3].MatIndex = 5;

    Spheres[4].P = v3(-2, 3, 0); 
    Spheres[4].r = 2.0f;
    Spheres[4].MatIndex = 6;
    
    world World = {};
    
    World.MaterialCount = ArrayCount(Materials);
    World.Materials = Materials;
    
    World.PlaneCount = ArrayCount(Planes);
    World.Planes = Planes;

    World.SphereCount = ArrayCount(Spheres);
    World.Spheres = Spheres;

    v3 CameraP{0, -10, 1};
    v3 CameraZ = CameraP.normalize();
    v3 CameraX = v3{0, 0 ,1}.cross(CameraZ).normalize();
    v3 CameraY = CameraZ.cross(CameraX).normalize();

    f32 FilmDist = 1.0f;
    f32 FilmW = 1.0f;
    f32 FilmH = 1.0f;
    if (Image.Width > Image.Height)
    {
        FilmH = FilmW * ((f32)Image.Height / (f32)Image.Width);
    }
    else if (Image.Height > Image.Width)
    {
        FilmW = FilmH * ((f32)Image.Width / (f32)Image.Height);
    }

    f32 HalfFilmW = 0.5 * FilmW;
    f32 HalfFilmH = 0.5 * FilmH;
    v3 FilmCenter = CameraP - FilmDist * CameraZ;
    
    f32 HalfPixW = 0.5f / Image.Width;
    f32 HalfPixH = 0.5f / Image.Height;

    u32 RaysPerPixel = 256;
    u32 *Out = Image.Pixels;
    for(u32 Y = 0;
            Y < Image.Height;
            ++Y)
    {
        f32 FilmY = -1.0f + 2.0f * ((f32)Y / (f32)Image.Height);
        for(u32 X = 0;
                X < Image.Width;
                ++X)
        {
            f32 FilmX = -1.0f + 2.0f * ((f32)X / (f32)Image.Width);
            
            v3 Color = {};
            f32 Contrib = 1.0f / (f32)RaysPerPixel;
            for (u32 RayIndex = 0;
                    RayIndex < RaysPerPixel;
                    ++RayIndex)
            {
                f32 OffX = FilmX + RandomBilateral() * HalfPixW;
                f32 OffY = FilmY + RandomBilateral() * HalfPixH;
                v3 FilmP = FilmCenter + (FilmX * HalfFilmW * CameraX) + (FilmY * HalfFilmH * CameraY);

                v3 RayOrigin = CameraP;
                v3 RayDirection = (FilmP - CameraP).normalize();

                Color += Contrib * RayCast(&World, RayOrigin, RayDirection);
            }
            
            v4 BMPColor =
            {
                255.0f * ExactLinearTosRGB(Color.x),
                255.0f * ExactLinearTosRGB(Color.y),
                255.0f * ExactLinearTosRGB(Color.z),
                255.0f
            };
            u32 BMPValue = BGRAPackColor4x8(BMPColor); 

            *Out++ = BMPValue; // (Y < 32) ? 0xFFFF0000 : 0xFF0000FF;
        }
        if (!(Y % 64))
        {
            printf("\rRaycasting %d%%...    ", 100*Y / Image.Height);
            fflush(stdout);
        }
    }

    WriteImage(Image, const_cast<char*>("test.bmp"));
    
    printf("\nDone\n");
    return 0;
}
