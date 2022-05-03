Shortcuts simplify My Drive … 
In the coming weeks, items in more than one folder will be replaced by shortcuts. Access to files and folders won't change.Learn more
Myint_Thiha_Prog1.c
Who has access

System properties
Type
C
Size
12 KB
Storage used
12 KB
Location
Assignment1
Owner
me
Modified
Mar 16, 2022 by me
Opened
11:14 PM by me
Created
Mar 16, 2022 with Google Drive Web
Add a description
Viewers can download
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved;must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
};
struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
};
unsigned char getColor(unsigned char *data, int width, int x, int y, int ratio, int color)
{
    int bplo =  width * 3;
    if (bplo % 4 != 0)
    {
        bplo = bplo + 4 - (bplo % 4);
    }
    bplo = bplo*y;
    return data[x*3 + bplo + color];
}

void readFile(char *file1, char *file2, float ratio, char *out)
{
    FILE *file = fopen(file1, "rb");
    struct tagBITMAPFILEHEADER fh;
    struct tagBITMAPINFOHEADER fih;
    fread(&fh.bfType, 1, 2, file);
    fread(&fh.bfSize, 1, 4, file);
    fread(&fh.bfReserved1, 1, 2, file);
    fread(&fh.bfReserved2, 1, 2, file);
    fread(&fh.bfOffBits, 1, 4, file);
    fread(&fih, 1, sizeof(struct tagBITMAPINFOHEADER), file);
    unsigned char *data = (unsigned char*)malloc(fih.biSizeImage);
    fread(data, 1, fih.biSizeImage, file);
    fclose(file);

    file = fopen(file2, "rb");
    struct tagBITMAPFILEHEADER fh2;
    struct tagBITMAPINFOHEADER fih2;
    fread(&fh2.bfType, 1, 2, file);
    fread(&fh2.bfSize, 1, 4, file);
    fread(&fh2.bfReserved1, 1, 2, file);
    fread(&fh2.bfReserved2, 1, 2, file);
    fread(&fh2.bfOffBits, 1, 4, file);
    fread(&fih2, 1, sizeof(struct tagBITMAPINFOHEADER), file);
    unsigned char *data2 = (unsigned char*)malloc(fih2.biSizeImage);
    fread(data2, 1, fih2.biSizeImage, file);
    fclose(file);

    DWORD biSizeImagef;
    int height, width;
    if (fih.biSizeImage > fih2.biSizeImage)
    {
        biSizeImagef = fih.biSizeImage;
        width = fih.biWidth;
        height = fih.biHeight;
    }
    else
    {
        biSizeImagef = fih2.biSizeImage;
        width = fih2.biWidth;
        height = fih2.biHeight;
    }
    unsigned char *dataf = (unsigned char*)malloc(biSizeImagef);
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            
            float ratioX;
            float ratioY;
            float dx;
            float dy;
            if (fih.biWidth == fih2.biWidth)
            {
                int bplo = width * 3;
                if (bplo % 4 != 0)
                {
                    bplo = bplo + 4 - (bplo % 4);
                }
                bplo = bplo*y;
                unsigned char red1 = getColor(data, fih.biWidth, x, y, ratio, 2);
                unsigned char red2 = getColor(data2, fih2.biWidth, x, y, ratio, 2);
                unsigned char red_result = red1 * ratio + red2 * (1 - ratio);
                dataf[x*3 + bplo + 2] = red_result;
                

                unsigned char green1 = getColor(data, fih.biWidth, x, y, ratio, 1);
                unsigned char green2 = getColor(data2, fih2.biWidth, x, y, ratio, 1);
                unsigned char green_result = green1 * ratio + green2 * (1 - ratio);
                dataf[x*3 + bplo + 1] = green_result;

                unsigned char blue1 = getColor(data, fih.biWidth, x, y, ratio, 0);
                unsigned char blue2 = getColor(data2, fih2.biWidth, x, y, ratio, 0);
                unsigned char blue_result = blue1 * ratio + blue2 * (1 - ratio);
                dataf[x*3 + bplo + 0] = blue_result;
            }
            else if (fih.biWidth < width)
            {
                 int bplo = fih2.biWidth * 3;
                if (bplo % 4 != 0)
                {
                    bplo = bplo + 4 - (bplo % 4);
                }
                ratioX = x * ((float)fih.biWidth / (float)width);
                ratioY = y * ((float)fih.biHeight / (float)height);
                bplo = bplo*y;
                
                dx = ratioX - (int)ratioX;
                dy = ratioY - (int)ratioY;
                unsigned char red1 = getColor(data, fih.biWidth, (int)ratioX, (int)ratioY + 1, ratio, 2);
                unsigned char red2 = getColor(data, fih.biWidth, (int)ratioX + 1, (int)ratioY + 1, ratio, 2);
                unsigned char red3 = getColor(data, fih.biWidth, (int)ratioX, (int)ratioY, ratio, 2);
                unsigned char red4 = getColor(data, fih.biWidth, (int)ratioX + 1, (int)ratioY, ratio, 2);
                unsigned char redl = red1 * (1- dy) + red3 * (dy);
                unsigned char redr = red2 * (1 - dy)  + red4 * (dy);
                unsigned char red = red1 * (1 - dx) + red2 * (dx); 
                unsigned char red5 = getColor(data2, fih2.biWidth, x, y, ratio, 2);
                unsigned char red_result = red * (1 - ratio) + red5 * (ratio);
                dataf[x*3 + bplo + 2] = red_result;

                unsigned char blue1 = getColor(data, fih.biWidth, (int)ratioX, (int)ratioY + 1, ratio, 0);
                unsigned char blue2 = getColor(data, fih.biWidth, (int)ratioX + 1, (int)ratioY + 1, ratio, 0);
                unsigned char blue3 = getColor(data, fih.biWidth, (int)ratioX, (int)ratioY, ratio, 0);
                unsigned char blue4 = getColor(data, fih.biWidth, (int)ratioX + 1, (int)ratioY, ratio, 0);
                unsigned char bluel = blue1 * (1- dy) + blue3 * (dy);
                unsigned char bluer = blue2 * (1 - dy)  + blue4 * (dy);
                unsigned char blue = blue1 * (1 - dx) + blue2 * (dx); 
                unsigned char blue5 = getColor(data2, fih2.biWidth, x, y, ratio, 0);
                unsigned char blue_result = blue * (1 - ratio) + blue5 * (ratio);
                dataf[x*3 + bplo + 0] = blue_result;

                unsigned char green1 = getColor(data, fih.biWidth, (int)ratioX, (int)ratioY + 1, ratio, 1);
                unsigned char green2 = getColor(data, fih.biWidth, (int)ratioX + 1, (int)ratioY + 1, ratio, 1);
                unsigned char green3 = getColor(data, fih.biWidth, (int)ratioX, (int)ratioY, ratio, 1);
                unsigned char green4 = getColor(data, fih.biWidth, (int)ratioX + 1, (int)ratioY, ratio, 1);
                unsigned char greenl = green1 * (1- dy) + green3 * (dy);
                unsigned char greenr = green2 * (1 - dy)  + green4 * (dy);
                unsigned char green = green1 * (1 - dx) + green2 * (dx); 
                unsigned char green5 = getColor(data2, fih2.biWidth, x, y, ratio, 1);
                unsigned char green_result = green * (1 - ratio) + green5 * (ratio);
                dataf[x*3 + bplo + 1] = green_result; 
            
            }
            else if (fih2.biWidth < width)
            {
                int bplo = fih.biWidth * 3;
                    if (bplo % 4 != 0)
                    {
                        bplo = bplo + 4 - (bplo % 4);
                    }
                bplo = bplo*y;
                ratioX = x * ((float)fih2.biWidth / (float)width);
                ratioY = y * ((float)fih2.biHeight / (float)height);
                dx = ratioX - (int)ratioX;
                dy = ratioY - (int)ratioY;
                unsigned char red1 = getColor(data2, fih2.biWidth, (int)ratioX, (int)ratioY + 1, ratio, 2);
                unsigned char red2 = getColor(data2, fih2.biWidth, (int)ratioX + 1, (int)ratioY + 1, ratio, 2);
                unsigned char red3 = getColor(data2, fih2.biWidth, (int)ratioX, (int)ratioY, ratio, 2);
                unsigned char red4 = getColor(data2, fih2.biWidth, (int)ratioX + 1, (int)ratioY, ratio, 2);
                unsigned char redl = red1 * (1- dy) + red3 * (dy);
                unsigned char redr = red2 * (1 - dy)  + red4 * (dy);
                unsigned char red = red1 * (1 - dx) + red2 * (dx); 
                unsigned char red5 = getColor(data, fih.biWidth, x, y, ratio, 2);
                unsigned char red_result = red * ratio + red5 * (1 - ratio);
                dataf[x*3 + bplo + 2] = red_result;

                unsigned char blue1 = getColor(data2, fih2.biWidth, (int)ratioX, (int)ratioY + 1, ratio, 0);
                unsigned char blue2 = getColor(data2, fih2.biWidth, (int)ratioX + 1, (int)ratioY + 1, ratio, 0);
                unsigned char blue3 = getColor(data2, fih2.biWidth, (int)ratioX, (int)ratioY, ratio, 0);
                unsigned char blue4 = getColor(data2, fih2.biWidth, (int)ratioX + 1, (int)ratioY, ratio, 0);
                unsigned char bluel = blue1 * (1- dy) + blue3 * (dy);
                unsigned char bluer = blue2 * (1 - dy)  + blue4 * (dy);
                unsigned char blue = blue1 * (1 - dx) + blue2 * (dx); 
                unsigned char blue5 = getColor(data, fih.biWidth, x, y, ratio, 0);
                unsigned char blue_result = blue * ratio + blue5 * (1 - ratio);
                dataf[x*3 + bplo + 0] = blue_result;

                unsigned char green1 = getColor(data2, fih2.biWidth, (int)ratioX, (int)ratioY + 1, ratio, 1);
                unsigned char green2 = getColor(data2, fih2.biWidth, (int)ratioX + 1, (int)ratioY + 1, ratio, 1);
                unsigned char green3 = getColor(data2, fih2.biWidth, (int)ratioX, (int)ratioY, ratio, 1);
                unsigned char green4 = getColor(data2, fih2.biWidth, (int)ratioX + 1, (int)ratioY, ratio, 1);
                unsigned char greenl = green1 * (1- dy) + green3 * (dy);
                unsigned char greenr = green2 * (1 - dy)  + green4 * (dy);
                unsigned char green = green1 * (1 - dx) + green2 * (dx); 
                unsigned char green5 = getColor(data, fih.biWidth, x, y, ratio, 1);
                unsigned char green_result = green * ratio + green5 * (1 - ratio);
                dataf[x*3 + bplo + 1] = green_result;
            }
        }
    }
    if (fih.biSizeImage > fih2.biSizeImage)
    {
        file = fopen(out, "wb+");
        fwrite(&fh.bfType, 1, 2, file);
        fwrite(&fh.bfSize, 1, 4, file);
        fwrite(&fh.bfReserved1, 1, 2, file);
        fwrite(&fh.bfReserved2, 1, 2, file);
        fwrite(&fh.bfOffBits, 1, 4, file);
        fwrite(&fih, 1, sizeof(struct tagBITMAPINFOHEADER), file);
        fwrite(dataf, 1, fih.biSizeImage, file);
        fclose(file);
        biSizeImagef = fih.biSizeImage;
        width = fih.biWidth;
        height = fih.biHeight;
    }
    else
    {
        file = fopen(out, "wb+");
        fwrite(&fh2.bfType, 1, 2, file);
        fwrite(&fh2.bfSize, 1, 4, file);
        fwrite(&fh2.bfReserved1, 1, 2, file);
        fwrite(&fh2.bfReserved2, 1, 2, file);
        fwrite(&fh2.bfOffBits, 1, 4, file);
        fwrite(&fih2, 1, sizeof(struct tagBITMAPINFOHEADER), file);
        fwrite(dataf, 1, fih2.biSizeImage, file);
        fclose(file);

    }
}

int main(int argc, char *argv[])
{
    if (argc > 5)
    {
        printf("\nToo many arguments. Please Enter:");
        printf("[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");
        return 0;
    } 
    if (argc < 5)
    {
        printf("\nNot enough arguments. Please Enter:");
        printf("[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");
        return 0;
    }  
    if (argv[1] == 0 || argv[2] == 0)
    {
        printf("\nFile not found. Please Enter:");
        printf("[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");
        return 0;
    }
    char b[] = ".bmp"; 
    if (!(strstr(argv[1], b)) || !(strstr(argv[2], b)) || !(strstr(argv[4], b)))
    {
        printf("\nNot a .bmp file. Please Enter:");
        printf("[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");
        return 0;
    }   

    readFile(argv[1], argv[2], atof(argv[3]), argv[4]);
    return 0;
}
