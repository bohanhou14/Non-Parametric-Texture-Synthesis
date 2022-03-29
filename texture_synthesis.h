#ifndef TEXTURE_SYNTHESIS_H
#define TEXTURE_SYNTHESIS_H
#include "image.h"

/** A struct storing information about a to-be-synthesized pixel*/
typedef struct
{
	/** The index of the pixel to be synthesized*/
	PixelIndex idx;
	
	/** The number of neighboring pixels in a set state*/
	unsigned int neighborCount;
	
	/** A member for storing a value used to resolve ties*/
	unsigned int r;
} TBSPixel;

/** A function that compares two TBSPixels and returns a negative number if the first should come earlier in the sort order and a positive number if it should come later*/
int CompareTBSPixels( const void *v1 , const void *v2 );

/** A function that sorts an array of TBSPixels*/
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz );

/** A function that extends the exemplar into an image with the specified dimensions, using the prescribed window radius -- the verbose argument is passed in to enable logging to the command prompt, if desired*/
Image *SynthesizeFromExemplar( const Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius);

/**A function checking if all the pixels in an image is set. Return the number of TBSPixels or 0 if the image is set*/
int IsImageSet(const Image *synthesized);

/**A function checking if a pixel is TBS and return the number of neighboring set pixels*/
int IsTBSPixel(const Image *synthesized, PixelIndex index);

/**A function that returns the list of indices of TBSPixels on the synthesized image*/
TBSPixel *GetTBSPixelList(const Image *synthesized, const int NumTBSPixels);

/**A function that takes a TBSPixelIndex as input returns a list of its matching pixels on the exemplar image*/
Pixel ** FindMatches (const Image *exemplar, Image * synthesized, PixelIndex TBSPixelindex, int ** SetPixels, unsigned int windowRadius,int* sizeofBestMatch);

/**A function that takes a pixelindex as input returns a list of its neighboring set pixels*/
int ** GetWindowList(const Image *synthesized, PixelIndex index, unsigned int windowRadius);

/**A function that takes a neighbor pixel's relative index within the window, and returns its absolute index on the image*/
PixelIndex GetAbsolutePixelIndex(const Image * image, PixelIndex centerIndex, int m, int n, int windowRadius);

/**A function that computes the GaussianWeight of a pixel on a window from the central pixel of that window*/
double GaussianWeight(int relativeIndex_x, int relativeIndex_y, unsigned int windowRadius);

/**A function that chooses a BestMatch from a list of BestMatches*/
Pixel* chooseRandom(Pixel** BestMatches,int size);

#endif // TEXTURE_SYNTHESIS_INCLUDED