Shortcuts simplify My Drive … 
In the coming weeks, items in more than one folder will be replaced by shortcuts. Access to files and folders won't change.Learn more
Midterm_Thiha_Myint.c
Who has access

System properties
Type
C
Size
4 KB
Storage used
4 KB
Location
Midterm
Owner
me
Modified
Mar 16, 2022 by me
Opened
11:08 PM by me
Created
Mar 16, 2022 with Google Drive Web
Add a description
Viewers can download
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>

#pragma pack(1)

typedef struct BMPFILEHEADER {
    unsigned short bfType;      /* Magic number for file. Must be in "MB" */
    unsigned int bfSize;        /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits;     /* Offset to bitmap data */
} BMPFILEHEADER;

typedef struct BMPINFOHEADER {
    unsigned int biSize;         /* Size of info header */
    int biWidth;                 /* Width of image */
    int biHeight;                /* Height of image */
    unsigned short biPlanes;     /* Number of color planes */
    unsigned short biBitCount;   /* Number of bits per pixel */
    unsigned int biCompression;  /* Type of compression to use */
    unsigned int biSizeImage;    /* Size of image data */
    int biXPelsPerMeter;         /* X pixels per meter */
    int biYPelsPerMeter;         /* Y pixels per meter */
    unsigned int biClrUsed;      /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} BMPINFOHEADER;

typedef struct col {
    int r, g, b;
} col;

typedef struct compressedformat {
    int width, height;
    int rowbyte_quarter[4];
    int palettecolors;
    col* colors;
  //  colors = (col*) malloc(palettecolors * sizeof(int));
} compressedformat;

typedef struct chunk {
	char color_index;
	short count;
} chunk;

BMPFILEHEADER* get_bmp_file_header() {
    BMPFILEHEADER* header = (BMPFILEHEADER*) malloc(sizeof(BMPFILEHEADER));
    header->bfType = 19778;
    header->bfSize = 4320054;
    header->bfReserved1 = 0;
    header->bfReserved2 = 0;
    header->bfOffBits = 54;
    return header;
}

BMPINFOHEADER* get_bmp_info_header() {
    BMPINFOHEADER* header = (BMPINFOHEADER*) malloc(sizeof(BMPINFOHEADER));
    header->biSize = 40;
    header->biWidth = 1200;
    header->biHeight = 1200;
    header->biPlanes = 1;
    header->biBitCount = 24;
    header->biCompression = 0;
    header->biSizeImage = 4320000;
    header->biXPelsPerMeter = 3780;
    header->biYPelsPerMeter = 3780;
    header->biClrUsed = 0;
    header->biClrImportant = 0;
    return header;
}

long get_file_size(char file_name[]) {
	FILE* fp = fopen(file_name, "r");
	fseek(fp, 0L, SEEK_END);
    long res = ftell(fp); 
	fclose(fp); 
	return res; 
} 

int main(int argc, char const *argv[]) {
	
    char compressed_file[] = "compressed.bin";
    long file_size = get_file_size(compressed_file);
    
    int fd = open(compressed_file, O_RDWR);

    char* file = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);

	compressedformat* obj = (compressedformat*) &file[0];

    col* colors = (col*) &file[sizeof(int) * 7];

    chunk* cnk = (chunk*) &file[(sizeof(int) * 7) + (sizeof(col) * obj->palettecolors)];

    clock_t time = clock();

    char* out_mem = mmap(0, obj->width*obj->height*3, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    for (int p = 0; p < 4; p++) {
        if (fork() == 0) {
            int offset = (obj->width*obj->height*3/4)*p;
            for (int i = p==0 ? 0 : (obj->rowbyte_quarter[p-1]/3); i < (obj->rowbyte_quarter[p]/3); i++) {
                for (int j = 0; j < cnk[i].count; j++) {
                    out_mem[offset++] = (char)colors[cnk[i].color_index].b;
                    out_mem[offset++] = (char)colors[cnk[i].color_index].g;
                    out_mem[offset++] = (char)colors[cnk[i].color_index].r;
                }
            }
            return 0;
        }
    }
    FILE* out = fopen("output.bmp", "wb+");

    BMPFILEHEADER* file_header = get_bmp_file_header();
    fwrite(file_header, sizeof(BMPFILEHEADER), 1, out);
    free(file_header);

    BMPINFOHEADER* info_header = get_bmp_info_header();
    fwrite(info_header, sizeof(BMPINFOHEADER), 1, out);
    free(info_header);

    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }

    fwrite(out_mem, obj->width*obj->height*3, 1, out);

    fclose(out);

    munmap(out_mem, obj->width*obj->height*3);

    munmap(file, file_size);

    time = clock() - time;
    printf("Time:%ld\n", time);

    return 0;
}
