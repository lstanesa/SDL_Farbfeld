#include <SDL2/SDL_video.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SDL_Surface *SDL_LoadFarbfeldSurface(const char *filename) {
    uint32_t width, height;
    SDL_Surface *surf;
    uint32_t rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
 
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        return NULL;
    }

    char headbuf[17];

    if (fread(&headbuf, 1, 16, fp) != 16) {
        return NULL;
    } 

    width = ntohl(*(uint32_t*)&headbuf[8]);
    height = ntohl(*(uint32_t*)&headbuf[12]);

    uint8_t *pixbuf = (uint8_t*)malloc(width * height * 4);

    uint16_t * row;
    int i, j, off, rlen;

    rlen = width * 8;
    row = malloc(rlen);

    for(off = 0, i = 0; i < height; i++) {
        fread(row, 1, (size_t)rlen, fp);

        for(j = 0; j < rlen / 2; j+= 4, off += 4) {
            pixbuf[off] = row[j];
            pixbuf[off + 1] = row[j + 1];
            pixbuf[off + 2] = row[j + 2];
            pixbuf[off + 3] = row[j + 3];
        }
    }

    free(row);

    fclose(fp);
    
    surf = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

    SDL_LockSurface(surf);
    
    memcpy(surf->pixels, pixbuf, (width * height * 4));

    SDL_UnlockSurface(surf);

    free(pixbuf);

    return surf;
}
