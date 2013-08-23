
#include "image.h"

#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <string.h>
#include <math.h>

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

METHODDEF(void) my_error_exit (j_common_ptr cinfo);

METHODDEF(void) my_error_exit (j_common_ptr cinfo) {
	(struct my_error_mgr *) myerr = (my_error_ptr) cinfo->err;
	longjmp(myerr->setjmp_buffer, 1);
}

Image *createImage(unsigned int width, unsigned int height) {
	Image *result = (Image *) malloc(sizeof(Image));
	result->canvas = (Pixel *) malloc(sizeof(Pixel) * width * height);
	result->width = width;
	result->height = height;
	return result;
}

void disposeImage(Image *image) {
	free(image->canvas);
	free(image);
}

inline Pixel * pixelAt(Image *image, unsigned int x, unsigned int y) {
	if(x > image->width) {
		return pixelAt(image, x - 1, y);
	}
	if(y > image->height) {
		return pixelAt(image, x, y - 1);
	}
	return &(image->canvas[(y * image->width) + x]);
}

void gdResample(Image *dst, Image *src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH) {
	int x, y;
	double sy1, sy2, sx1, sx2;
	
	for (y = dstY; (y < dstY + dstH); y++) {
		sy1 = ((double) y - (double) dstY) * (double) srcH / (double) dstH;
		sy2 = ((double) (y + 1) - (double) dstY) * (double) srcH / (double) dstH;
		for (x = dstX; (x < dstX + dstW); x++) {
			double sx, sy;
			double spixels = 0;
			double red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
			sx1 = ((double) x - (double) dstX) * (double) srcW / dstW;
			sx2 = ((double) (x + 1) - (double) dstX) * (double) srcW / dstW;
			sy = sy1;
			do {
				double yportion;
				if(floor (sy) == floor (sy1)) {
					yportion = 1.0 - (sy - floor (sy));
					if(yportion > sy2 - sy1) {
						yportion = sy2 - sy1;
					}
					sy = floor (sy);
				}
				else if(sy == floor (sy2)) {
					yportion = sy2 - floor (sy2);
				} else {
					yportion = 1.0;
				}
				sx = sx1;
				do {
					double xportion;
					double pcontribution;
					
					Pixel p;
					
					if(floor (sx) == floor (sx1)) {
						xportion = 1.0 - (sx - floor (sx));
						if(xportion > sx2 - sx1) {
							xportion = sx2 - sx1;
						}
						sx = floor (sx);
					}
					else if (sx == floor (sx2)) {
						xportion = sx2 - floor (sx2);
					} else {
						xportion = 1.0;
					}
					
					pcontribution = xportion * yportion;
					p = *pixelAt(src, floor(sx + srcX), floor(sy + srcY));
					red += p.red * pcontribution;
					green += p.green * pcontribution;
					blue += p.blue * pcontribution;
					spixels += xportion * yportion;
					
					sx += 1.0;
					
				} while (sx < sx2);
				
				sy += 1.0;
				
			} while (sy < sy2);
			
			if(spixels != 0.0) {
				red /= spixels;
				green /= spixels;
				blue /= spixels;
				alpha /= spixels;
			}
			
			if(red > 255.0) red = 255.0;
			if(green > 255.0) green = 255.0;
			if(blue > 255.0) blue = 255.0;
			
			Pixel *p = pixelAt(dst, x, y);
			p->red = red;
			p->green = green;
			p->blue = blue;
		}
	}
}

Image *resample(Image *source, unsigned int target_width, unsigned int target_height) {
	Image *destination = createImage(target_width, target_height);
	gdResample(destination, source, 0, 0, 0, 0, target_width, target_height, source->width, source->height);
	return destination;
}

Image *openJpeg(const char *filename) {
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	JSAMPARRAY buffer;
	FILE *infile;
	unsigned int row_stride;
	
	// Пытаемся открыть файл на чтение
	if((infile = fopen(filename, "rb")) == 0) {
		return 0;
	}
	
	// Задаём обработчик ошибок
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	
	// Произошла ли какая-то ошибка?
	if(setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(& cinfo);
		fclose(infile);
		return 0;
	}
	
	// Инициализация декомпрессора
	jpeg_create_decompress(& cinfo);

	// Указываем источник данных
	jpeg_stdio_src(& cinfo, infile);
	
	// Считываем заголовок
	jpeg_read_header(& cinfo, TRUE);
	
	// Выполняем 
	jpeg_start_decompress(& cinfo);
	
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) & cinfo, JPOOL_IMAGE, row_stride, 1);
	
	Image *result = createImage(cinfo.output_width, cinfo.output_height);
	
	while(cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(& cinfo, & buffer, 1);
		memcpy((& result->canvas[(cinfo.output_scanline - 1) * cinfo.output_width]), buffer, row_stride);
	}
	
	// Прибираемся
	jpeg_finish_decompress(& cinfo);
	jpeg_destroy_decompress(& cinfo);
	fclose(infile);
	
	return result;
}

void saveAsJpeg(Image *image, const char *filename, unsigned short int quality) {
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *outfile;
	JSAMPROW row_pointer[1];
	int row_stride;
	
	cinfo.err = jpeg_std_error(& jerr);
	jpeg_create_compress(& cinfo);
	
	if((outfile = fopen(filename, "wb")) == 0) {
		return;
	}
	
	jpeg_stdio_dest(& cinfo, outfile);

	cinfo.image_width = image->width;
	cinfo.image_height = image->height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	
	jpeg_set_defaults(& cinfo);
	
	jpeg_set_quality(& cinfo, quality, TRUE);
	jpeg_start_compress(& cinfo, TRUE);

	while(cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & (image->canvas[cinfo.next_scanline * image->width]);
		(void) jpeg_write_scanlines(& cinfo, row_pointer, 1);
	}
	
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);	
}
