#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include <math.h>

double PixelSquaredDifference( Pixel * p1 , Pixel * p2 )
{
	double d2 = 0;
	d2 += ((int)(p1->r)-(int)(p2->r)) * ((int)(p1->r)-(int)(p2->r));
	d2 += ((int)(p1->g)-(int)(p2->g)) * ((int)(p1->g)-(int)(p2->g));
	d2 += ((int)(p1->b)-(int)(p2->b)) * ((int)(p1->b)-(int)(p2->b));
	return d2;
}


bool InBounds(const Image *synthesized, PixelIndex idx )
{
	if (idx.x < 0 || idx.y < 0) {
		return 0;
	}
	return ((unsigned int)(idx.x)<synthesized->width) && ((unsigned int)(idx.y)<synthesized->height);
}

Pixel *GetPixel( Image *image , PixelIndex idx )
{
	if( !InBounds( image , idx ) )
	{
		fprintf( stderr , "[ERROR] GetPixel: Pixel image out of bounds: (%d,%d) not in [0,%d)x[0,%d)\n" , idx.x , idx.y , image->width , image->height );
		return NULL;
	}
	return image->pixels + idx.y*image->width + idx.x;
}

const Pixel *GetConstPixel( const Image *image , PixelIndex idx )
{
	if( !InBounds( image , idx ) )
	{
		fprintf( stderr , "[ERROR] GetConstPixel: Pixel image out of bounds: (%d,%d) not in [0,%d)x[0,%d)\n" , idx.x , idx.y , image->width , image->height );
		return NULL;
	}
	return image->pixels + idx.y*image->width + idx.x;
}

Image *AllocateImage( unsigned int width , unsigned int height )
{
	Image* image = malloc(sizeof(Image));
	image->width = width;
	image->height = height;
	image->pixels = (Pixel*)malloc(sizeof(Pixel)*width*height);
	return image;
}

void FreeImage( Image **image )
{
	free((*image)->pixels);
	free(*image);
}

