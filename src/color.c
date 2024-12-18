#include "color.h"

Vec3 unpackRGB(unsigned int packedRGB){
    Vec3 color;
    color.x = (float)((packedRGB >> 16) & 0xFF) / 255.0f;
    color.y = (float)((packedRGB >> 8) & 0xFF) / 255.0f;
    color.z = (float)(packedRGB & 0xFF) / 255.0f;
    
    return color;
}
void writeColour(FILE *ppmFile, Vec3 color){
    // Convert the color components from float (0.0 to 1.0) to int (0 to 255)
    int r = (int)(color.x * 255.0f + 0.5f);
    int g = (int)(color.y * 255.0f + 0.5f);
    int b = (int)(color.z * 255.0f + 0.5f);

    // Clamp the values to ensure they are within [0, 255]
    if (r < 0)   r = 0;
    if (r > 255) r = 255;
    if (g < 0)   g = 0;
    if (g > 255) g = 255;
    if (b < 0)   b = 0;
    if (b > 255) b = 255;

    // Write the color components to the PPM file
    fprintf(ppmFile, "%d %d %d ", r, g, b);

}


int compareColor(const void *a, const void *b)
{
    int a1 = 0, b1 = 0;
    for (int i = 0; i < sizeof(int); i++)
    {
        a1 |= (*((unsigned char*)a + i) & 0x0F) << (i * 8);
        b1 |= (*((unsigned char*)b + i) & 0x0F) << (i * 8);
    }
    
    return (a1 < b1) ? -1 : (b1 < a1) ? 1 : (*((int*)a) < *((int*)b)) ? -1 : (*((int*)a) > *((int*)b)) ? 1 : 0;
}