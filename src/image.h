
/**
* Gallerizer — an image gallery indexing tool
* CLI version (pure libjpeg, no GD required)
* © 2012 Ilya I. Averkov (WST) <admin@jsmart.web.id>
*/

#ifndef IMAGE_H
#define IMAGE_H

#include <stdlib.h>
#include <stdint.h>

typedef struct Pixel {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} __attribute__((__packed__)) Pixel;

typedef struct Image {
	Pixel *canvas __attribute__((__packed__));
	unsigned int width;
	unsigned int height;
} __attribute__((__packed__)) Image;

inline Pixel * pixelAt(Image *image, unsigned int x, unsigned int y);
Image *createImage(unsigned int width, unsigned int height);
void disposeImage(Image *image);
Image *resample(Image *source, unsigned int target_width, unsigned int target_height);
Image *openJpeg(const char *filename);
void saveAsJpeg(Image *image, const char *filename, unsigned short int quality);

#endif
